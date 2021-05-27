#include <bluefruit.h>
#include <ble_gap.h>

//#define ENABLE_SERIAL

// Software Timer for blinking RED LED
SoftwareTimer blinkTimer;
//SchedulerRTOS task;
// int dataUpdateInterval = 600000;
// int baconBroadcastInterval = 300000/0.625;

int dataUpdateInterval = 60000;
int baconBroadcastInterval = 250/0.625;

const uint8_t CUSTOM_UUID[] =
{
    0xA0, 0xDB, 0xD3, 0x6A, 0x00, 0xA6, 0xF7, 0x8C,
    0xE7, 0x11, 0x8F, 0x71, 0x1A, 0xFF, 0x67, 0xDF
};

int counter = 0;

uint8_t msd_payload[5];


BLEUuid uuid = BLEUuid(CUSTOM_UUID);

void setup() 
{
  #ifdef ENABLE_SERIAL
    Serial.begin(115200);
    //while ( !Serial ) delay(10);   // for nrf52840 with native usb
      Serial.println("Bluefruit52 Peripheral Example");
    Serial.println("----------------------------------------\n");
  #endif



  // Initialize blinkTimer for 1000 ms and start it
  blinkTimer.begin(dataUpdateInterval, blink_timer_callback);
  blinkTimer.start();

  if (!Bluefruit.begin())
  {
    #ifdef ENABLE_SERIAL
      Serial.println("Unable to init Bluefruit");
    #endif
    while(1)
    {
      delay(100);
    }
  }
  else
  {
    #ifdef ENABLE_SERIAL
      Serial.println("Bluefruit initialized (peripheral mode)");
    #endif
  }

  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");

  // Set up and start advertising
  startAdv();

  #ifdef ENABLE_SERIAL
    Serial.println("Advertising started"); 
  #endif
  //task.startLoop(tsetFunc);
}

void startAdv(void)
{   
  //Serial.println("Adv loop");
  // Note: The entire advertising packet is limited to 31 bytes!
  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Preferred Solution: Add a custom UUID to the advertising payload, which
  // we will look for on the Central side via Bluefruit.Scanner.filterUuid(uuid);
  // A valid 128-bit UUID can be generated online with almost no chance of conflict
  // with another device or etup
  //Bluefruit.Advertising.addUuid(uuid);

  /*
  // Alternative Solution: Manufacturer Specific Data (MSD)
  // You could also send a custom MSD payload and filter for the 'Company ID'
  // via 'Bluefruit.Scanner.filterMSD(CID);', although this does require a
  // valid CID, which is why the UUID method above is more appropriate in
  // most situations. For a complete list of valid company IDs see:
  // https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
  // For test purposes, 0xFFFF CAN be used, but according to the Bluetooth SIG:
  // > "This value may be used in the internal and interoperability tests before a
  // >  Company ID has been assigned. This value shall not be used in shipping end
  // >  products."
  */
  //uint8_t msd_payload[5]; // Two bytes are required for the CID, so we have 2 bytes user data, expand as needed
  uint16_t msd_cid = 0xFFFF;
  memset(msd_payload, 0, sizeof(msd_payload));
  memcpy(msd_payload, (uint8_t*)&msd_cid, sizeof(msd_cid));
  msd_payload[2] = 0x00;
  msd_payload[3] = 0x00;
  msd_payload[4] = 0x00;
  Bluefruit.Advertising.addData(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, msd_payload, sizeof(msd_payload));

  // Not enough room in the advertising packet for name
  // so store it in the Scan Response instead
  Bluefruit.ScanResponse.addData(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,msd_payload,0x05);
  //Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(baconBroadcastInterval, 244);    // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(0);      // number of seconds in fast mode
  Bluefruit.Advertising.start();
}

void loop() 
{
}

/**
 * Software Timer callback is invoked via a built-in FreeRTOS thread with
 * minimal stack size. Therefore it should be as simple as possible. If
 * a periodically heavy task is needed, please use Scheduler.startLoop() to
 * create a dedicated task for it.
 * 
 * More information http://www.freertos.org/RTOS-software-timer.html
 */
void blink_timer_callback(TimerHandle_t xTimerID)
{
  //Serial.println("changed");
  (void) xTimerID;
  //digitalToggle(LED_RED);
  // data is set , to change it we need to change it in memory
  counter++;
  Bluefruit.ScanResponse.clearData();
  msd_payload[4] = (uint8_t)(counter%256);
  msd_payload[3] = (uint8_t)(counter/256);
  Bluefruit.ScanResponse.addData(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,msd_payload,0x05);

  //Bluefruit.Advertising.stop();
  //startAdv();
}


void tsetFunc(){
  //Serial.println("lol");
}