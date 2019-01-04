#include <Adafruit_GFX.h>   

#include <Adafruit_ST7735.h>

#include <SPI.h>

//TFT to Arduino Uno/Nano Pins:

//GND = GND

//VCC = 3.3v

//A0 (DC) = 8

//RESET = 9

//CS = 10

//SDA = 11

//SCL = 13

#define TFT_CS     10

#define TFT_RST    9 

#define TFT_DC     8

#define TFT_SCLK 13  

#define TFT_MOSI 11  


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

 

void setup(void) {

  tft.initR(INITR_BLACKTAB);  //Initializes ST7735.

  //Please note that use of 'BLACKTAB' displays red instead of blue and vice versa.

 

  tft.fillScreen(ST7735_BLACK);

 

  tft.setCursor(5, 10);

  tft.setTextColor(ST7735_RED); 

  tft.setTextSize(1);

  tft.println("Welcome to");

  tft.setCursor(5, 20);

  tft.setTextColor(ST7735_GREEN);

  tft.setTextSize(2);

  tft.println("Karen's");

  tft.setCursor(5, 38);

  tft.setTextColor(ST7735_BLUE);

  tft.setTextSize(3);

  tft.print("e-Shop");

 

  tft.drawLine(0, 140, 127, 140, ST7735_WHITE);

  tft.drawCircle(40, 140, 5, ST7735_CYAN);

  tft.fillTriangle(64, 135, 59, 145, 69, 145, ST7735_MAGENTA);

  tft.drawTriangle(64, 135, 59, 145, 69, 145, ST7735_WHITE);

  tft.fillCircle(88, 140, 5, ST7735_YELLOW);

}

 

void loop()

{

 

}