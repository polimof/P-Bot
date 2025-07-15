#include <WiFiManager.h>
WiFiManager wifiManager;

#include "additional.h"
#include <ESP32Servo.h>
Servo myservo1;

#define sleep_Time 45

#include <Preferences.h>
Preferences preferences;

#include <esp_task_wdt.h>
//3 seconds WDT
#define WDT_TIMEOUT 10000
//if 1 core doesn't work, try with 2
#define CONFIG_FREERTOS_NUMBER_OF_CORES 1 
esp_task_wdt_config_t twdt_config = {
        .timeout_ms = WDT_TIMEOUT,
        .idle_core_mask = (1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1,    // Bitmask of all cores
        .trigger_panic = true,
    };


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
const long utcOffsetInSeconds = 8 * 3600;  // Adjust this based on your timezone.... 7 is for malaysia
long utcOffset = 0;
WiFiUDP ntpUDP;


#include "RTClib.h"
#include "time.h"
RTC_DS3231 rtc;

int induce_state = 0;
int time_display = 0;
int induce_count = 0;

const int buzzer = 16;  // Change this to your buzzer pin

//const char* ssid = "OnePlus Nord";//"OnePlus Nord";
//const char* password = "1234567890";

//char ssid = "OnePlus Nord";//"OnePlus Nord";
//char password = "1234567890";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 8*3600;
const int   daylightOffset_sec = 3600;

char daysOfTheWeek[7][12] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

int year = 0;
int month = 0;
int day = 0;
int hour = 0;
int minute = 0;
int second = 0;
//////////////////////////////////////////////////////////////////////////////
// Note definitions
////////////////////////////////////////////////////////////////////////////////
#define right_Sensor 27
#define left_Sensor 12
#define ir_Sensor 14

int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SDA_1 21
#define SCL_1 22

#define SDA_2 33
#define SCL_2 32
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire,-1,400000,100000);//important to create this
Adafruit_SSD1306 oled2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1,-1,400000,100000);//important to create this

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#include "FastIMU.h"
#define IMU_ADDRESS 0x69    //Change to the address of the IMU
MPU6500 IMU;               //Change to the name of any supported IMU! 

calData calib = { 0 };  //Calibration data
AccelData accelData;    //Sensor data
GyroData gyroData;
MagData magData;

#include <QMC5883LCompass.h>

QMC5883LCompass compass;

float x, y, z, x_a, y_a, z_a;

float xa_diff = 0;
float ya_diff = 0;
float za_diff = 0;

float previous_x = 0;
float previous_y = 0;
float previous_z = 0;

float previous_x_a = 0;
float previous_y_a = 0;
float previous_z_a = 0;

#include <DHT11.h>

// Create an instance of the DHT11 class.
// - For Arduino: Connect the sensor to Digital I/O Pin 2.
// - For ESP32: Connect the sensor to pin GPIO2 or P2.
// - For ESP8266: Connect the sensor to GPIO2 or D4.
DHT11 dht11(15);

int temperature = 0;
    int humidity = 0;



////////////////////////////time of flight sensor////////////////////////////////////////
#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

float range = 0;
int angry_count = 0;
///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time 
unsigned long previousMillis2 = 0;   
//unsigned long previousMillis3 = 0;   
//unsigned long previousMillis4 = 0;  

unsigned long watch_Millis = 0;
unsigned long watch_Millis2 = 0;
//unsigned long watch_Millis3 = 0;
//unsigned long watch_Millis4 = 0;

int interval = 3000;
int interval2 = 600000;
int interval3 = 180000;
int interval4 = 1800;

long randNumber = 0;
long randNumber2 = 0;
long randNumber3 = 0;

long t1_gap = 0;
long t2_gap = 0;
long t3_gap = 0;
long t4_gap = 0;
int t5_gap = 0;
int t6_gap = 0;

float cl = 0;
float al = 0;
float bl = 0;

int a = 0;
int b = 0;
int c = 0;
int sel = 0;
int e = 0;
int ans = 0;
int right_Answer = 0;
/////////////////////////////////////////////////////////////////////////////

int trig_Game = 0;
int trig_Menu = 0;

//////////////////////////////////////////////////timer mata tutup auto////////////////////////////////////////////////////////////
unsigned long currentMillis = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////wifi manager//////////////////////////////////
void handlePreOtaUpdateCallback(){
  Update.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("CUSTOM Progress: %u%%\r", (progress / (total / 100)));
  });
}
/////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
void showTime()
{
  Serial.print("time");
  DateTime now = rtc.now();
  
  
        Serial.print(now.year(), DEC);
        Serial.print('/');
        Serial.print(now.month(), DEC);
        Serial.print('/');
        Serial.print(now.day(), DEC);
        Serial.print(" (");
        Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
        Serial.print(") ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();
        delay(100);

        

        
}

void showTime_oled()
{
  oled2.clearDisplay();
  //oled2.setRotation(2);
  oled2.setTextSize(2);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(0,10);             // Start at top-left corner

display.clearDisplay();
  //oled2.setRotation(2);
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,10);             // Start at top-left corner

  //oled.print("time");
  DateTime now = rtc.now();
  
  
        oled2.print(now.year(), DEC);
        oled2.print('/');
        oled2.print(now.month(), DEC);
        oled2.print('/');
        oled2.print(now.day(), DEC);
        oled2.println();
        oled2.print(" (");
        oled2.print(daysOfTheWeek[now.dayOfTheWeek()]);
        oled2.print(") ");
        oled2.println();
        oled2.print(" -");
        if (now.hour() < 10)
        {oled2.print("0");}
        oled2.print(now.hour(), DEC);
        oled2.print(':');
        if (now.minute() < 10)
        {oled2.print("0");}
        oled2.print(now.minute(), DEC);
        oled2.print("-");
        //oled.print(':');
        //oled.print(now.second(), DEC);
        oled2.println();
        oled2.display();
        delay(100);

        display.print(now.year(), DEC);
        display.print('/');
        display.print(now.month(), DEC);
        display.print('/');
        display.print(now.day(), DEC);
        display.println();
        display.print(" (");
        display.print(daysOfTheWeek[now.dayOfTheWeek()]);
        display.print(") ");
        display.println();
        display.print(" -");
        if (now.hour() < 10)
        {display.print("0");}
        display.print(now.hour(), DEC);
        display.print(':');
        if (now.minute() < 10)
        {display.print("0");}
        display.print(now.minute(), DEC);
        display.print("-");
        //oled.print(':');
        //oled.print(now.second(), DEC);
        display.println();
        display.display();
        delay(100);


  //oled.println(F("Hello, world!"));
        
}

////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
void showTime_oled_big()
{
  oled2.clearDisplay();
  //oled2.setRotation(2);
  oled2.setTextSize(7);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(30,15);             // Start at top-left corner

display.clearDisplay();
  //oled2.setRotation(2);
  display.setTextSize(7);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,15);             // Start at top-left corner

  //oled.print("time");
  DateTime now = rtc.now();
  
  
        //oled2.print(now.year(), DEC);
        //oled2.print('/');
        //oled2.print(now.month(), DEC);
        //oled2.print('/');
        //oled2.print(now.day(), DEC);
        //oled2.println();
        //oled2.print(" (");
        //oled2.print(daysOfTheWeek[now.dayOfTheWeek()]);
        //oled2.print(") ");
        //oled2.println();
        //oled2.print(" -");
        if (now.hour() < 10)
        {display.print("0");}
        display.print(now.hour(), DEC);
        display.print(':');
        //oled2.print(now.minute(), DEC);
        //oled2.print("-");
        //oled.print(':');
        //oled.print(now.second(), DEC);
        display.println();
        display.display();
        delay(100);

        //display.print(now.year(), DEC);
        //display.print('/');
        //display.print(now.month(), DEC);
        //display.print('/');
        //display.print(now.day(), DEC);
        //display.println();
        //display.print(" (");
        //display.print(daysOfTheWeek[now.dayOfTheWeek()]);
        //display.print(") ");
        //display.println();
        //display.print(" -");
        //display.print(now.hour(), DEC);
        //display.print(':');
        if(now.minute() < 10)
        {oled2.print("0");}
        oled2.print(now.minute(), DEC);
        //oled.print(':');
        //oled.print(now.second(), DEC);
        oled2.println();
        oled2.display();
        delay(100);


  //oled.println(F("Hello, world!"));
        
}

