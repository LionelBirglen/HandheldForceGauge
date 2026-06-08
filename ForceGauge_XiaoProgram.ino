/*
  Handheld Force Gauge
  
  Record and translate force values from a force sensor connected to a NAU7802 transducer module.
  Values are displayed on a SSD1306 128x32 OLED display connected to the i2c bus as the NAU7802 transducer.
  Two tactile switch connected to digital input D2 and D9 serve as a user interface and each button 
  has two functions which are triggered by either a short or long push of the button. Values are 
  broadcasted on Bluetooth.
  
  Default values and calibration data are for use with a 100kg force sensor.
  
  by Prof. Lionel Birglen
  Polytechnique Montreal, 2025
*/

#include <Wire.h>
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PushButtonTaps.h>
#include <ArduinoBLE.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32  //Yes the screen actual height is 64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

//OLED object creation
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Force gauge interface object creation
NAU7802 myScale;

//Pushbutton object creation
PushButtonTaps pushBtn1;
PushButtonTaps pushBtn2;

//Pin number for pushbutton
int pushButton1 = D2;
int pushButton2 = D9;

//Constant definition
const double kgs = 0.101936799184506;         //kgs to newtons coefficient
const unsigned long inhibitUnitChange = 500;  //when switching units, how long before another change can be made
const double newtons = 1;                     //Newtons to newtons coefficient lol
const int n_samples = 120;                    //Number of measurements to be averages

//Variable definition
int activeSerial = 1;
int activeBluetooth = 1;
int operationMode = 1;                        //averaging or peak mode
unsigned long lastUnitSwitch = 0;             //time of last unit switch
double measUnit = 1;                          //index of unit
int32_t currentBias = 5500;//1200;            //offset reading, updated when pussing the zeroing button
const double m_scale = 0.000228;//0.000227;   //force scaling
int32_t currentReading = 0;                   //current force readings unitless
int32_t maxReading = 0;                       //current max value recorded (long integer)
int32_t minReading = 2147483647;              //current min value recorded (long integer)
double currentReadingFloat = 0;               //current force reading with unit
double minReadingFloat = 0;                   //current min value recorded (double float)
double maxReadingFloat = 0;                   //current max value recorded (double float)
uint8_t orientation = 0;                      //Integer for screen orientation

//Bluetooth definitions
BLEService scaleService("19B10010-E8F2-537E-4F6C-D104768A1214");  // Custom 128-bit UUIDs (can be changed)
BLEFloatCharacteristic forceCharacteristic(
  "19B10011-E8F2-537E-4F6C-D104768A1214",
  BLERead | BLENotify
);

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println("Force Gauge Sensing");

  //Bluetooth intialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    activeBluetooth=0;
  }

  BLE.setLocalName("Force Gauge");          //Bluetooth device name
  BLE.setAdvertisedService(scaleService);   //Service advertisement
  scaleService.addCharacteristic(forceCharacteristic);
  BLE.addService(scaleService);
  forceCharacteristic.writeValue(0.0f);   // initial value
  BLE.advertise();                        // start advertising

  Serial.println("Bluetooth ready, advertising force sensor");

  //Pushbutton definition as active high
  pushBtn1.setButtonPin(pushButton1);
  pushBtn1.setButtonActiveLow(false);
  pushBtn2.setButtonPin(pushButton2);
  pushBtn2.setButtonActiveLow(false);

  //I2C Initialization
  Wire.begin();

  //Generate 3.3V display voltage internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay(); //Start by clearing the screen

  //Force gauge initialization
  if (myScale.begin() == false)
  {
    Serial.println("Scale not detected. Stopping here.");
    while (1);                                              //Stop everything
  }
  Serial.println("Scale detected!");
  display.setTextColor(SSD1306_WHITE);
}

