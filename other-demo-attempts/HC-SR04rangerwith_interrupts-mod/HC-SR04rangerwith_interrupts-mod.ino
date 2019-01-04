/*
Arduino Uno HC-SR04 ultrasonic range finder program using interrupts 
Author:  Steve Garratt 
Date:    19/10/13
 
Interrupts from timer 1 are used to schedule and deliver the sensor
trigger pulse. The same interrupt is used to control the flash rate
of the onboard LED indicating distance.

The duration of the sensor echo signal is measured by generating an
external interrupt ervery time the echo signal changes state.

This sketch uses the TimerOne library downloadable from here:
http://code.google.com/p/arduino-timerone/downloads/detail?name=TimerOne-v9.zip&can=2&q=

Install the library using the following guide:
http://arduino.cc/en/Guide/Libraries
*/

#include <TimerOne.h>                                 // Header file for TimerOne library

//#define onBoardLED 13                                 // Pin 13 onboard LED
#define sensorA_TrigPin 12                                    // Pin 12 trigger output
#define sensorA_Echo_Pin 10                                  // Interrupt id for echo pulse

#define sensorB_TrigPin 4                                    // Pin 12 trigger output
#define sensorB_Echo_Pin 3                                  // Interrupt id for echo pulse

#define TIMER_US 50                                   // 50 uS timer duration 
#define TICK_COUNTS 4000                              // 200 mS worth of timer ticks

volatile long sensorA_Echo_Start = 0;                         // Records start of echo pulse 
volatile long sensorA_Echo_End = 0;                           // Records end of echo pulse
volatile long sensorA_Echo_Duration = 9999999;                      // Duration - difference between end and start. Set high in case of disconnection
volatile long sensorB_Echo_Start = 0;                         // Records start of echo pulse 
volatile long sensorB_Echo_End = 0;                           // Records end of echo pulse
volatile long sensorB_Echo_Duration = 9999999;                      // Duration - difference between end and start. Set high in case of disconnection
volatile long echo_duration = 0;                      // Duration - difference between end and start

volatile int trigger_time_count = 0;                  // Count down counter to trigger pulse time
volatile long range_flasher_counter = 0;              // Count down counter for flashing distance LED
volatile int sensor = sensorA_TrigPin;         //Set sensor  var to sensor PIN for first run through

// ----------------------------------
// setup() routine called first.
// A one time routine executed at power up or reset time.
// Used to initialise hardware.
// ----------------------------------
void setup() 
{
  pinMode(sensorA_TrigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(sensorA_Echo_Pin, INPUT);                            // Echo pin set to input
  pinMode(sensorB_TrigPin, OUTPUT);                           // Trigger pin set to output
  pinMode(sensorB_Echo_Pin, INPUT);                            // Echo pin set to input

//  pinMode(onBoardLED, OUTPUT);                        // Onboard LED pin set to output

  trigger_time_count = TICK_COUNTS;                  //So first countdown works - TH
  Timer1.initialize(TIMER_US);                        // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );                 // Attach interrupt to the timer service routine 
  attachInterrupt(digitalPinToInterrupt(sensorA_Echo_Pin), sensorA_echo_interrupt, CHANGE);  // Attach interrupt to the sensor echo input
                               
  attachInterrupt(digitalPinToInterrupt(sensorB_Echo_Pin), sensorB_echo_interrupt, CHANGE);  // Attach interrupt to the sensor echo input
   Serial.begin (9600);  // Initialise the serial monitor output
}

// ----------------------------------
// loop() Runs continuously in a loop.
// This is the background routine where most of the processing usualy takes place.
// Non time critical tasks should be run from here.
// ----------------------------------
void loop()
{
    Serial.println(echo_duration / 58);               // Print the distance in centimeters
    delay(100);                                       // every 100 mS
}

// --------------------------
// timerIsr() 50uS second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{
    trigger_pulse();                                 // Schedule the trigger pulses
    distance_flasher();                              // Flash the onboard LED distance indicator
}