////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
void read_temp()
{
  
    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);

    // Check the results of the readings.
    // If the reading is successful, print the temperature and humidity values.
    // If there are errors, print the appropriate error messages.
    if (result == 0) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
    
}
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////check_range////////////////////////////////////
void check_range1()
{
   VL53L0X_RangingMeasurementData_t measure;
   lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) 
  { 
    range = measure.RangeMilliMeter;

  }
  else {
    Serial.println(" out of range ");
    range = 0;
  }

 if((range < 250)&&(range > 200))
   {
    buttonState3 = HIGH;
    angry_count = angry_count + 1;

    if(angry_count > 4)
    {
      
        angry_Face();
        r2d2Whistle2();
        delay(1000);
        angry_count = 0;
        //induce_state = 0;
        //delay(1000);
      

    }
   }

     else if((range < 40)&&(range > 10))
   {
    //buttonState3 = HIGH;
    induce_state = 1;
    Serial.print("induce state :");
  Serial.println(induce_state);

    }

   else
   {buttonState3 = LOW;}

}

void check_range_induce1()
{
   VL53L0X_RangingMeasurementData_t measure;
   lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) 
  { 
    range = measure.RangeMilliMeter;
    Serial.print(" range: ");
    Serial.println(range);

  }
  else {
    Serial.println(" out of range ");
    range = 0;
  }

 if((range < 250)&&(range > 200))
   {
    buttonState3 = HIGH;
    angry_count = angry_count + 1;

    if(angry_count > 4)
    {
      
      
        angry_count = 0;
        //induce_state = 0;
        //delay(1000);

    }
   }

     else if((range < 40)&&(range > 10))
   {
    //buttonState3 = HIGH;
      
    induce_state = 1;
    Serial.print("induce state :");
  Serial.println(induce_state);

    }

   else
   {buttonState3 = LOW;}

}


void check_range()
{
   VL53L0X_RangingMeasurementData_t measure;
   lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) 
  { 
    range = measure.RangeMilliMeter;
  }
  else {
    Serial.println(" out of range ");
    range = 0;
  }

   if((range < 350)&&(range > 200))
   {
    //buttonState3 = HIGH;
    //angry_count = angry_count + 1;
    induce_count = induce_count +1;
    showTime_oled_big();
    delay(700);

    if(induce_count > 2)
    {
      angry_Face();
      r2d2Whistle2();
      delay(1000);
      induce_count = 0;
      induce_state = 1;
      Serial.print("induce state :");
      Serial.println(induce_state);
      buttonState3 = HIGH;
    }

   }
   else
   {
    induce_count = 0;
    buttonState3 = LOW;
    induce_state = 0;
    }
}

void check_range_induce()
{
   VL53L0X_RangingMeasurementData_t measure;
   lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) 
  { 
    range = measure.RangeMilliMeter;
  }
  else {
    Serial.println(" out of range ");
    range = 0;
  }

   if((range < 350)&&(range > 200))
   {
    //buttonState3 = HIGH;
    //angry_count = angry_count + 1;
    induce_count = induce_count +1;
    showTime_oled_big();
    delay(700);

    if(induce_count > 2)
    {
      angry_Face();
      r2d2Whistle2();
      delay(1000);
      induce_count = 0;
      induce_state = 1;
      Serial.print("induce state :");
      Serial.println(induce_state);
      buttonState3 = HIGH;
    }

   }
   else
   {
    induce_count = 0;
    buttonState3 = LOW;
    induce_state = 0;
    }
}
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
void check_dz()
{
  // Read compass values
  compass.read();

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();
  
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.println();

  if( ((x-previous_x) > 180) || ((y-previous_y) > 180) || ((z-previous_z) > 180))// gyro differential check
  {
  confuse_Face();
  display.setRotation(1);
  oled2.setRotation(1);
  delay(200);
  
  confuse_Face();
  display.setRotation(2);
  oled2.setRotation(2);
  delay(200);

  confuse_Face();
  display.setRotation(3);
  oled2.setRotation(3);
  delay(200);

  confuse_Face();
  display.setRotation(0);
  oled2.setRotation(0);
  delay(200);

  confuse_Face();
  display.setRotation(1);
  oled2.setRotation(1);
  delay(200);

  confuse_Face();
  display.setRotation(2);
  oled2.setRotation(2);
  delay(200);
////////////////////////////////////////////////////
r2d2Whistle3();
/////////////////////////////////////////////////////
  confuse_Face();
    display.setRotation(3);
  oled2.setRotation(3);
  delay(200);

  confuse_Face();
  display.setRotation(0);
  oled2.setRotation(0);
  delay(200);
//////////////////////////////////////////////////////////////////////////////////////
r2d2Angry();
///////////////////////////////////////////////////////////////////////////////////////
  }

}


////////////////////////////////////////////////////////////////////////////////////

void check_IMU()
{
  IMU.update();
  
  IMU.getGyro(&gyroData);
  x = gyroData.gyroX;
  y = gyroData.gyroY;
  z = gyroData.gyroZ;

  IMU.getAccel(&accelData);
  x_a = accelData.accelX;
  y_a = accelData.accelY;
  z_a = accelData.accelZ;

  Serial.print(x_a);
  Serial.print("\t");
  Serial.print(y_a);
  Serial.print("\t");
  Serial.print(z_a);
  Serial.print("\t");
  
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");
  Serial.print(range);
  
  
  if( ((x-previous_x) > 180) || ((y-previous_y) > 180) || ((z-previous_z) > 180))
  {
  confuse_Face();
  display.setRotation(1);
  oled2.setRotation(1);
  delay(200);
  
  confuse_Face();
  display.setRotation(2);
  oled2.setRotation(2);
  delay(200);

  confuse_Face();
  display.setRotation(3);
  oled2.setRotation(3);
  delay(200);

  confuse_Face();
  display.setRotation(0);
  oled2.setRotation(0);
  delay(200);

  confuse_Face();
  display.setRotation(1);
  oled2.setRotation(1);
  delay(200);

  confuse_Face();
  display.setRotation(2);
  oled2.setRotation(2);
  delay(200);
////////////////////////////////////////////////////
r2d2Whistle3();
/////////////////////////////////////////////////////
  confuse_Face();
    display.setRotation(3);
  oled2.setRotation(3);
  delay(200);

  confuse_Face();
  display.setRotation(0);
  oled2.setRotation(0);
  delay(200);
//////////////////////////////////////////////////////////////////////////////////////
r2d2Angry();
///////////////////////////////////////////////////////////////////////////////////////
  }

}


////////////////////////////////////////////////////////////////////////////////////

void check_IMU_animate()
{
  IMU.update();
  
  IMU.getGyro(&gyroData);
  x = gyroData.gyroX;
  y = gyroData.gyroY;
  z = gyroData.gyroZ;

  IMU.getAccel(&accelData);
  x_a = accelData.accelX;
  y_a = accelData.accelY;
  z_a = accelData.accelZ;

  xa_diff =(x_a - previous_x_a);
  ya_diff =(y_a - previous_y_a);
  za_diff =(z_a - previous_z_a);


  Serial.print(x_a);
  Serial.print("\t");
  Serial.print(y_a);
  Serial.print("\t");
  Serial.print(z_a);
  Serial.print("\t");
  
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);  
  Serial.print("\t");

  Serial.print("za_diff");
  Serial.print(za_diff);
  Serial.print("\t");

  Serial.print("range");
  Serial.print(range);
  Serial.print("\t");

  //Serial.print(xa_diff);
  //Serial.print("\t");
  //Serial.print(ya_diff);
  //Serial.print("\t");

  if( ((x-previous_x) > 180) || ((y-previous_y) > 180) || ((z-previous_z) > 180))
  {
  confuse_Face();
  display.setRotation(1);
  oled2.setRotation(1);
  delay(200);
  
  confuse_Face();
  display.setRotation(2);
  oled2.setRotation(2);
  delay(200);

  confuse_Face();
  display.setRotation(3);
  oled2.setRotation(3);
  delay(200);

  confuse_Face();
  display.setRotation(0);
  oled2.setRotation(0);
  delay(200);

  confuse_Face();
  display.setRotation(1);
  oled2.setRotation(1);
  delay(200);

  confuse_Face();
  display.setRotation(2);
  oled2.setRotation(2);
  delay(200);
////////////////////////////////////////////////////
r2d2Whistle3();
/////////////////////////////////////////////////////
  confuse_Face();
    display.setRotation(3);
  oled2.setRotation(3);
  delay(200);

  confuse_Face();
  display.setRotation(0);
  oled2.setRotation(0);
  delay(200);
//////////////////////////////////////////////////////////////////////////////////////
r2d2Angry();
///////////////////////////////////////////////////////////////////////////////////////
  }

}


