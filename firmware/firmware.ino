#include <SPI.h> 
#include <SD.h>  
#include <SparkFun_ADXL345.h>

ADXL345 adxl = ADXL345(10);
#define interruptPin 2

const char* filename = "tets.csv";
// SRAM   2k bytes, use haft ot it for data queue. Each read will be store in 3 byte. max queue size = 999
int dataQueue[999];
int queueCounter = 0;
int x,y,z;

void writeLog()
{
    File logFile;
    logFile = SD.open(filename, FILE_WRITE);

    for(int i=0;i<999;i++)
        file.println(dataQueue[i]);
        if((i+1)% 3 == 0)
            file.println("\n");
        else
            file.print(",");
        file.flush();
    queueCounter = 0;
}

void setup() {
    // turn this off for some extra speed
    Serial.begin(9600);
    
    adxl.powerOn();
    adxl.setRangeSetting(16);           // Accepted values are 2g, 4g, 8g or 16g
    adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1

    adxl.setActivityXYZ(1, 1, 1);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
    adxl.setActivityThreshold(128);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
    
    adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
    adxl.setInactivityThreshold(128);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
    adxl.setTimeInactivity(30);         // How many seconds of no activity is inactive?
    adxl.InactivityINT(1);

    attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt

}

void loop() { 
    measure();
    delay(5);
}

void measure() {
    adxl.readAccel(&x, &y, &z);
    dataQueue[queueCounter] = x;
    dataQueue[queueCounter+1] = y;
    dataQueue[queueCounter+2] = z;
    queueCounter+=3;

    if(queueCounter == 999)
        writeLog();
}


void ADXL_ISR()
{
    byte interrupts = adxl.getInterruptSource();
  
    // Inactivity
    if(adxl.triggered(interrupts, ADXL345_INACTIVITY))
    {
        Serial.println("INACTIVITY");
        // sleep code from here
    }
  
    // code will resume from here ? or set active pin to other intp pin?
    // Activity
    if(adxl.triggered(interrupts, ADXL345_ACTIVITY))
    {
        Serial.println("ACTIVITY"); 
        // wake up code from here
        sleep_disable();
        power_all_enable();
    }
}

void enterSleepMode()
{
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
    power_adc_disable();
    power_spi_disable();
    power_timer0_disable();
    power_timer1_disable();
    power_timer2_disable();
    power_twi_disable();

    // if only have serial 
    // UCSR0B &= ~bit (RXEN0);  // disable receiver
    // UCSR0B &= ~bit (TXEN0);  // disable transmitter

    sleep_enable();
    digitalWrite (AWAKE_LED, LOW);
    // interrupts (); nointerrupts (); ??
    sleep_cpu ();      
}