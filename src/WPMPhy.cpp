
/*
    WPMPhy.cpp - - Physical layer driver for WPMControl
  
    Auther: wfang2002@gmail.com
 
    https://github.com/wfang2002/Washer-Motor-Control-Arduino
 
*/

#include "WPMPhy.h"

#include "Stream.h"

#define READBYTE_TIMEOUT    200 // ms
#define RECEIVE_TIMEOUT     2000 //ms


static byte calculateLRC(String msg)
{
    byte LRC = 0x00;
    for (int i = 0; i < msg.length(); i++)
    {
        LRC = (byte)(LRC ^ (byte)msg[i]);
    }
    return LRC;
}

WPMPhy::WPMPhy(Stream &stream, bool hasEcho)
:_serial(stream)
{
    _hasEcho = hasEcho;

    // serial port config: 1200 baud, 7 bit, parity Even, stop bit one  
    _serial.setTimeout(READBYTE_TIMEOUT);
}

// Sync call - Send a command to controller and wait for response
String WPMPhy::sendReceive(String cmd)
{

    send(cmd);

    // we're receiving echoed sending string here
    if (_hasEcho)
        receive();

    // actuall response from motor controller
    return receive();

}

// Sends command to controller, not wait for response
bool WPMPhy::send(String cmd)
{
    //cleanup buffer before writing
    while (_serial.available() > 0)
    {
        _serial.read();
    }

    byte lrcCal = calculateLRC(cmd + (char)WPMPhy::ETX);

    String out = (char)WPMPhy::STX + cmd + (char)WPMPhy::ETX + (char)lrcCal;

    _serial.write(out.c_str());

    return true;
}

// Returns response message string
String WPMPhy::receive()
{
    unsigned long ts = millis();

    String message = "";

    // Wait till we get STX or timeout
    while (_serial.read() != (byte)WPMPhy::STX && millis() - ts < RECEIVE_TIMEOUT);

    // timeout ?
    if (millis() - ts >= RECEIVE_TIMEOUT) 
    {
        return message;
    }

    byte recv = 0;
    do
    {
        if (_serial.available() > 0)
        {
           recv = (byte)_serial.read(); 
           message += (char)recv;
        }
        
    } while (recv != (byte)WPMPhy::ETX);

    byte lrcRecv = 0;
    _serial.readBytes(&lrcRecv, 1);
    byte lrcCal = calculateLRC(message);

    if (lrcCal != lrcRecv)
    {
        //Wrong LRC
        return "";
    }

    message.remove(message.length() - 1);
    return message;
}