////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////
void kelip()
{
//close eye
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();
  delay(200);

 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  //delay(500);

 

  //say_Hi = 0;
}

void kelip_tepi1()
{
//close eye
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();
  delay(200);

 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_lefteye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_lefteye, 128, 64, 1);
  display.display();
  oled2.display();

  reconfig_Servo(); 
  for (int pos = 90; pos >= 60; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(20);             // waits 15ms for the servo to reach the position
	}
  for (int pos = 60; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(20);             // waits 15ms for the servo to reach the position
	}

  delay(500);

	

r2d2ShortBeep5();
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();
  delay(200);

 

  //say_Hi = 0;
}

void kelip_tepi2()
{
//close eye
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();
  delay(200);

 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_righteye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_righteye, 128, 64, 1);
  display.display();
  oled2.display();
  
  reconfig_Servo(); 
  ///////////////////////////////////////////////////////////////////////////////////
	for (int pos = 90; pos <= 110; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(20);             // waits 15ms for the servo to reach the position
	}
	for (int pos = 110; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(20); 
  }
  delay(200);

r2d2ShortBeep5();
 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

  //say_Hi = 0;
}

void setup() {
  // initialize the pin as an input:
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
   myservo1.setPeriodHertz(50);// Standard 50hz servo
   myservo1.attach(13, 500, 2400);


  pinMode(ir_Sensor, INPUT);
  pinMode(left_Sensor, INPUT);
  pinMode(right_Sensor, INPUT);

  pinMode(buzzer, OUTPUT);
  //pinMode(17, OUTPUT);

  pinMode(2, OUTPUT);

/////////////////////////save gmt value////////////////////////////////////////////
preferences.begin("my-clock", false);

float my_gmt = preferences.getFloat("my_gmt", 0);

  // Print the my_gmt to Serial Monitor
  Serial.printf("Current gmt value: %f\n", my_gmt);

  // Store the value of my_gmt to the Preferences
  //preferences.putFloat("my_gmt", my_gmt);

  // Close the Preferences
  preferences.end();
////////////////////////////////////////////////////////////////////

previousMillis = millis();
//previousMillis2 = previousMillis;
//previousMillis3 = previousMillis;

 Serial.begin(115200);

Wire.begin(SDA_1, SCL_1);//I2C number 1 for the firts robot eye and all other sensors.
Wire1.begin(SDA_2, SCL_2);//I2C number 2 for the second oled robot eye.

display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
oled2.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

delay(2000);//this delay is need so that it can detects the 2 button pressed at the start to enter wifi configuration.

///////////////////////////trigger wifi upon reboot////////////////////////////////
   buttonState1 = digitalRead(right_Sensor);
   buttonState2 = digitalRead(left_Sensor);

/*
   if ((buttonState2 == HIGH) && (buttonState1 == HIGH)) 
  {

    Serial.println("Entering wifi mode.");
    wificonf_Face();
 
    wifiManager.setConfigPortalTimeout(180);
    //wifiManager.setConfigPortalBlocking(false);

    wifiManager.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);
    //wifiManager.resetSettings();
    if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    } 

  Serial.println("connected...yeey :)");
  }

  else
  {
    Serial.println("Entering wifi mode.");
    //wificonf_Face();
 
    wifiManager.setConfigPortalTimeout(1);
    //wifiManager.setConfigPortalBlocking(false);

    wifiManager.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);
    //wifiManager.resetSettings();
    if(!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    } 

  Serial.println("connected...yeey :)");
  }
  /////////////////////////////////////////////////////////////////



//WiFi.SSID() //this contains the SSID
//WiFi.psk() //this contains the password
////////////////////////////////////////////////////////////////////////////////
//WiFi.begin(WiFi.SSID(), WiFi.psk());
  delay(1000);
   Serial.println(WiFi.SSID());
    Serial.println(WiFi.psk());
*/

  Serial.println("RTC begin");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
      while (1) delay(10);
  }
  ////////////////////initialize date from rtc ////////////////////////////
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
  ////////////////////////////////////////////
/*
  utcOffset = my_gmt * 3600;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffset);
  timeClient.begin();
  timeClient.update();
  configTime(utcOffset, daylightOffset_sec, ntpServer);
  if (WiFi.status() == WL_CONNECTED) {

     //rtc.adjust(DateTime(2023, 6, 18, 13, 59, 0));
    //rtc.adjust(DateTime(2024, 12, 21, (timeClient.getHours()), (timeClient.getMinutes()), (timeClient.getSeconds())));
  printLocalTime();
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
  showTime();
  delay(500);
  }
*/
//////////////////////////////////////////////////////////////////////////////
IMU.init(calib, IMU_ADDRESS);  

/*//////////////////////////////////////////////////////////////////////////////
 compass.init();
 compass.read();

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

 previous_x = x;
 previous_y = y;
 previous_z = z;
  ////////////////////////////////////////////////////////////////*/

  ////////////////////////////Time of flight sensor initialize//////////////////
  Serial.println("VL53L0X begin");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
 
  /////////////////////////////////////////////////////////////////////////////////
  
  ////////////////////////watchdog timer///////////////////////////////////////
  Serial.println("Configuring WDT...");
  esp_task_wdt_deinit(); //wdt is enabled by default, so we need to deinit it first
  esp_task_wdt_init(&twdt_config); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
///////////////////////////////////////////////////////////////




  display.clearDisplay();
   oled2.clearDisplay();

  display.setRotation(0);
  oled2.setRotation(0);

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  oled2.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
    oled2.display();

  delay(20);

  currentMillis = millis();
  
  //testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}




