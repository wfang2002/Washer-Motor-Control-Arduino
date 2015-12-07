/*
    WPMPhy.h - - Physical layer driver for WPMControl
  
    Auther: wfang2002@gmail.com
 
    https://github.com/wfang2002/Washer-Motor-Control-Arduino
 
*/

#ifndef _WPMPHY_H_
#define _WPMPHY_H_

#include <Arduino.h>

class Stream;
class String;

// Physic Layer (UART)
class WPMPhy
{
    enum ControlChar
    {
        STX = 0x02,
        ETX = 0x03
    };

public:
    // if use hardware duplex to half-duplex adapter, hasEcho = true
    // if use software half-duplex, set hasEcho to false.
    WPMPhy(Stream &stream, bool hasEcho = true);

    // Sync call - Send a command to controller and wait for response
    String sendReceive(String cmd);

    //Send command to controller, not wait for response
    bool send(String cmd);

    String receive();

private:
    Stream &_serial;
    bool _hasEcho;
};


#endif