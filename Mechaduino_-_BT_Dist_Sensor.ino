
/*
  -------------------------------------------------------------
  Mechaduino 0.1 & 0.2 Firmware  v0.1.4
  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver

  All Mechaduino related materials are released under the
  Creative Commons Attribution Share-Alike 4.0 License
  https://creativecommons.org/licenses/by-sa/4.0/

  Many thanks to all contributors!
  --------------------------------------------------------------

  Controlled via a SerialUSB terminal at 115200 baud.

  Implemented serial commands are:

  s  -  step
  d  -  dir
  p  -  print [step number] , [encoder reading]

  c  -  calibration routine
  e  -  check encoder diagnositics
  q  -  parameter query

  x  -  position mode
  v  -  velocity mode
  t  -  torque mode

  y  -  enable control loop
  n  -  disable control loop
  r  -  enter new setpoint

  j  -  step response
  k  -  edit controller gains -- note, these edits are stored in volatile memory and will be reset if power is cycled
  g  -  generate sine commutation table
  m  -  print main menu


  ...see serialCheck() in Utils for more details

*/


#include "Utils.h"
#include "Parameters.h"
#include "State.h"
#include "analogFastWrite.h"

#include <Adafruit_VL6180X.h>
Adafruit_VL6180X dSense = Adafruit_VL6180X();
float dist = 0;
uint8_t dStatus;

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup()        // This code runs once at startup
{

  digitalWrite(ledPin, HIGH);       // turn LED on
  setupPins();                      // configure pins
  setupTCInterrupts();              // configure controller interrupt

  SerialUSB.begin(115200);
  Serial1.begin(9600);
  dSense.begin();

  delay(3000);                      // This delay seems to make it easier to establish a connection when the Mechaduino is configured to start in closed loop mode.
  serialMenu();                     // Prints menu to serial monitor
  setupSPI();                       // Sets up SPI for communicating with encoder
  digitalWrite(ledPin, LOW);        // turn LED off


  // Uncomment the below lines as needed for your application.
  // Leave commented for initial calibration and tuning.

  //    configureStepDir();           // Configures setpoint to be controlled by step/dir interface
  //    configureEnablePin();         // Active low, for use wath RAMPS 1.4 or similar
  enableTCInterrupts();         // uncomment this line to start in closed loop
  mode = 'x';                   // start in position mode

}

void checkDist() {

  dist = dSense.readRange();
  dStatus = dSense.readRangeStatus();
  SerialUSB.println(dist);
  if (dist < 200 && dist > 20) {
    r=map(dist,20,200,0,30*360);
  }
  //check_status();
}

bool check_status() {
  if  ((dStatus >= VL6180X_ERROR_SYSERR_1) && (dStatus <= VL6180X_ERROR_SYSERR_5)) {
    SerialUSB.println("System error");
    return false;
  }
  else if (dStatus == VL6180X_ERROR_ECEFAIL) {
    SerialUSB.println("ECE failure");
  }
  else if (dStatus == VL6180X_ERROR_NOCONVERGE) {
    SerialUSB.println("No convergence");
  }
  else if (dStatus == VL6180X_ERROR_RANGEIGNORE) {
    SerialUSB.println("Ignoring range");
  }
  else if (dStatus == VL6180X_ERROR_SNR) {
    SerialUSB.println("Signal/Noise error");
  }
  else if (dStatus == VL6180X_ERROR_RAWUFLOW) {
    SerialUSB.println("Raw reading underflow");
  }
  else if (dStatus == VL6180X_ERROR_RAWOFLOW) {
    SerialUSB.println("Raw reading overflow");
  }
  else if (dStatus == VL6180X_ERROR_RANGEUFLOW) {
    SerialUSB.println("Range reading underflow");
  }
  else if (dStatus == VL6180X_ERROR_RANGEOFLOW) {
    SerialUSB.println("Range reading overflow");
  }
  return true;
}

//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////


void loop()                 // main loop
{

  serialCheck();              //must have this execute in loop for serial commands to function

if(true) {
  checkDist();
}
  //r=0.1125*step_count;      //Don't use this anymore. Step interrupts enabled above by "configureStepDir()", adjust step size in parameters.cpp

}