void loop() {

  watch_Millis = currentMillis - previousMillis;
  //watch_Millis2 = currentMillis - previousMillis2;
  //watch_Millis3 = currentMillis - previousMillis3;     
  Serial.println(watch_Millis);
  check_IMU();
  check_range();

  ///////////////////////random eye blinking//////////////////////////////////
  if (watch_Millis >= interval) 
  {
    
    ///////////////////////t1_gap/////////////////////////////////////
    t1_gap = t1_gap + 1;
    Serial.println(t1_gap);
      if(t1_gap > sleep_Time)///sleep_Time * 3 = real sleep time
      {
       r2d2FiveBeep4();
       animate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
        previousMillis = millis() - (3*interval);
        sleepy_Face();
         
      }
    ///////////////////////////////////////////////////////////

    /////////////////////////t2_gap///////////////////////////////////////
    t2_gap = t2_gap + 1;
    Serial.println(t2_gap);
    if(t2_gap > 15)///after 50 minutes
    {
      t2_gap = 0;
      randNumber2 = random(0,3);

      if(randNumber2 > 0)
      {
        ////////////////////////////////////
        r2d2Whistle3();
        ///////////////////////////////////
        randNumber3 = random(0,3);
              if(randNumber3 == 0)
              {angry_Face();}
              else if(randNumber3 == 1)
              {close_Face();}
              else if(randNumber3 > 1)
              {sharingan_Face();}
              delay(1500);
        /////////////////////////////////////////////////////////////////////
        //right_Face();
        target_Face();
        delay(50);
        ////////////////////////////////reconfigure timer for sevo/////////////////////
        reconfig_Servo(); 
        ///////////////////////////////////////////////////////////////////////////////////
	      for (int pos = 90; pos <= 130; pos += 1) { // goes from 0 degrees to 180 degrees
		        // in steps of 1 degree
		        myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		        delay(15);             // waits 15ms for the servo to reach the position
	          }
        r2d2ShortBeep4();
        ////////////////////////////////reconfigure timer for sevo/////////////////////
        reconfig_Servo(); 
        ///////////////////////////////////////////////////////////////////////////////////
        //left_Face();
        target_Face();
        delay(50);
        ////////////////////////////////////////////////////////////////////////////
	      for (int pos = 130; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		        myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		        delay(15);             // waits 15ms for the servo to reach the position
	          }
        ///////////////////////////////////////////////////////////////////////////////////
        //left_Face();
        target_Face();
        delay(50);
        /////////////////////////////////////////////////////////////////////////////////////
  	    for (int pos = 90; pos >= 50; pos -= 1) { // goes from 0 degrees to 180 degrees
		        // in steps of 1 degree
		        myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		        delay(15);             // waits 15ms for the servo to reach the position
	          }

        r2d2ShortBeep4();
        ////////////////////////////////reconfigure timer for sevo/////////////////////
        reconfig_Servo();
        ///////////////////////////////////////////////////////////////////////////////////
        //right_Face();
        target_Face();
        delay(50);

	      for (int pos = 50; pos <= 90; pos += 1) { // goes from 180 degrees to 0 degrees
		        myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		        delay(15);             // waits 15ms for the servo to reach the position
	          }

        //starwars();
        ////////////////////////////////////
        trig_Game = 0; // reset the trigger game value
      }
    }

    ///////////////////////t3/////////////////////////////////////
    t3_gap = t3_gap + 1;
    Serial.println(t3_gap);
    if(t3_gap > 20)///after 1 minutes
    {
      t3_gap = 0;
      showTime_oled();
      delay(2000);
      showTime_oled_big();
      r2d2Talking2();
      delay(2000);
    }
    ///////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////

    randNumber = random(0,9);
    if(randNumber > 3)
    {
    kelip();
    r2d2ShortBeep5();
  //showTime_oled();
  //MP3Player.reset();

    }
    else if(randNumber == 2)
    {
    kelip_tepi1();
    r2d2ShortBeep4();
    normal_Face();
    }
    else if(randNumber == 1)
    {
    kelip_tepi2();
    r2d2ShortBeep4();
    normal_Face();
    }
        else if(randNumber == 0)
    {
    //kelip_tepi2();
    r2d2ShortBeep1();
    sus_Face();
    delay(1000);
    }
       else if(randNumber == 3)
    {
    //kelip_tepi2();
    //r2d2ShortBeep1();
    normal_Face();
    }
  
  ////////////////
  //check_IMU();
  //read_temp();
  delay(20);
  previous_x = x;
  previous_y = y;
  previous_z = z;
  previous_x_a = x;
  previous_y_a = y;
  previous_z_a = z;

  previousMillis = previousMillis + interval;
  
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////////

   buttonState1 = digitalRead(right_Sensor);
   buttonState2 = digitalRead(left_Sensor);
   check_range();
   //buttonState3 = digitalRead(ir_Sensor);        

  
  if ((buttonState2 == HIGH) && (buttonState1 == HIGH)) {
  // turn LED on:
  //trig_Game = trig_Game + 1;
  trig_Menu = trig_Menu + 1;
  t1_gap=0;
  digitalWrite(2, HIGH);
  ////////////////////////////////////
  r2d2Whistle3();
  //r2d2ShortBeep1();
  //r2d2Talking1();
  ///////////////////////////////////
  happy_Face();
  ////////////////////////////////////
  r2d2ShortBeep1();
  r2d2Talking1();
  ///////////////////////////////////
  ////////////////////////////////reconfigure timer for sevo/////////////////////
  reconfig_Servo(); 
  ///////////////////////////////////////////////////////////////////////////////////
	for (int pos = 90; pos <= 110; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (int pos = 110; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}

  //////////////////////////////////////////////////menu starts////////////////////////////////////////////////////////////
  if(trig_Menu > 1)
  {
    int menu = 0;
    float gmt = 0;
    menu_Face();

    delay(2000);
    while(1)
    { 
      wifiManager.process();
      esp_task_wdt_reset();
      buttonState1 = digitalRead(right_Sensor);
      buttonState2 = digitalRead(left_Sensor);
      //check_range();
      //buttonState3 = digitalRead(ir_Sensor);  

      if (buttonState2 == HIGH)
      {
        menu = menu + 1;
        delay(500); 
        if(menu > 4)
        {menu = 1;}     
      }

      if(menu == 1 )
      {
        //display game
        Serial.println("menu game");
        game_Face();
      }
      else if (menu == 2)
      {
          //display change gmt
          Serial.println("wifi config");
          gmt_Face();
          delay(500);
      }
     
      else if (menu == 3)
      {
          //display exit?
          Serial.println("Set Time");
          setclock_Face();  
      }

       else if (menu == 4)
      {
          //display exit?
          Serial.println("exit?");
          exit_Face();
         
      }
      

      if ((menu == 1) && (buttonState1 == HIGH))
      {
        //game
        Serial.println("game entered");
        //delay(3000);
        trig_Menu = 0;
        r2d2Talking2();
        reconfig_Servo(); 
        game_Part();
        trig_Menu = 0;
        break;
        

      }

      else if((menu == 2) && (buttonState1 == HIGH))
       {
         while(1)
         {
           //esp_task_wdt_reset();
           Serial.println("Entering wifi mode.");
           wificonf_Face();
 
            wifiManager.setConfigPortalBlocking(false);
            wifiManager.setConfigPortalTimeout(180);
            //automatically connect using saved credentials if they exist
            //If connection fails it starts an access point with the specified name
            wifiManager.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);
            if(wifiManager.autoConnect("AutoConnectAP")){
            Serial.println("connected...yeey :)");
            }
            else {
            Serial.println("Configportal running");
            } 

            Serial.println(WiFi.SSID());
            Serial.println(WiFi.psk());
       
            delay(500);
            previousMillis = millis() - (3*interval);
            trig_Menu = 0;
            break;
          }
        }

      else if((menu == 3) && (buttonState1 == HIGH))
      {
        Serial.println("Set Time Entered");//gmt
         DateTime now = rtc.now();
         year = now.year();
         month = now.month();
         day = now.day();
         hour = now.hour();
         minute = now.minute();
         second = now.second();

         //rtc.adjust(DateTime(year, month, day, hour, minute, second));
         showTime();

        //preferences.begin("my-clock", false);
        //float my_gmt = preferences.getFloat("my_gmt", 0);
        //Serial.printf(");
        //gmt = my_gmt;
        //preferences.end();

        display.clearDisplay();
        display.setTextSize(2);             // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE);        // Draw white text
        display.setCursor(0,15);
        display.println("   ^^^^^");
        display.print("Set day:");
        display.display();


        oled2.clearDisplay();
          //oled2.setRotation(2);
        oled2.setTextSize(2);             // Normal 1:1 pixel scale
        oled2.setTextColor(SSD1306_WHITE);        // Draw white text
        oled2.setCursor(0,10);             // Start at top-left corner

        oled2.clearDisplay();
        oled2.print(now.year(), DEC);
        oled2.print('/');
        oled2.print(now.month(), DEC);
        oled2.print('/');
        oled2.print(now.day(), DEC);
        oled2.println();
        oled2.print(" (");
        oled2.print(daysOfTheWeek[now.dayOfTheWeek()]);
        oled2.print(") ");
        oled2.println();
        oled2.print(" -");
        if (now.hour() < 10)
        {oled2.print("0");}
        oled2.print(now.hour(), DEC);
        oled2.print(':');
        if (now.minute() < 10)
        {oled2.print("0");}
        oled2.print(now.minute(), DEC);
        oled2.print("-");
        //oled.print(':');
        //oled.print(now.second(), DEC);
        oled2.println();
        oled2.display();
        //delay(100);
        
        int time_menu = 1;
        delay(1000);
        oled2.clearDisplay();
        while(1)
        {
        oled2.clearDisplay();
        oled2.clearDisplay();
        //oled2.setRotation(2);
        oled2.setTextSize(2);             // Normal 1:1 pixel scale
        oled2.setTextColor(SSD1306_WHITE);        // Draw white text
        oled2.setCursor(0,10);             // Start at top-left corner
     
        oled2.print(year);
        oled2.print('/');
        oled2.print(month);
        oled2.print('/');
        oled2.print(day);
        oled2.println();
        oled2.print(" (");
        oled2.print(daysOfTheWeek[now.dayOfTheWeek()]);
        oled2.print(") ");
        oled2.println();
        oled2.print(" -");
        if (now.hour() < 10)
        {oled2.print("0");}
        oled2.print(hour);
        oled2.print(':');
        if (minute < 10)
        {oled2.print("0");}
        oled2.print(minute);
        oled2.print("-");
        //oled.print(':');
        //oled.print(now.second(), DEC);
        oled2.println();
        oled2.display();
        delay(100);
        
          
          esp_task_wdt_reset();
          buttonState1 = digitalRead(right_Sensor);
          buttonState2 = digitalRead(left_Sensor);
          //check_range();
          //buttonState3 = digitalRead(ir_Sensor);  

          if((buttonState2 == HIGH) && (time_menu == 1) )
            {
              day = day + 1;
              Serial.println(day);

              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Save day Now?");
              display.display();

              delay(500); 
              if(day > 31)
              {day = 1;}      
            }

          else if((buttonState2 == HIGH) && (time_menu == 2) )
            {
              month = month + 1;
              Serial.println(month);

              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Save month Now?");
              display.display();

              delay(500); 
              if(month > 12)
              {month = 1;}      
            }

            else if((buttonState2 == HIGH) && (time_menu == 3) )
            {
              year = year + 1;
              Serial.println(year);

              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Save year Now?");
              display.display();

              delay(500); 
              if(year > 2050)
              {year = 2020;}      
            }

            else if((buttonState2 == HIGH) && (time_menu == 4) )
            {
              hour = hour + 1;
              Serial.println(hour);

              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Save hour Now?");
              display.display();

              delay(500); 
              if(hour > 23)
              {hour = 0;}      
            }

            else if((buttonState2 == HIGH) && (time_menu == 5) )
            {
              minute = minute + 1;
              Serial.println(minute);

              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Save minute Now?");
              display.display();

              delay(500); 
              if(minute > 59)
              {minute = 0;}      
            }


          else if(buttonState1 == HIGH)
          {
          
            if (time_menu == 1)
            {
              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.println("Ok!");
              display.print("now month:");
              //display.print(day);
              display.display();
            }

            if (time_menu == 2)
            {
            display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.println("Ok!");
              display.print("now year:");
              //display.print(month);
              display.display();
            }

            if (time_menu == 3)
            {
            display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.println("Ok!");
              display.print("now hour:");
              //display.print(year);
              display.display();
            }

            if (time_menu == 4)
            { 
              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.println("Ok!");
              display.print("now minute:");
              //display.print(hour);
              display.display();
            }

            if (time_menu == 5)
            {
            display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Ok! All set!");
              //display.print(minute);
              display.display();
            }

            if(time_menu > 5)
            {
              rtc.adjust(DateTime(year, month, day, hour, minute, second));
              showTime();
              display.clearDisplay();
              display.setTextSize(2);             // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE);        // Draw white text
              display.setCursor(0,15);
              display.println("   ^^^^^");
              display.print("Set time success!");
              display.display();
              delay(1000);
              break;
            }

            delay(100);//menu = 3;
            //break;//and save
            time_menu = time_menu + 1;


          }
        }
        delay(500);
        previousMillis = millis() - (3*interval);
        trig_Menu = 0;
        break;
      }

      else if((menu == 4) && (buttonState1 == HIGH))
      {
        trig_Menu = 0;
        previousMillis = millis() - (3*interval);
        break;
      }



    }
  }
  
  //game_Part();
  ///////////////////////////////////////// menu ends///////////////////////////////////////////////////////////////

  }
  
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  else if (buttonState2 == HIGH) {
  t1_gap=0;
  digitalWrite(2, HIGH);
  ////////////////////////////////////
  //r2d2Whistle3();
  r2d2ShortBeep1();
  ///////////////////////////////////
  leftwink_Face();
  ////////////////////////////////reconfigure timer for sevo/////////////////////
  reconfig_Servo(); 
  ///////////////////////////////////////////////////////////////////////////////////
	for (int pos = 90; pos <= 110; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (int pos = 110; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}

  }


  else if((buttonState3 == HIGH) && (induce_state == 1))
  {
        induce_state = 0;
        buttonState3 = 0;
    r2d2FiveBeep4();
     // delay(1000);
       animate_induce(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
        previousMillis = millis() - (3*interval);
        sleepy_Face();
        induce_state = 0;

  }

  else if (buttonState1 == HIGH) {
    t1_gap=0;
    digitalWrite(2, HIGH);
  ///////////////////////////////////
  //r2d2Whistle3();
  r2d2ShortBeep1();
  ///////////////////////////////////
  rightwink_Face();
  ////////////////////////////////reconfigure timer for sevo/////////////////////
  reconfig_Servo(); 
  ///////////////////////////////////////////////////////////////////////////////////

	for (int pos = 90; pos <= 110; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (int pos = 110; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	} 

  }
  else if (buttonState3 == HIGH) {
  t1_gap=0;
  digitalWrite(2, HIGH);
  /////////////////////////////////////////////////////////////////////
  r2d2FiveBeep4();
  ///////////////////////////////////////////////////////////////////////
  Serial.println("detected");
  Serial.println(range);
  Serial.print("induce state :");
  Serial.println(induce_state);


  sus_Face();
  ////////////////////////////////reconfigure timer for sevo/////////////////////
  reconfig_Servo();
  ///////////////////////////////////////////////////////////////////////////////////
  
	for (int pos = 90; pos <= 110; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (int pos = 110; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo1.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
  
  showTime_oled();
  delay(2000);
  showTime_oled_big();
  r2d2Talking2();
  delay(2000);

  }


  else {
    
    digitalWrite(2, LOW);
    digitalWrite(16, LOW);
    normal_Face();

  }

  //////////////////////////////////////////////////timer mata tutup auto////////////////////////////////////////////////////////////
  currentMillis = millis();
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Serial.println(ESP.getFreeHeap());
  esp_task_wdt_reset();


}

void testdrawbitmap(void) {
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_angryeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_angryeye2, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

      display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_downeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_downeye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_upeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_upeye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

     display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

      display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_righteye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_righteye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

      display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_lefteye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_lefteye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_happyeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_happyeye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

      display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_suseye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_suseye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sadeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sadeye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

       display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

    display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sadeye2, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sadeye2, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

  
 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  display.display();
  oled2.display();
    display.setRotation(1);
  oled2.setRotation(1);
  delay(500);

   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  display.display();
  oled2.display();
    display.setRotation(2);
  oled2.setRotation(2);
  delay(500);

   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  display.display();
  oled2.display();
    display.setRotation(3);
  oled2.setRotation(3);
  delay(500);

  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  display.display();
  oled2.display();
    display.setRotation(0);
  oled2.setRotation(0);
  delay(500);

   
 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sharinganeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sharinganeye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

  
 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

 
 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepeye, 128, 64, 1);
  display.display();
  oled2.display();
  delay(500);

  //animate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void animate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) {
          buttonState1 = digitalRead(right_Sensor);
          buttonState2 = digitalRead(left_Sensor);
          //buttonState3 = digitalRead(ir_Sensor); 

            previous_x = x;
            previous_y = y;
            previous_z = z;
            previous_x_a = x;
            previous_y_a = y;
            previous_z_a = z;

          check_range(); 
          esp_task_wdt_reset();
          check_IMU_animate();
          Serial.println();

          if(buttonState1 == HIGH)
          {t1_gap = 0; r2d2Whistle2(); break; }
          else if (buttonState2 == HIGH)
          {t1_gap = 0; r2d2Whistle2(); break; }
          else if (buttonState3 == HIGH)
          {
            t1_gap = 0; 
            r2d2Whistle2(); 
            showTime_oled_big();
            delay(2000);
            break; 
          }
     // Loop forever...
    display.clearDisplay(); // Clear the display buffer
    oled2.clearDisplay(); 

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
      oled2.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    oled2.display();
    delay(50);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}

void animate_induce(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) {
          buttonState1 = digitalRead(right_Sensor);
          buttonState2 = digitalRead(left_Sensor);
          //buttonState3 = digitalRead(ir_Sensor); 

            previous_x = x;
            previous_y = y;
            previous_z = z;
            previous_x_a = x;
            previous_y_a = y;
            previous_z_a = z;

          check_range_induce(); 
          esp_task_wdt_reset();
          check_IMU_animate();
          Serial.println();

          if(buttonState1 == HIGH)
          {t1_gap = 0; r2d2Whistle2(); break; }
          else if (buttonState2 == HIGH)
          {t1_gap = 0; r2d2Whistle2(); break; }
          else if ((buttonState3 == HIGH)&&(induce_state == 1))
          {
            t1_gap = 0; 
            r2d2Whistle2(); 
              showTime_oled_big();
            delay(2000);
            induce_state = 0;
            break; 
          
            }
     // Loop forever...
    display.clearDisplay(); // Clear the display buffer
    oled2.clearDisplay(); 

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
      oled2.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    oled2.display();
    delay(50);        // Pause for 1/10 second
    time_display = time_display + 1;

    if(time_display > 200)
    {
      time_display = 0;
      showTime_oled_big();
      delay(2000);

    }

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}

void printLocalTime(){

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    oled2.clearDisplay();
    oled2.setRotation(2);
    oled2.setTextSize(2);             // Normal 1:1 pixel scale
    oled2.setTextColor(SSD1306_WHITE);        // Draw white text
    oled2.setCursor(0,0);             // Start at top-left corner
    oled2.println(F("Failed to obtain time"));
    oled2.display();
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
 Serial.println("Time variables");

 char timeWeekDay[10];
 strftime(timeWeekDay,10, "%A", &timeinfo);
 Serial.println(timeWeekDay);

 char timeYear[5];
 //int year;
 strftime(timeYear,5, "%Y", &timeinfo);
 year = atoi( timeYear );
 Serial.println(timeYear);
 Serial.println(year);

 char timeMonth[10];;
 //int month = 11;
 strftime(timeMonth,10, "%B", &timeinfo);
 if (timeMonth[0] == 'J' && timeMonth[1] == 'a')
  {
    month=1;
  }
  else if (timeMonth[0] == 'F' && timeMonth[1] == 'e')
  {
    month=2;
  }
  else if (timeMonth[0] == 'M' && timeMonth[1] == 'a' && timeMonth[2] == 'r')
  {
    month=3;
  }
  else if (timeMonth[0] == 'A' && timeMonth[1] == 'p')
  {
    month=4;
  }
  else if (timeMonth[0] == 'M' && timeMonth[1] == 'a' && timeMonth[2] == 'y')
  {
    month=5;
  }
  else if (timeMonth[0] == 'J' && timeMonth[1] == 'u' && timeMonth[2] == 'n')
  {
    month=6;
  }
  else if (timeMonth[0] == 'J' && timeMonth[1] == 'u' && timeMonth[2] == 'l')
  {
    month=7;
  }
  else if (timeMonth[0] == 'A' && timeMonth[1] == 'u')
  {
    month=8;
  }
  else if (timeMonth[0] == 'S' && timeMonth[1] == 'e')
  {
    month=9;
  }
  else if (timeMonth[0] == 'O' && timeMonth[1] == 'c')
  {
    month=10;
  }
  else if (timeMonth[0] == 'N' && timeMonth[1] == 'o')
  {
    month=11;
  }
  else if (timeMonth[0] == 'D' && timeMonth[1] == 'e')
  {
    month=12;
  }
  else 
  {
    month=3;
  }

 Serial.println(timeMonth);
 Serial.println(month);

 char timeDay[3];
 //int day;
 strftime(timeDay,3, "%d", &timeinfo);
 day = atoi( timeDay );
 Serial.println(timeDay);
 Serial.println(day);

 char timeHour[3];
 //int hour;
 strftime(timeHour,3, "%H", &timeinfo);
 hour = atoi( timeHour );
 Serial.println(timeHour);
 Serial.println(hour);

 char timeMinute[3];
 //int minute;
 strftime(timeMinute,3, "%M", &timeinfo);
 minute = atoi( timeMinute );
 Serial.println(timeMinute);
 Serial.println(minute);

 char timeSecond[3];
 //int second;
 strftime(timeSecond,3, "%S", &timeinfo);
 second = atoi( timeSecond );
 Serial.println(timeSecond);
 Serial.println(second);


  
}


void r2d2Sound1() {
  tone(buzzer, 1000, 100); delay(120);
  tone(buzzer, 1500, 80); delay(100);
  tone(buzzer, 1200, 90); delay(110);
  tone(buzzer, 1800, 150);
}

// Variation 2: Robotic chatter
void r2d2Sound2() {
  tone(buzzer, 1300, 60); delay(80);
  tone(buzzer, 1700, 90); delay(110);
  tone(buzzer, 800, 70); delay(90);
  tone(buzzer, 1400, 120); delay(130);
  tone(buzzer, 2000, 100);
}

// Variation 3: Excited beep sequence
void r2d2Sound3() {
  tone(buzzer, 1600, 50); delay(70);
  tone(buzzer, 900, 40); delay(60);
  tone(buzzer, 1100, 60); delay(80);
  tone(buzzer, 1400, 90); delay(100);
  tone(buzzer, 1900, 120);
}

// Variation 1: Rising Whistle
void r2d2Whistle1() {
  for (int freq = 800; freq <= 2000; freq += 20) {
    tone(buzzer, freq, 20);
    delay(10);
  }
  noTone(buzzer);
}

// Variation 2: Falling Whistle
void r2d2Whistle2() {
  for (int freq = 2000; freq >= 800; freq -= 30) {
    tone(buzzer, freq, 20);
    delay(10);
  }
  noTone(buzzer);
}

// Variation 3: Up-Down Whistle
void r2d2Whistle3() {
  for (int freq = 1000; freq <= 1800; freq += 25) {
    tone(buzzer, freq, 20);
    delay(10);
  }
  for (int freq = 1800; freq >= 1000; freq -= 25) {
    tone(buzzer, freq, 20);
    delay(10);
  }
  noTone(buzzer);
}


// Variation 1: Fast robotic chirp
void r2d2ShortBeep1() {
  tone(buzzer, 1200, 40); delay(50);
  tone(buzzer, 1600, 50); delay(60);
  tone(buzzer, 1800, 40);
}

// Variation 2: Rapid ascending beeps
void r2d2ShortBeep2() {
  tone(buzzer, 1000, 30); delay(40);
  tone(buzzer, 1300, 35); delay(45);
  tone(buzzer, 1600, 40);
}

// Variation 3: Stuttered chirp
void r2d2ShortBeep3() {
  tone(buzzer, 1400, 25); delay(30);
  tone(buzzer, 1800, 30); delay(35);
  tone(buzzer, 900, 40);
}

// Variation 4: Jumping tone
void r2d2ShortBeep4() {
  tone(buzzer, 1500, 30); delay(35);
  tone(buzzer, 1100, 25); delay(30);
  tone(buzzer, 1700, 40);
}

// Variation 5: Excited burst
void r2d2ShortBeep5() {
  tone(buzzer, 1300, 10); delay(10);
  //tone(buzzer, 900, 15); delay(10);
  //tone(buzzer, 1500, 15);
}

void r2d2LongBeep6() {
  tone(buzzer, 1000, 150); delay(160);
  tone(buzzer, 1400, 180); delay(200);
  tone(buzzer, 900, 200);
}

// Variation 7: Excited warble
void r2d2LongBeep7() {
  tone(buzzer, 1600, 120); delay(130);
  tone(buzzer, 1200, 140); delay(150);
  tone(buzzer, 1800, 180);
}

// Variation 8: Smooth frequency shift
void r2d2LongBeep8() {
  for (int freq = 1100; freq <= 1700; freq += 50) {
    tone(buzzer, freq, 40);
    delay(50);
  }
  noTone(buzzer);
}

// Variation 9: Playful chirp with echo
void r2d2LongBeep9() {
  tone(buzzer, 1300, 180); delay(190);
  tone(buzzer, 1500, 120); delay(140);
  tone(buzzer, 900, 160);
}

// Variation 10: Up-down droid chatter
void r2d2LongBeep10() {
  tone(buzzer, 1400, 100); delay(110);
  tone(buzzer, 1800, 150); delay(160);
  tone(buzzer, 1200, 180);
}

// Variation 1: Rising beep sequence
void r2d2FiveBeep1() {
  tone(buzzer, 1000, 80); delay(100);
  tone(buzzer, 1200, 90); delay(110);
  tone(buzzer, 1400, 100); delay(120);
  tone(buzzer, 1600, 110); delay(130);
  tone(buzzer, 1800, 120);
}

// Variation 2: Falling beep sequence
void r2d2FiveBeep2() {
  tone(buzzer, 2000, 90); delay(100);
  tone(buzzer, 1700, 100); delay(110);
  tone(buzzer, 1400, 110); delay(120);
  tone(buzzer, 1100, 120); delay(130);
  tone(buzzer, 900, 130);
}

// Variation 3: Playful up and down
void r2d2FiveBeep3() {
  tone(buzzer, 1300, 80); delay(90);
  tone(buzzer, 1600, 90); delay(100);
  tone(buzzer, 1000, 100); delay(110);
  tone(buzzer, 1400, 110); delay(120);
  tone(buzzer, 1800, 120);
}

// Variation 4: Quick, stuttered beeps
void r2d2FiveBeep4() {
  tone(buzzer, 1200, 50); delay(60);
  tone(buzzer, 1400, 60); delay(70);
  tone(buzzer, 1600, 70); delay(80);
  tone(buzzer, 1100, 80); delay(90);
  tone(buzzer, 1300, 90);
}

// Variation 5: High-energy robot chatter
void r2d2FiveBeep5() {
  tone(buzzer, 1500, 70); delay(80);
  tone(buzzer, 1800, 80); delay(90);
  tone(buzzer, 1300, 90); delay(100);
  tone(buzzer, 1700, 100); delay(110);
  tone(buzzer, 900, 110);
}

void r2d2Talking1() {
  tone(buzzer, 1000, 80); delay(100);
  tone(buzzer, 1200, 90); delay(110);
  tone(buzzer, 1500, 70); delay(90);
  tone(buzzer, 1300, 80); delay(100);
  tone(buzzer, 1800, 100); delay(120);
  tone(buzzer, 1100, 90); delay(110);
  tone(buzzer, 1700, 110); delay(130);
  tone(buzzer, 900, 80); delay(100);
  tone(buzzer, 1600, 100); delay(120);
  tone(buzzer, 1400, 90);
}

// Variation 2: More expressive robotic speech
void r2d2Talking2() {
  tone(buzzer, 1400, 100); delay(110);
  tone(buzzer, 1800, 80); delay(90);
  tone(buzzer, 1300, 90); delay(100);
  tone(buzzer, 1000, 70); delay(80);
  tone(buzzer, 1700, 110); delay(120);
  tone(buzzer, 1200, 100); delay(110);
  tone(buzzer, 1600, 90); delay(100);
  tone(buzzer, 900, 80); delay(90);
  tone(buzzer, 1500, 110); delay(120);
  tone(buzzer, 1100, 90);
}

void r2d2Angry() {
  tone(buzzer, 2000, 100); delay(110);
  tone(buzzer, 1800, 80); delay(90);
  tone(buzzer, 2200, 90); delay(100);
  tone(buzzer, 2500, 70); delay(80);
  tone(buzzer, 1700, 60); delay(70);
  tone(buzzer, 2300, 100); delay(110);
  tone(buzzer, 1900, 80); delay(90);
  tone(buzzer, 2700, 90); delay(100);
  tone(buzzer, 2100, 70); delay(80);
  tone(buzzer, 2600, 120); delay(130);
}

void angry_Face()
{
   display.clearDisplay();
    oled2.clearDisplay();
    display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_angryeye, 128, 64, 1);
    oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_angryeye2, 128, 64, 1);
    display.display();
    oled2.display();
   
}

void reconfig_Servo()
{
   delay(10); 
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
   myservo1.setPeriodHertz(50);// Standard 50hz servo
   myservo1.attach(13, 500, 2400); 
  delay(10); 
}

void happy_Face()
{
   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_happyeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_happyeye, 128, 64, 1);
  display.display();
  oled2.display();

}

void normal_Face()
{
   display.clearDisplay();
    oled2.clearDisplay();
    display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
    oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_normaleye, 128, 64, 1);
    display.display();
    oled2.display();
   
}

void leftwink_Face()
{
display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_happyeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye, 128, 64, 1);
  display.display();
  oled2.display();
}

