/*  *****************************************
 *  ADXL345_Calibration
 *  ADXL345 Hook Up Guide Calibration Example 
 *  
 *  Utilizing Sparkfun's ADXL345 Library
 *  Bildr ADXL345 source file modified to support 
 *  both I2C and SPI Communication
 *  
 *  E.Robert @ SparkFun Electronics
 *  Created: Jul 13, 2016
 *  Updated: Sep 13, 2016
 *  
 *  Development Environment Specifics:
 *  Arduino 1.6.11
 *    
 *  Hardware Specifications:
 *  SparkFun ADXL345
 *  Arduino Uno
 *  *****************************************/
 
#include <SparkFun_ADXL345.h>

/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
ADXL345 adxl = ADXL345(5);           // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
//ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

/****************** VARIABLES ******************/
/*                                             */
int AccelMinX = 0;
int AccelMaxX = 0;
int AccelMinY = 0;
int AccelMaxY = 0;
int AccelMinZ = 0;
int AccelMaxZ = 0; 

int accX = 0;
int accY = 0;
int accZ = 0;

/************** DEFINED VARIABLES **************/
/*                                             */
#define offsetX   -123       // OFFSET values
#define offsetY   -16
#define offsetZ   -10

#define gainX     133        // GAIN factors
#define gainY     261
#define gainZ     248 

unsigned int lastRead = 0;

/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup()
{
  Serial.begin(9600);                 // Start the serial terminal
  Serial.println("SparkFun ADXL345 Accelerometer Breakout Calibration");
  Serial.println();
  
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(2);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity
                                      
  adxl.setSpiBit(0);                // Configure the device: 4 wire SPI mode = '0' or 3 wire SPI mode = 1
                                      // Default: Set to 1
                                      // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 
  lastRead = millis();
}

/****************** MAIN CODE ******************/
/*  Accelerometer Readings and Min/Max Values  */
void loop()
{
  // Get the Accelerometer Readings
  int x,y,z;                          // init variables hold results
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store in variables x,y,z


  


  // UNCOMMENT SECTION TO VIEW NEW VALUES
  accX = (x - offsetX)/gainX;         // Calculating New Values for X, Y and Z
  accY = (y - offsetY)/gainY;
  accZ = (z - offsetZ)/gainZ;
  
  int lap = millis()-lastRead;
  //if (lap > 0)
    Serial.println(lap);
    //Serial.println(" ");
  lastRead = millis();

  //Serial.print(x); Serial.print(" "); Serial.print(y); Serial.print(" "); Serial.println(z);
  //Serial.println(); 
  //delay(100);
}
