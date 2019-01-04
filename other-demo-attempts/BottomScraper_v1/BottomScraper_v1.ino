//www.elegoo.com
//2016.12.9

/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include "SR04.h"
#include "pitches.h"
 

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

#define TRIG_PIN 2
#define ECHO_PIN 4
#define TRIG2_PIN 6
#define ECHO2_PIN 5

SR04 sr04a = SR04(ECHO_PIN,TRIG_PIN);
SR04 sr04b = SR04(ECHO2_PIN,TRIG2_PIN);

long a;
long b;

int melody[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int duration = 500;  // 500 miliseconds

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Bottom Scaper !");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Left =      cm "); 
  lcd.setCursor(0, 1);
  lcd.print("Right =     cm"); 
}

void loop() {
  lcd.setCursor(7, 0);
  lcd.print("     "); 
  lcd.setCursor(8, 1);
  lcd.print("     "); 

//Measure left sensor
   a=sr04a.Distance();
   b=sr04b.Distance();
  // set the cursor to column 0, line 0
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(7, 0);
  lcd.print(a);
  lcd.setCursor(8,1); 
  lcd.print(b);


  
  if  (a < 30.0 && a > 20.0) { 
     tone(13, NOTE_C5, duration);
     delay(750);
  } else if (a <= 20.0 && a > 10.0) {
     tone(13, NOTE_C5, duration);
     delay(500);
  } else if (a <= 10.0 && a > 5.0) {
     tone(13, NOTE_C5, duration);
     delay(250);
  } else if (a <= 5.0) {
     tone(13, NOTE_C5, duration);
     delay(100);
  }
  delay(250);
}
