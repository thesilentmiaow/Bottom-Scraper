
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

#define BROWN   0x4D9F
#define RANGES 3  //We have 3 colours for the graph based on sensor reading

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
int yPosLowerGraph = tft.height() - 2;   //Position the cursor at bottom 
int yPosUpperGraph = (tft.height() / 2) - 2 ;  //Position cursor half way up

int zone[RANGES] = {0,0,0};  //The different values of graph for the colours Green, Amber Red

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");
  Serial.println(tft.width());
  Serial.println(tft.height());

  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);

  // blank the screen
  tft.fillScreen(ST7735_BLACK);

  tft.fillScreen(ST7735_BLACK);
  tft.drawLine(0, 79, tft.width(), 79, ST7735_YELLOW);
  // a single pixel
//  tft.drawPixel(0, 0, ST7735_GREEN);
//  tft.drawPixel(127, 0, ST7735_RED);
//  tft.drawPixel(0, 159, ST7735_BLUE);

  tft.fillRect(0, 0 , 10, 160, ST7735_WHITE);
  tft.drawRect(0, 0 , 128, 160, ST7735_WHITE);
  tft.fillRect(118, 0 , 10, 160, BROWN);
  
  for (int i = 0; i <= (RANGES -1); i++) {
    zone[i] = (tft.width()-20) / RANGES * i;
   // Serial.println(zone[i]);   
  }
}

void loop() {
  int sensor = analogRead(A0);
  int graphWidth = map(sensor,0,1023,0,tft.width()-20);
  int colour = ST7735_GREEN;
  
  //print out the height to the serial monitor
   //Serial.println(graphWidth);
   //Serial.println(sensor);
   //Serial.println(zone[1]);
   //Serial.println(zone[2]);
  if (graphWidth > zone[2]) {
   colour = ST7735_RED;
  }  else if (graphWidth > zone[1]) {
   colour = ST7735_YELLOW;
  }

  // draw a line in a nice color
  tft.drawFastHLine(118-graphWidth, yPosLowerGraph, graphWidth, colour );
  tft.drawFastHLine(118-graphWidth, yPosUpperGraph, graphWidth, colour );

  // if the graph has reached the screen edge
  // erase the screen and start again
  if (yPosUpperGraph <=  1) {
    yPosLowerGraph = tft.height() - 2;   //Position the cursor at bottom 
    yPosUpperGraph = (tft.height() / 2) - 2;  //Position cursor half way up
    tft.fillRect(10,1,108,78, ST7735_BLACK);
    tft.fillRect(10,80,108,78, ST7735_BLACK);
  } else {
    // increment the horizontal position:
    Serial.println(yPosUpperGraph);
    Serial.println(yPosLowerGraph);
    yPosLowerGraph--;
    yPosUpperGraph--;
  }

  delay(250);

}