void loop()
{
  unsigned long currentTime = millis();

  if (!Serial) {activeSerial=0;} else {activeSerial=1;};

  BLE.poll();   //keep BLE stack alive

  //Read pushbuttons
  byte tap1 = pushBtn1.checkButtonStatus();
  byte tap2 = pushBtn2.checkButtonStatus();

  //Debug outputs:
//  Serial.print("Button 1: ");
//  Serial.println(tap1);
//  Serial.print("Button 2: ");
//  Serial.println(tap2);

  //Force sensor read 
  if (myScale.available() == true)
  {
    //Read n_samples times the force sensor and sum all values
    for (int i = 1; i <= n_samples; i++) {
      currentReading += myScale.getReading();
      delay(1);
    }

    //If zeroing button is pushed, set current bias to the current average reading
    if (tap1 == 1) {
      currentBias = -(currentReading / n_samples);
      maxReading = 0;
      minReading = 2147483647;
    }

    //If unit change button is pressed, switch units and deactivate function for 0.5s
    if (tap2 == 1) {
      if (currentTime - lastUnitSwitch > inhibitUnitChange) {
        if (measUnit == newtons) measUnit = kgs;
        else measUnit = newtons;
        lastUnitSwitch = currentTime;
      }
    }

    //If long press on zeroing button, switch screen orientation
    if (tap1 == 3) {
      switch (orientation) {
        case 0:
          orientation = 2;
          break;
        case 2:
          orientation = 0;
          break;
      }
      display.setRotation(orientation);
    }

    //If long press on unit button, switch operation mode
    if (tap2 == 3) {
      switch (operationMode) {
        case 1:
          operationMode = 2;        
          break;
        case 2:
          operationMode = 1;
          break;
      }
    }

    //Average all measurements and convert to newtons 
    currentReading = (currentReading / n_samples) + currentBias;
    currentReadingFloat = measUnit * m_scale * currentReading;

    //Compare to previously stored peak values
    if (operationMode==2) {
      if (currentReading>maxReading) {maxReading=currentReading;maxReadingFloat=measUnit * m_scale * maxReading;}
      if (currentReading<minReading) {minReading=currentReading;minReadingFloat=measUnit * m_scale * minReading;}  
    }
    
    //Publish readings on serial port
    Serial.print("Active Serial / Bluetooth : ");
    Serial.print(activeSerial);
    Serial.print("/");
    Serial.print(activeBluetooth);
    Serial.println();
    Serial.print("Current bias : ");
    Serial.println(currentBias);
    Serial.print("Raw reading in current mode: ");
    Serial.println(currentReading);
    Serial.println("Min/Max Raw reading: ");
    Serial.print(minReading);
    Serial.print("/");
    Serial.println(maxReading);
    Serial.print("Average reading with current unit: ");
    Serial.println(currentReadingFloat);
    Serial.println("Min/Max reading with current unit: ");
    Serial.print(minReadingFloat);
    Serial.print("/");
    Serial.println(maxReadingFloat);

    //Display on OLED
    display.clearDisplay();
    switch (operationMode) {
        case 1:
          display.setCursor(0, 0);
          display.setTextSize(3);
          char buf[64];
          sprintf(buf, "%.2f", currentReadingFloat);
          if (currentReading < 0)
          {
            display.println(buf);
          }
          else
          {
            display.print('+');   //Add a + sign if value is positive, cleaner to look at
            display.println(buf);
          } 
          break;
        case 2:
          display.setCursor(0, 0);
          display.setTextSize(2);
          char buf2[64];
          sprintf(buf2, "%.2f", minReadingFloat);
          if (minReading < 0)
          {
            display.println(buf2);
          }
          else
          {
            display.print('+');   //Add a + sign if value is positive, cleaner to look at
            display.println(buf2);
          }   
          display.setCursor(0, 18);
          display.setTextSize(2);
          char buf3[64];
          sprintf(buf3, "%.2f", maxReadingFloat);
          if (maxReading < 0)
          {
            display.println(buf3);
          }
          else
          {
            display.print('+');   //Add a + sign if value is positive, cleaner to look at
            display.println(buf3);
          }   
          break;
      }
    

    //Display current unit onscreen
    display.setTextSize(1);
    switch (operationMode) {
      case 1:
      if (measUnit == newtons) {
        display.setCursor(0, 24);
        display.print(" N");
      } 
      else {
        display.setCursor(0, 24);
        display.print("KG");
      }
    break;
    case 2:
    if (measUnit == newtons) {
        display.setCursor(111, 0);
        display.print(" N");
      } 
      else {
        display.setCursor(111, 0);
        display.print("KG");
      }
    break;
    }
    
    //Display information
    display.setTextSize(1);
    switch (operationMode) {
      case 1:
      display.setCursor(23, 24);
      display.print("Averaging");
      if (activeBluetooth==1) {
        display.setCursor(85, 24);
        display.print("BLE ");
      }
      if (activeSerial==1) {
        display.setCursor(105, 24);
        display.print("SER");
      }
      break;
      case 2:
        display.setCursor(102, 8);
        display.print("Peak");
        if (activeBluetooth==1) {
        display.setCursor(105, 17);
        display.print("BLE");
      }
      if (activeSerial==1) {
        display.setCursor(105, 25);
        display.print("SER");
      }
      break;
    }

    //Put everything on screen
    display.display();
  }

  //Bluetooth update
  forceCharacteristic.writeValue((float)currentReadingFloat);
}
