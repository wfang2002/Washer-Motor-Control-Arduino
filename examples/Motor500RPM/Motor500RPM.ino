/*
WPMControl example
This example for the WPMControl shows how to use the WPMControl
library to control the speed of a Whirlpool Duet washing machine
motor (3 phase 820W, P/N 8182793) through Motor Control Unit 
(MCU P/N 8181693).

An half-duplex adapter (http://www.code2control.com/2014/01/uart-full-duplex-to-half-duplex-converter-for-motor-control-unit-8181693/) 
is needed for the example. 

Connect Arduino to half-duplex adapter:
Arduino pin  ----   half-duplex adapter pin
   0 (RX)    ----       3 (RX)
   1 (TX)    ----       2 (TX)
   GND       ----       4 (GND)
   5V        ----       1 (VCC)

This example code is part of the public domain
https://github.com/wfang2002/Washer-Motor-Control-Arduino

*/

#include <SoftwareSerial.h>
#include <WPMControl.h>

// for debug printout
SoftwareSerial debugStream(2, 3); // RX, TX

// create an instance of the WPMControl class
WPMControl wpmcontrol;

bool motorRunning = false;

int speed = 500; // motor target speed (RPM), valid range: 300 - 1200
int acceleration = 1000; // speed acceleration rate, valid range: 64 - 3200

unsigned long prtInterval = millis();
unsigned long runtime;

void setup()
{
    // use software serial for debug output
    debugStream.begin(19200);
  
    // use hardware serial for motor control
    Serial.begin(WPMCONTROL_SERIAL_BAUD, WPMCONTROL_SERIAL_CFG);
    while (!Serial) {
        // wait for serial port to connect. Needed for native USB port only
    }

    debugStream.println("start WPM");
    
    // tell WPMControl to use hardware serial port    
    if (wpmcontrol.connect(Serial))
    {
        debugStream.println("WPMContol Connected");

        // start motor with provided speed, acceleration rate and direction (DIR_CW or DIR_CCW)
        motorRunning = wpmcontrol.setSpeed(speed, acceleration, WPMControl::DIR_CW);
        
        runtime = millis();
        debugStream.println("Motor shall start running");
    }

}

void loop()
{
    
    
    if (motorRunning)
    {
        // must call DoTask in loop to allow driver process commands and response
         wpmcontrol.doTask();

         if ((millis() - prtInterval) > 2000)
         {
            prtInterval = millis();
            debugStream.print(millis());
            debugStream.print(" - Motor speed (RPM): ");

            // show motor running speed 
            debugStream.println(wpmcontrol.getSpeed());
         }

         // let motor run for 60 seconds the stop
         if (millis() - runtime > 60000) {
            wpmcontrol.stopMotor(acceleration);
            wpmcontrol.disconnect();
            motorRunning = false;
            debugStream.println("Motor shall stop running");
             
         }   
    }

}