void rightwink_Face()
{
display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_happyeye, 128, 64, 1);
  display.display();
  oled2.display();
}

void sus_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_suseye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_suseye, 128, 64, 1);
  display.display();
  oled2.display();
}

void left_Face()
{
    display.clearDisplay();
    oled2.clearDisplay();
    display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_lefteye, 128, 64, 1);
    oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_lefteye, 128, 64, 1);
    display.display();
    oled2.display();
}

void right_Face()
{
     display.clearDisplay();
    oled2.clearDisplay();
    display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_righteye, 128, 64, 1);
    oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_righteye, 128, 64, 1);
    display.display();
    oled2.display();
}

void confuse_Face()
{
   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_confuseeye, 128, 64, 1);
  display.display();
  oled2.display();
}

void sharingan_Face()
{

 display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sharinganeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sharinganeye, 128, 64, 1);
  display.display();
  oled2.display();
}

void close_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye, 128, 64, 1);
  display.display();
  oled2.display();
}

void wificonf_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_wificonf, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_wificonf, 128, 64, 1);
  display.display();
  oled2.display();
}

void Gamemode_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_Gamemode, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_Gamemode, 128, 64, 1);
  display.display();
  oled2.display();
}

void rightans_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_rightans, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_rightans, 128, 64, 1);
  display.display();
  oled2.display();
}

