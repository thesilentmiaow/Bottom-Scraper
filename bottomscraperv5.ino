// V4 was based on having two sensors at front
// But not enough space at front of van for two sensors
// So v5 rewrite to have one sensor at front and one at black

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#include "pitches.h"
#include <TimerOne.h>

// notes in the melody:
int beepduration = 50;  // 50 miliseconds
int range_beep_counter = 0;

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8
#define PIN_3     3

#define BROWN   0x4D9F
#define RANGES 3  //We have 3 colours for the graph based on sensor reading

// TFT screen is 128x160 pixels. Positions are x,y coord from top left corner with screen in portrait format
// So Width refers to the shorter 128 pixel dimension. Height refers to the longer 160 pixel dimension

// For this project screen is positioned on landscape format. 
// So we use width when plotting the verticals of the graph

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
int yPosFrontGraph; 
int yPosBackGraph; 

int zone[RANGES] = {0,0,0};  //The different values of graph for the colours Green, Amber Red

#include "SR04.h"
#define TRIG_PINa 5
#define ECHO_PINa 6
#define TRIG_PINb 4
#define ECHO_PINb 2
#define RIDEHEIGHT 30   //Distance from ground to front sensor 
#define SCRAPEHEIGHT 17  //Calculated front sensor range that will scrape at rear
#define BACKHEIGHT 27  //Distance from ground to rear sensor
#define FRONTCAL 2  //+ OR - Calibration adjustment of range sensor
int mode = 0;   //zero = ONE sensor. Non zero means TWO sensors

SR04 sr04Front = SR04(ECHO_PINa,TRIG_PINa);
SR04 sr04Back = SR04(ECHO_PINb,TRIG_PINb);
long frontRange = RIDEHEIGHT +10;  //sensor a range. INitialised to a high value in case sensor disconnected
long backRange = RIDEHEIGHT +10;  //sensor b range. INitialised to a high value in case sensor disconnected;
long nearest = RIDEHEIGHT +10;;
int nearestSensor = 1;  //Which sensor is recording nearest range

#define TIMER_US 50       // 50 uS timer duration 

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! Bottom scraper alarm here");

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");
  Serial.println(tft.width());
  Serial.println(tft.height());

  // blank the screen
  tft.fillScreen(ST7735_BLACK);

  if (mode) {
    tft.drawLine(0, 80, tft.width(), 80, ST7735_YELLOW);  //Separator b/w 2 displays
    yPosFrontGraph = (tft.height() / 2 - 1);   //Position cursor half way up for front sensor
    yPosBackGraph = 158 ;

  } else {
    yPosFrontGraph = 158;
    yPosBackGraph = 0;
  }

  tft.fillRect(0, 0 , 10, 160, ST7735_WHITE);  //Base of vehicle
  tft.drawRect(0, 0 , 128, 160, ST7735_WHITE); //Frame
  tft.fillRect(118, 0 , 10, 160, BROWN);  //The ground

  //Print text on screen
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setTextWrap(false);
  if (mode) {
    tft.setCursor(80, 7);
    tft.print("Front");
    tft.setCursor(80, 85);
    tft.print("Rear");
  }

  for (int i = 0; i <= (RANGES -1); i++) {
    zone[i] = ((tft.width()-20) / RANGES) * i;
  }
  Timer1.initialize(TIMER_US);                        // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );                 // Attach interrupt to the timer service routine 
}

