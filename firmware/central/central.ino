#include <string.h>
#include <bluefruit.h>


// Custom UUID used to differentiate this device.
// Use any online UUID generator to generate a valid UUID.
// Note that the byte order is reversed ... CUSTOM_UUID
// below corresponds to the follow value:
// df67ff1a-718f-11e7-8cf7-a6006ad3dba0
const uint8_t CUSTOM_UUID[] =
{
    0xA0, 0xDB, 0xD3, 0x6A, 0x00, 0xA6, 0xF7, 0x8C,
    0xE7, 0x11, 0x8F, 0x71, 0x1A, 0xFF, 0x67, 0xDF
};

BLEUuid uuid = BLEUuid(CUSTOM_UUID);

unsigned long lastRead;
unsigned long minTime = 0;
unsigned long maxTime = 0;
unsigned long sum;
float avg =0;
unsigned long count = 0;

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("#Bluefruit52 Central Example");
  Serial.println("#-------------------------------------\n");

  /* Enable both peripheral and central modes */
  if ( !Bluefruit.begin(1, 1) )
  {
    Serial.println("#Unable to init Bluefruit");
    while(1)
    {
      digitalToggle(LED_RED);
      delay(100);
    }
  }
  else
  {
    Serial.println("#Bluefruit initialized (central mode)");
  }
  
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  /* Set the device name */
  Bluefruit.setName("CENTRAL");

  /* Set the LED interval for blinky pattern on BLUE LED */
  Bluefruit.setConnLedInterval(250);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Filter out packet with a min rssi
   * - Interval = 100 ms, window = 50 ms
   * - Use active scan (used to retrieve the optional scan response adv packet)
   * - Start(0) = will scan forever since no timeout is given
   */

  lastRead = millis();

  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.filterRssi(-80);            // Only invoke callback for devices with RSSI >= -80 dBm
  //Bluefruit.Scanner.filterUuid(uuid);           // Only invoke callback if the target UUID was found
  Bluefruit.Scanner.filterMSD(0xFFFF);          // Only invoke callback when MSD is present with the specified Company ID
  Bluefruit.Scanner.setInterval(160, 80);       // in units of 0.625 ms
  Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds
  Serial.println("#Scanning ...");
}

/* This callback handler is fired every time a valid advertising packet is detected */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // benchmark 
  unsigned lap = millis() -lastRead;
  lastRead = millis();
  sum = sum + lap;
  count++;
  avg = (float)sum/(float)lap;

  if(maxTime < lap)
    maxTime = lap;

  if(minTime > lap || minTime == 0)
    minTime = lap;


  //printPlot(lap);
  //printBechmarkData();

  //Serial.println("scan callback");

  /* Display the timestamp and device address */
  if (report->type.scan_response)
  {
    //Serial.printf("[SRP%9d] Packet received from ", millis());
    //printData(report);
    printJsonData(report);
  }
  else
  {
    //Serial.printf("[ADV%9d] Packet received from ", millis());
    //printData(report);
  }

  //printData(report);
  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}

void printJsonData(ble_gap_evt_adv_report_t* report){
  uint8_t buffer[32];
  uint8_t len = 0;
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
  if (len)
  {
    // only need the last 2 byte
    int counter = buffer[3]*256 + buffer[4];
    float battVoltage = ((float)(buffer[5]*256 + buffer[6]))*2*3.3/1024;
    //int battVoltage = buffer[5]*256 + buffer[6];
    Serial.print("{\"counter\":");
    Serial.print(counter);
    Serial.print(",\"voltage\":");
    Serial.print(battVoltage);
    Serial.println("}");

    memset(buffer, 0, sizeof(buffer));
  }
}


