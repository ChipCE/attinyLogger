#include <SPI.h> 
//#include <SD.h>  
#include <SparkFun_ADXL345.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <SdFat.h>

#define SD_CS_PIN 4
#define ADXL_CS_PIN 5
#define STOP_PIN 17
#define ENABLE_SERIAL_PIN 16


ADXL345 adxl;// = ADXL345(ADXL_CS_PIN);
SdFat SD;

unsigned int lastReadMillis;
int x=0,y=0,z=0;
int lastReadX=0,lastReadY=0,lastReadZ=0;
int dataQueue[200];
int queuePos = 0;

bool dataChanged(int x,int y,int z,int lap)
{
    if ((x != lastReadX) || (y != lastReadY) || (z != lastReadZ))
    {
        lastReadX = x;
        lastReadY = y;
        lastReadZ = z;
        return true;
    }
    return false;
}

void appendToQueue(int x,int y,int z,int lap)
{
    dataQueue[queuePos] = lap - lastReadMillis;
    dataQueue[queuePos+1] = x; 
    dataQueue[queuePos+2] = y;
    dataQueue[queuePos+3] = z;

    queuePos = queuePos + 4;
    lastReadMillis = lap;

}

void readSensor()
{
    int x,y,z;   
    adxl.readAccel(&x, &y, &z);

    if(Serial)
    {
        Serial.print(x); Serial.print(",");
        Serial.print(y); Serial.print(",");
        Serial.println(z);
    }

    // log if data changed, ignore time change because MCU can read the data for under 1ms
    if(dataChanged(x,y,z,0))
        appendToQueue(x,y,z,millis());
}

void exportLog()
{
    if(queuePos == 200)
    {
        if (!SD.begin(SD_CS_PIN)) {
            if(Serial)
                Serial.println("SD failed");
            while (1);
        }
        File logFile;

        logFile = SD.open("test.txt", FILE_WRITE);

        for(int i=0;i<200;i++)
        {
            //Serial.print(dataQueue[i]);
            logFile.print(dataQueue[i]);
            if (i !=0 && (i+1)%4 == 0)
                logFile.print("\n");
            else
                logFile.print(",");
        }
        queuePos = 0;
        //Serial.println("Exported");
        logFile.close();
        adxl = ADXL345(ADXL_CS_PIN);
    }
}

void exportTimeStamp()
{
    if (!SD.begin(SD_CS_PIN)) {
        if(Serial)
            Serial.println("SD failed");
        while (1);
    }
    File logFile;

    logFile = SD.open("test.txt", FILE_WRITE);
    tmElements_t timeObj;
    if (RTC.read(timeObj))
    {
        //tmYearToCalendar(timeObj.Year),timeObj.Month,timeObj.Day,timeObj.Hour,timeObj.Minute,timeObj.Second);
        logFile.print("#");
        logFile.print(tmYearToCalendar(timeObj.Year));
        logFile.print("-");
        logFile.print(timeObj.Month);
        logFile.print("-");
        logFile.print(timeObj.Day);
        logFile.print("_");
        logFile.print(timeObj.Hour);
        logFile.print(":");
        logFile.print(timeObj.Minute);
        logFile.print(":");
        logFile.println(timeObj.Second);
    }
    else 
    {
        if (RTC.chipPresent())
            logFile.println("no time set");
        else
            logFile.println("RTC-err");
    }
    logFile.close();
    adxl = ADXL345(ADXL_CS_PIN);
}


void setup() {
    pinMode(STOP_PIN,INPUT_PULLUP);
    pinMode(ENABLE_SERIAL_PIN,INPUT_PULLUP);
    if(!digitalRead(ENABLE_SERIAL_PIN))
    {
        Serial.begin(9600);
        Serial.println("Ready");
    }

    //printTime();
    // write timestamp once
    exportTimeStamp();

    adxl.powerOn();
    adxl.setRangeSetting(8);           // Accepted values are 2g, 4g, 8g or 16g
    adxl.setSpiBit(0);                  // Set SPI mode
    adxl = ADXL345(ADXL_CS_PIN); // this will need to be recalled after sd init/read/write
    //Serial.println("Init completed");
}

void stop()
{
    if (!digitalRead(STOP_PIN))
    {
        if(Serial)
            Serial.println("Stop");
        while(1);
    }
}

void loop() { 
    readSensor();
    exportLog();
    stop();
}