void wrongans_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_wrongans, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_wrongans, 128, 64, 1);
  display.display();
  oled2.display();
}

void target_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_target, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_angryeye2, 128, 64, 1);
  display.display();
  oled2.display();
}



void sleepy_Face()
{
  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepy3, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepy3, 128, 64, 1);
  display.display();
  oled2.display();

  delay(1000);

r2d2ShortBeep2();

  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();

  delay(100);

  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepy3, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepy3, 128, 64, 1);
  display.display();
  oled2.display();

  delay(1000);

r2d2ShortBeep2();

  display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye1, 128, 64, 1);
  display.display();
  oled2.display();

  delay(100);

   display.clearDisplay();
  oled2.clearDisplay();
  display.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_sleepy3, 128, 64, 1);
  oled2.drawBitmap(
    (display.width()  - 128 ) / 2,
    (display.height() - 64) / 2,
    epd_bitmap_closeeye, 128, 64, 1);
  display.display();
  oled2.display();

  delay(1000);

}

void display_Add()
{

                display.print(a);
                display.print("+");
                display.print(b);
                display.println("=");
                display.display();
                //delay(100);

        
                oled2.print(a);
                oled2.print("+");
                oled2.print(b);
                oled2.println("=");
                oled2.display();
                delay(1000);
}