void loop() {
 //Sensor a at front of vehicle. Sensor b and back of vehicle
  frontRange=(sr04Front.Distance()> 0 ? sr04Front.Distance(): RIDEHEIGHT);  //Get a distance as long as non-zero
  frontRange = frontRange + FRONTCAL;
  backRange=(sr04Back.Distance()> 0 ? sr04Back.Distance(): RIDEHEIGHT); //Get a distance as long as non-zero
                                                          //no sensor attached returns 0 distance
  nearest = (frontRange<(backRange+(RIDEHEIGHT-BACKHEIGHT)) ? frontRange:backRange);   //What is nearest measurement
  nearestSensor = (frontRange<(backRange + (RIDEHEIGHT-BACKHEIGHT)) ? 1:2);   //Which is nearer sensor Front = 1 Back = 2. 
                                                                //Back sensor sits 3cm below level of front sensor
                                                                //So apply an offset - 
  Serial.print(nearest);
  Serial.println("cm");
  if (mode) {
    plotGraph(frontRange, 1);   //Pass range and sensor number.  Front = 1
    plotGraph(backRange, 2);            //Back sensor = 2
  } else {  //1 sensor only so must be front
    plotGraph(frontRange, 1);
  }

    // if the graph has reached the screen edge
    // erase the screen and start again
  if (yPosFrontGraph <= 2 ) {   //off the edge of the screen
    if (mode) {  //2 sensors
        yPosFrontGraph = (tft.height() / 2) - 1 ;   //Position the cursor half way up for front sensor
        yPosBackGraph = 158;  //Position cursor at bottom for back sensor
        tft.fillRect(10,1,108,78, ST7735_BLACK);
        tft.fillRect(10,80,108,78, ST7735_BLACK);
        tft.fillRect(0, 0 , 10, 160, ST7735_WHITE);//Base of vehicle
        tft.fillRect(118, 0 , 10, 160, BROWN);    //The ground
        tft.drawLine(0, 80, tft.width(), 80, ST7735_YELLOW);  //Separator b/w 2 displays
    } else {   //only 1 sensor
        yPosFrontGraph = 158;   //Position the cursor at bottom for one sensor
        tft.fillRect(10,1,108,158, ST7735_BLACK);
        tft.fillRect(0, 0 , 10, 160, ST7735_WHITE); //Base of vehicle
        tft.fillRect(118, 0 , 10, 160,BROWN );   //The ground
    }
    
    if (mode) {
      tft.setCursor(80, 7);
      tft.print("Front");
      tft.setCursor(80, 85);
      tft.print("Rear");
    }
  } else {
    if (mode) {
      yPosFrontGraph--;
      yPosBackGraph--;
    } else {
      yPosFrontGraph--;
    }
  }
  delay(50);
}

void plotGraph(long dist, int sensor)  {
  int graphWidth = 0;
  if ( sensor == 1) {  //Front sensor
    graphWidth = constrain(map(dist,SCRAPEHEIGHT,RIDEHEIGHT,0,tft.width()-20),0, tft.width()-20 );  //Map sensor range to screen width constrained
  } else {  //Back sensor
    graphWidth = constrain(map(dist,0,BACKHEIGHT,0,tft.width()-20),0, tft.width()-20 );  //Map sensor range to screen width constrained
  }
  int colour = ST7735_RED;

  if (graphWidth > zone[2]) {
   colour = ST7735_GREEN;
  }  else if (graphWidth > zone[1]) {
   colour = ST7735_YELLOW;
  }
  
  if (graphWidth > 0) {
    if (mode) {  //so two sensors
      if (sensor==2) {  //Sensor 2 is backsensor 
        tft.drawFastHLine(10, yPosBackGraph, graphWidth, colour );
      } else {  //sensor 1 is front sensor
        tft.drawFastHLine(10, yPosFrontGraph, graphWidth, colour );
      }
    } else {  //one sensor
      tft.drawFastHLine(10, yPosFrontGraph, graphWidth, colour );
    }
  } 
}

void distanceBeeper()
{
  if (--range_beep_counter <= 0)  {              // Decrement and test the flash timer                                                // Flash timer time out
    if (nearestSensor == 1)  {                    // If the echo duration is within limits
      // If the range is within 2/3 of normal ride height which should also trigger yellow graph
      if (nearest < (SCRAPEHEIGHT + 0.66666*(RIDEHEIGHT-SCRAPEHEIGHT)))   
      {
        if (nearest < SCRAPEHEIGHT) {
          range_beep_counter = nearest * 50 ;  // Reload the timer with the current echo duration
        } else  {
          range_beep_counter = nearest * 200 ;  // Reload the timer with the current echo duration
        }
        tone(PIN_3, NOTE_C5, beepduration);
      }
      else
      {
        range_beep_counter = RIDEHEIGHT * 200;              // If out of range use a default
      }
    } else {   //Back sensor is nearest
      if (nearest < (BACKHEIGHT +3) ) {
        if (nearest < 5) {
          range_beep_counter = nearest * 50 ;  // Reload the timer with the current echo duration
        } else  {
          range_beep_counter = nearest * 200 ;  // Reload the timer with the current echo duration
        }
        tone(PIN_3, NOTE_C5, beepduration);
      } else {
        range_beep_counter = RIDEHEIGHT * 200;              // If out of range use a default
      }

    }  
  }
}

// --------------------------
// timerIsr() 50uS second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{
    distanceBeeper();
}