void printData(ble_gap_evt_adv_report_t* report)
{
  uint8_t len = 0;
  uint8_t buffer[32];
  memset(buffer, 0, sizeof(buffer));
    // MAC is in little endian --> print reverse
  Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
  Serial.println("");
  
  /* Raw buffer contents */
  Serial.printf("%14s %d bytes\n", "PAYLOAD", report->data.len);
  if (report->data.len)
  {
    Serial.printf("%15s", " ");
    Serial.printBuffer(report->data.p_data, report->data.len, '-');
    Serial.println();
  }

  /* RSSI value */
  Serial.printf("%14s %d dBm\n", "RSSI", report->rssi);

  /* Adv Type */
  Serial.printf("%14s ", "ADV TYPE");
  if ( report->type.connectable )
  {
    Serial.print("Connectable ");
  }else
  {
    Serial.print("Non-connectable ");
  }

  if ( report->type.directed )
  {
    Serial.println("directed");
  }else
  {
    Serial.println("undirected");
  }

  /* Shortened Local Name */
  if(Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    Serial.printf("%14s %s\n", "SHORT NAME", buffer);
    memset(buffer, 0, sizeof(buffer));
  }

  /* Complete Local Name */
  if(Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    Serial.printf("%14s %s\n", "COMPLETE NAME", buffer);
    memset(buffer, 0, sizeof(buffer));
  }

  /* TX Power Level */
  if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_TX_POWER_LEVEL, buffer, sizeof(buffer)))
  {
    Serial.printf("%14s %i\n", "TX PWR LEVEL", buffer[0]);
    memset(buffer, 0, sizeof(buffer));
  }

  /* Check for UUID16 Complete List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE, buffer, sizeof(buffer));
  if ( len )
  {
    printUuid16List(buffer, len);
  }

  /* Check for UUID16 More Available List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE, buffer, sizeof(buffer));
  if ( len )
  {
    printUuid16List(buffer, len);
  }

  /* Check for UUID128 Complete List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE, buffer, sizeof(buffer));
  if ( len )
  {
    printUuid128List(buffer, len);
  }

  /* Check for UUID128 More Available List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE, buffer, sizeof(buffer));
  if ( len )
  {
    printUuid128List(buffer, len);
  }  

  /* Check for BLE UART UUID */
  if ( Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE) )
  {
    Serial.printf("%14s %s\n", "BLE UART", "UUID Found!");
  }

  /* Check for DIS UUID */
  if ( Bluefruit.Scanner.checkReportForUuid(report, UUID16_SVC_DEVICE_INFORMATION) )
  {
    Serial.printf("%14s %s\n", "DIS", "UUID Found!");
  }

  /* Check for Manufacturer Specific Data */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
  if (len)
  {
    Serial.printBuffer(buffer, len, '-');
    Serial.println();
    memset(buffer, 0, sizeof(buffer));
  }

  Serial.println();
}

/* Prints a UUID128 list to the Serial Monitor */
void printUuid128List(uint8_t* buffer, uint8_t len)
{
  (void) len;
  Serial.printf("%14s %s", "128-Bit UUID");

  // Print reversed order
  for(int i=0; i<16; i++)
  {
    const char* fm = (i==4 || i==6 || i==8 || i==10) ? "-%02X" : "%02X";
    Serial.printf(fm, buffer[15-i]);
  }

  Serial.println();  
}


/* Prints a UUID16 list to the Serial Monitor */
void printUuid16List(uint8_t* buffer, uint8_t len)
{
  Serial.printf("%14s %s", "16-Bit UUID");
  for(int i=0; i<len; i+=2)
  {
    uint16_t uuid16;
    memcpy(&uuid16, buffer+i, 2);
    Serial.printf("%04X ", uuid16);
  }
  Serial.println();
}

void printBechmarkData(){
  Serial.print(avg);
  Serial.print("\t");

  Serial.print(minTime);
  Serial.print("\t");

  Serial.print(maxTime);
  Serial.print("\t");
}

void printPlot(unsigned long value){
  Serial.println(value);
}

void loop() 
{

}
