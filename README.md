Washer-Motor-Control-Arduino
========================

Arduino library to control Whirlpool Duet washing machine motor (3 phase 820W, P/N 8182793) through Motor Control Unit (MCU P/N 8181693).

An UART full-duplex to half-duplex adapter is required to connect Arduino hardware serial port to Washer MCU [UART FULL DUPLEX TO HALF DUPLEX CONVERTER FOR MOTOR CONTROL UNIT 8181693](http://www.code2control.com/category/garage-fun/)

Arduino hardware serial to half-duplex adapter wiring:
=======
```bash
Arduino pin  ----   half-duplex adapter pin
   0 (RX)    ----       3 (RX)
   1 (TX)    ----       2 (TX)
   GND       ----       4 (GND)
   5V        ----       1 (VCC)
```

API calls example:
=======

```bash

    // initialize serial steam. could be hardware serial or software searial.
    // here shows using hardware serial
    Serial.begin(WPMCONTROL_SERIAL_BAUD, WPMCONTROL_SERIAL_CFG);

    // create an instance of the WPMControl
    WPMControl wpmcontrol;
    
    // tell WPMControl to use initialized serial    
    if (wpmcontrol.connect(Serial))
    {

        // start motor with provided speed, acceleration rate and direction (DIR_CW or DIR_CCW)
        // speed range from 500 - 12000 RPM
        // acceleration rate from 64 - 3200. acceleration number higher the sooner the motor reaches target speed.
        // direction could be DIR_CW (clockwise) or DIR_CCW (counter-clockwise)
        motorRunning = wpmcontrol.setSpeed(speed, acceleration, WPMControl::DIR_CW);
        
    } 

    // to stop motor. deceleration number higher the sooner the motor will stop. valid from 64 - 3200.
    wpmcontrol.stopMotor(deceleration); 
```


##**DISCLAIMER - USE AT YOUR OWN RISK**

**SAFTY FIRST!** The motor outputs more than 1 horse power, running speed up to 10k+ RPM! 

I assumes no responsibility for any loss or hardship (be it loss of life, property, equipment, bodily injury, money, time or other) incurred directly or indirectly by using this API, the demo app or any other contents in this repository.