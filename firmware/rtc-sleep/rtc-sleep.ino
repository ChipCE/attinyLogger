
// during an alarm the INT pin of the RTC is pulled low
//
// this is handy for minimizing power consumption for sensor-like devices, 
// since they can be started up by this pin on given time intervals.


#include <Wire.h>
#include "ds3231.h"

#define BUFF_MAX 256
#define WAKE_LOW_PIN 5

uint8_t sleep_period = 1;       // the sleep interval in minutes between 2 consecutive alarms

// how often to refresh the info on stdout (ms)
unsigned long prev = 5000, interval = 5000;

void gotoSleep(unsigned long time)
{
  // shutdown when time reaches SLEEPING_DELAY ms
  if ((time>SLEEPING_DELAY))
  {
    // to reduce power consumption when sleeping, turn off all your LEDs (and other power hungry devices)
    digitalWrite(LED_BUILTIN, LOW);                     

    // setup your wake-up pins.
    pinMode(WAKE_LOW_PIN,  INPUT_PULLUP_SENSE);    // this pin (WAKE_LOW_PIN) is pulled up and wakes up the feather when externally connected to ground.
   
    // power down nrf52.
    sd_power_system_off();                              // this function puts the whole nRF52 to deep sleep (no Bluetooth).  If no sense pins are setup (or other hardware interrupts), the nrf52 will not wake up.
  } 
}


void set_next_alarm(void)
{
    struct ts t;
    unsigned char wakeup_min;

    DS3231_get(&t);

    // calculate the minute when the next alarm will be triggered
    wakeup_min = (t.min / sleep_period + 1) * sleep_period;
    if (wakeup_min > 59) {
        wakeup_min -= 60;
    }

    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm
    // A2M2 (minutes) (0 to enable, 1 to disable)
    // A2M3 (hour)    (0 to enable, 1 to disable) 
    // A2M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[4] = { 0, 1, 1, 1 };

    // set Alarm2. only the minute is set since we ignore the hour and day component
    DS3231_set_a2(wakeup_min, 0, 0, flags);

    // activate Alarm2
    DS3231_set_creg(DS3231_INTCN | DS3231_A2IE);
}

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    DS3231_init(DS3231_INTCN);
    DS3231_clear_a2f();
    set_next_alarm();
    pinMode(5,INPUT);
}

void loop()
{
    char buff[BUFF_MAX];
    unsigned long now = millis();
    struct ts t;

    // once a while show what is going on
    if ((now - prev > interval) && (Serial.available() <= 0)) {
      Serial.print("io state : "); Serial.println(digitalRead(5));
        DS3231_get(&t);

        // display current time
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
             t.mon, t.mday, t.hour, t.min, t.sec);
        Serial.println(buff);

        // display a2 debug info
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);

        if (DS3231_triggered_a2()) {
            Serial.println(" -> alarm2 has been triggered");
            set_next_alarm();
            // clear a2 alarm flag and let INT go into hi-z
            Serial.print("io state : "); 
            Serial.println(digitalRead(5));
            DS3231_clear_a2f();          
        }
        prev = now;
    }
}