// --------------------------
// trigger_pulse() called every 50 uS to schedule trigger pulses.
// Generates a pulse one timer tick long.
// Minimum trigger pulse width for the HC-SR04 is 10 us. This system
// delivers a 50 uS pulse.
// TH mods to alternate between pulsing sensor A and sensor B base don a sensorswitch static

// --------------------------
void trigger_pulse()
{
      static volatile int state = 0;                 // State machine variable
      
      if (!(--trigger_time_count))                   // Count to 200mS
      {                                              // Time out - Initiate trigger pulse
         trigger_time_count = TICK_COUNTS;           // Reload
         state = 1;                                  // Changing to state 1 initiates a pulse
         //Serial.println(sensor);
      }  
    
      switch(state)                                  // State machine handles delivery of trigger pulse
      {
        case 0:                                      // Normal state does nothing
            break;
        case 1:                                      // Initiate pulse
           sensor=(sensor==sensorA_TrigPin ? sensorB_TrigPin:sensorA_TrigPin);  //toggle the sensor PIN
           digitalWrite(sensor, HIGH);              // Set the trigger output high
           state = 2;                                // and set state to 2
           break;
        case 2:                                      // Complete the pulse
        default:         
           digitalWrite(sensor, LOW);               // Set the trigger output low
           state = 0;                                // and return state to normal 0  
           break;
     }
}

// --------------------------
// sensorA_echo_interrupt External interrupt from HC-SR04 echo signal. 
// Called every time the echo signal changes state for sensor A.
//
// Note: this routine does not handle the case where the timer
//       counter overflows which will result in the occassional error.
// --------------------------
void sensorA_echo_interrupt()
{
  Serial.println("Sensor A echo");
  switch (digitalRead(sensorA_Echo_Pin))                     // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      sensorA_Echo_End = 0;                                 // Clear the end time
      sensorA_Echo_Start = micros();                        // Save the start time
      break;
      
    case LOW:                                       // Low so must be the end of hte echo pulse
      sensorA_Echo_End = micros();                          // Save the end time
      sensorA_Echo_Duration = sensorA_Echo_End -  sensorA_Echo_Start;        // Calculate the pulse duration
      break;
  }  
}

// --------------------------
// sensorB_echo_interrupt External interrupt from HC-SR04 echo signal. 
// Called every time the echo signal changes state for sensor B.
//
// Note: this routine does not handle the case where the timer
//       counter overflows which will result in the occassional error.
// --------------------------
void sensorB_echo_interrupt()
{
  switch (digitalRead(sensorB_Echo_Pin))                     // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      sensorB_Echo_End = 0;                                 // Clear the end time
      sensorB_Echo_Start = micros();                        // Save the start time
      break;
      
    case LOW:                                       // Low so must be the end of hte echo pulse
      sensorB_Echo_End = micros();                          // Save the end time
      sensorB_Echo_Duration = sensorB_Echo_End -  sensorB_Echo_Start;        // Calculate the pulse duration
      break;
  }
}



// --------------------------
// distance_flasher() Called from the timer 1 timerIsr service routine.
// Flashes the onboard LED at a rate inversely proportional
// to distance. The closer it gets the higher the frequency.
// --------------------------
void distance_flasher()
{
 Serial.println(sensorA_Echo_Duration);
 Serial.println(sensorB_Echo_Duration);
   //Serial.println(sensorA_Echo_Duration);
  echo_duration= (sensorA_Echo_Duration <= sensorB_Echo_Duration ? sensorA_Echo_Duration:sensorB_Echo_Duration);

      if (--range_flasher_counter <= 0)                // Decrement and test the flash timer
      {                                                // Flash timer time out
         if (echo_duration < 25000)                    // If the echo duration is within limits
         {
           range_flasher_counter = echo_duration * 2;  // Reload the timer with the current echo duration
         }
         else
         {
           range_flasher_counter = 25000;              // If out of range use a default
         }
         
        // digitalWrite( onBoardLED, digitalRead( onBoardLED ) ^ 1 );   // Toggle the onboard LED
      }
}