void display_Sub()
{
                display.print(a);
                display.print("-");
                display.print(b);
                display.println("=");
                display.display();
                //delay(100);

        
                oled2.print(a);
                oled2.print("-");
                oled2.print(b);
                oled2.println("=");
                oled2.display();
                delay(1000);
}

void display_Mul()
{
                display.print(a);
                display.print("*");
                display.print(b);
                display.println("=");
                display.display();
                //delay(100);

        
                oled2.print(a);
                oled2.print("*");
                oled2.print(b);
                oled2.println("=");
                oled2.display();
                delay(1000);
}

void display_Div()
{
                display.print(al);
                display.print("/");
                display.print(bl);
                display.println("=");
                display.display();
                //delay(100);

        
                oled2.print(al);
                oled2.print("/");
                oled2.print(bl);
                oled2.println("=");
                oled2.display();
                delay(1000);
}

void menu_Face()
{ 
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,15);

  display.println("   ^^^^^");
  display.print("this button confirms");
  //display.println("=");
  display.display();
                //delay(100);
  oled2.clearDisplay();
  oled2.setTextSize(1);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(0,15);
  
  oled2.println("   ^^^^^");
  oled2.print("this button toggles");
  //oled2.println("=");
  oled2.display();
}

void game_Face()
{ 
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,15);

  display.println("   ^^^^^");
  display.print("   Enter?");
  //display.println("=");
  display.display();
                //delay(100);
  oled2.clearDisplay();
  oled2.setTextSize(2);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(0,15);
  
  oled2.println("   ^^^^^");
  oled2.print(" Game Mode");
  //oled2.println("=");
  oled2.display();
}

void gmt_Face()
{ 
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,15);

  display.println("   ^^^^^");
  display.print("   Enter?");
  //display.println("=");
  display.display();
                //delay(100);
  oled2.clearDisplay();
  oled2.setTextSize(2);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(0,15);
  
  oled2.println("   ^^^^^");
  oled2.print(" Wifi Config:");
  //oled2.println("=");
  oled2.display();
}

void exit_Face()
{ 
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,15);

  display.println("   ^^^^^");
  display.print("   Sure?");
  //display.println("=");
  display.display();
                //delay(100);
  oled2.clearDisplay();
  oled2.setTextSize(2);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(0,15);
  
  oled2.println("   ^^^^^");
  oled2.print("Exit menu?");
  //oled2.println("=");
  oled2.display();
}


void setclock_Face()
{ 
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,15);

  display.println("   ^^^^^");
  display.print("   Enter?");
  //display.println("=");
  display.display();
                //delay(100);
  oled2.clearDisplay();
  oled2.setTextSize(2);             // Normal 1:1 pixel scale
  oled2.setTextColor(SSD1306_WHITE);        // Draw white text
  oled2.setCursor(0,15);
  
  oled2.println("   ^^^^^");
  oled2.print("Set Clock");
  //oled2.println("=");
  oled2.display();
}

