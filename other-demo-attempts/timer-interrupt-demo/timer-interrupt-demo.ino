/*

Timer interrupt demo using LEDs
Steve Garratt
8th October 2013
http://homediyelectronics.com

Requires the TimerOne library downloadable from
http://code.google.com/p/arduino-timerone/downloads/detail?name=TimerOne-v9.zip&can=2&q=
Note: Download and rename to remove the hyphen before installing.

*/

#include <TimerOne.h>                           // Header file for TimerOne library
 
#define LED0 3                                  // Pin 3
#define LED1 4                                  // Pin 4
#define TIMER_US 100000                         // 100mS set timer duration in microseconds 
#define TICK_COUNTS 20                          // 2S worth of timer ticks


volatile long tick_count = TICK_COUNTS;         // Counter for 2S
volatile bool in_long_isr = false;              // True if in long interrupt
 
// ----------------------------------
//  setup() routine called first.
// ----------------------------------
void setup() 
{
  pinMode(LED0, OUTPUT);                        // Set the pins as outputs
  pinMode(LED1, OUTPUT);    
  
  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );           // attach the ISR routine here
}
 
// ----------------------------------
//  loop() Runs continuously in a loop.
// ----------------------------------
void loop()
{
}
 
// --------------------------
// timerIsr() 100 milli second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{
    digitalWrite( LED0, digitalRead( LED0 ) ^ 1 );   // Toggle LED 0
    
    if (!(--tick_count))                             // Count to 2S
    {
      tick_count = TICK_COUNTS;                      // Reload
      tick_2s_isr();                                 // Call the 2S routine
    }
}


// --------------------------
// tick_2s_isr() 2 second routine
// Called every time the count gets to 2S
// --------------------------
void tick_2s_isr()
{
    if (in_long_isr)                                // Prevent reentering this code
    {
      return;
    }
    
    in_long_isr = true;                             // Indicates we are in the long isr
    
    volatile long i;

    interrupts();                                   // Enable interrupts
    digitalWrite( LED1, digitalRead( LED1 ) ^ 1 );  // Toggle LED 1
    
    for (i = 0; i < 400000; i++)                   // Delay because delay() will not work here
    {
    }
   
   noInterrupts();                                 // Disable interrupts
    in_long_isr = false;                            // Indicates exit of the long isr
}