void game_Part()
{
 /////////////////////////////////////////////////if game starts////////////////////////////////////////////////////////////////
      Gamemode_Face();
      //trig_Game = 0;
      reconfig_Servo(); 
      delay(2000);

         
          previousMillis2 = millis() - 3000;

          int level = 1;
          int type = 1;
          right_Answer = 0;
          currentMillis = millis();

          oled2.clearDisplay();
          //oled2.setRotation(2);
          oled2.setTextSize(2);             // Normal 1:1 pixel scale
          oled2.setTextColor(SSD1306_WHITE);        // Draw white text
          oled2.setCursor(0,30);             // Start at top-left corner

          display.clearDisplay();
          //display.setRotation(2);
          display.setTextSize(2);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE);        // Draw white text
          display.setCursor(0,30);  

          oled2.clearDisplay(); oled2.setCursor(0,30); oled2.print("-Addition-"); oled2.display();
      ////////////////////////////question types starts////////////////////////////////////////////////////
           while(1)
          {   
              
              watch_Millis2 = currentMillis - previousMillis2;

              ///////////////////////random eye blinking//////////////////////////////////
              if (watch_Millis2 >= 2000)//interval 
                {
                 ///////////////////////t1_gap/////////////////////////////////////
                  t5_gap = t5_gap + 1;
                  Serial.println(t5_gap);
                  
                  display.clearDisplay();
                  display.setCursor(0,10); 
                  display.print((7-t5_gap));
                  display.setCursor(0,30); 
                  display.print("level:");
                  display.print(level);
                  display.display();
                  ///////////////////////////////////////////////////////////
                  previousMillis2 = previousMillis2 + 2000;
                }

              buttonState1 = digitalRead(right_Sensor);
              buttonState2 = digitalRead(left_Sensor);
              //buttonState3 = digitalRead(ir_Sensor);  
              esp_task_wdt_reset();

             if(buttonState1 == HIGH)//
              { 
                level = level + 1;
                if (level > 5)
                { level = 1;}
                Serial.print("level:");
                Serial.println(level);
                delay(500);

                display.clearDisplay();
                display.setCursor(0,30); 
                display.print("level:");
                display.print(level);
                display.display();
                //break; 
              }
              if(buttonState2 == HIGH)//
              { 
                type = type + 1;
                if (type > 3)
                { type = 1; }

                if (type == 1)
                 { Serial.println("Addition"); oled2.clearDisplay(); oled2.setCursor(0,30); oled2.print("-Addition-"); oled2.display();}
                else if (type == 2)
                 {Serial.println("Subtraction"); oled2.clearDisplay(); oled2.setCursor(0,30); oled2.print("-Subtract-"); oled2.display();}
                else if (type == 3)
                 {Serial.println("Multiplication"); oled2.clearDisplay();oled2.setCursor(0,30); oled2.print("-Multiply-"); oled2.display();}
                else if (type == 4)
                 {Serial.println("Division"); oled2.clearDisplay(); oled2.setCursor(0,30); oled2.print("-Division-"); oled2.display();}
                else if (type == 5)
                 {Serial.println("All");}
                delay(500);

                //break; 
              }
              currentMillis = millis();
              if (t5_gap == 7)
               {t5_gap = 0; break; }
                 
          } 
          //////////////////////////////////question types end///////////////////////////////////////////////

      for (int bi = 0; bi <= 5; bi++) 
      {
          esp_task_wdt_reset();
          //////////////////////////////set letter size///////////////////////////////////////////
          oled2.clearDisplay();
          //oled2.setRotation(2);
          oled2.setTextSize(2);             // Normal 1:1 pixel scale
          oled2.setTextColor(SSD1306_WHITE);        // Draw white text
          oled2.setCursor(30,15);             // Start at top-left corner

          display.clearDisplay();
          //display.setRotation(2);
          display.setTextSize(2);             // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE);        // Draw white text
          display.setCursor(30,15);  
          //////////////////////////////////////////////////////////////////////////////
  
          reconfig_Servo();
          myservo1.write(90);

          //////////////////////////////////level and type determination//////////////////////////////////
          if (type == 1)
            { 
              //display.println("Addition");
              if (level == 1)
               {
                a = random(1,20);
                b = random(1,20);
                c = a + b;
                e = random(1,20);
               }
               else if (level == 2)
               {
                a = random(11,40);
                b = random(11,40);
                c = a + b;
                e = random(1,20);
               }
               else if (level == 3)
               {
                a = random(11,60);
                b = random(21,60);
                c = a + b;
                e = random(1,20);
               }
               else if (level == 4)
               {
                a = random(11,80);
                b = random(31,80);
                c = a + b;
                e = random(1,20);
               }
               else if (level == 5)
               {
                a = random(11,100);
                b = random(41,100);
                c = a + b;
                e = random(1,20);
               }

               display_Add();

              }
          else if (type == 2)
            {
              //display.println("Subtraction");
              if (level == 1)
               {
                a = random(10,20);
                b = random(1,10);
                c = a - b;
                e = random(1,20);
               }
               else if (level == 2)
               {
                a = random(20,40);
                b = random(11,20);
                c = a - b;
                e = random(1,20);
               }
               else if (level == 3)
               {
                a = random(40,60);
                b = random(21,40);
                c = a - b;
                e = random(1,20);
               }
               else if (level == 4)
               {
                a = random(50,80);
                b = random(31,50);
                c = a - b;
                e = random(1,20);
               }
               else if (level == 5)
               {
                a = random(70,100);
                b = random(41,70);
                c = a - b;
                e = random(1,20);
               }

               display_Sub();


              }
          else if (type == 3)
            {
              //display.println("Multiplication");
              if (level == 1)
               {
                a = random(0,12);
                b = random(2,3);
                c = a * b;
                e = random(1,20);
               }
               else if (level == 2)
               {
                a = random(0,12);
                b = random(2,5);
                c = a * b;
                e = random(1,20);
               }
               else if (level == 3)
               {
                a = random(0,12);
                b = random(3,7);
                c = a * b;
                e = random(1,20);
               }
               else if (level == 4)
               {
                a = random(0,12);
                b = random(5,9);
                c = a * b;
                e = random(1,20);
               }
               else if (level == 5)
               {
                a = random(0,12);
                b = random(8,12);
                c = a * b;
                e = random(1,20);
               }

               display_Mul();

              }
          
          
          ///////////////////////////////////////////////////////////////////////////////////////////////
        

           sel = random(0,3);
            if(sel > 0)
            {
              if (type == 4)
              {
                display.println(cl);
                display.display();
                //delay(100);

                oled2.println((cl+e));
                oled2.display();
              }
              else
              {
                display.println(c);
                display.display();
                //delay(100);

                oled2.println((c+e));
                oled2.display();
              }
              //delay(3000);

              ans = 1;
            }

            else
            {
              if (type == 4)
              {
                display.println((cl+e));
                display.display();
                //delay(100);

                oled2.println(cl);
                oled2.display();
              }
              else
              {
                display.println((c+e));
                display.display();
                //delay(100);

                oled2.println(c);
                oled2.display();
              }
              //delay(3000);
              ans = 2;              
            }


          previousMillis2 = millis() - 3000;
          currentMillis = millis();

          while(1)
          {
              buttonState1 = digitalRead(right_Sensor);
              buttonState2 = digitalRead(left_Sensor);
              //buttonState3 = digitalRead(ir_Sensor);  
              esp_task_wdt_reset();

              watch_Millis2 = currentMillis - previousMillis2;

              ///////////////////////random eye blinking//////////////////////////////////
              if (watch_Millis2 >= 2000)//interval 
                {
                 ///////////////////////t1_gap/////////////////////////////////////
                  t5_gap = t5_gap + 1;
                  Serial.println(t5_gap);
                  
                  ///////////////////////////////////////////////////////////
                  previousMillis2 = previousMillis2 + 2000;
                }

             if((buttonState1 == HIGH) && (ans == 1))///////////////////////hhheeereee
              { 
                r2d2Whistle2(); 
                rightans_Face();
                delay(2000);
                right_Answer = right_Answer + 1;
                t5_gap = 0;
                break; 
              }
              else if((buttonState1 == HIGH) && (ans == 2))///////////////////////hhheeereee
              { 
                r2d2Whistle2(); 
                wrongans_Face();
                delay(2000);
                t5_gap = 0;
                break; 
              }
              else if ((buttonState2 == HIGH) && (ans == 2))
              {
                r2d2Whistle2(); 
                rightans_Face();
                delay(2000);
                right_Answer = right_Answer + 1;
                t5_gap = 0;
                break; 
              }
               else if ((buttonState2 == HIGH) && (ans == 1))
              {
                r2d2Whistle2(); 
                wrongans_Face();
                delay(2000);
                t5_gap = 0;
                break; 
              }
              currentMillis = millis();
              if (t5_gap > 5)
               {t5_gap = 0; r2d2Whistle2(); wrongans_Face(); delay(2000); break; }
            
          } 

         
         
      }

       r2d2Whistle3(); 
        happy_Face();
        delay(2000);
        oled2.clearDisplay();
        oled2.setTextSize(2);             // Normal 1:1 pixel scale
        oled2.setTextColor(SSD1306_WHITE);        // Draw white text
        oled2.setCursor(30,15); 
        oled2.print("score: ");
        oled2.setCursor(30,30); 
        oled2.print(right_Answer);
        oled2.print("/");
        oled2.print("6");
        oled2.display();
        delay(1000);
        r2d2Whistle3();

        previousMillis = millis() - (3*interval);

    
}
