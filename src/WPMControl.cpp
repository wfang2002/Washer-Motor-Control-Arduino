/*
    WPMControl.cpp - - Library for Arduino boards to control Whirlpool Duet washing machine 3 phase AC motor. For all Arduino boards.
  
    Auther: wfang2002@gmail.com
 
    https://github.com/wfang2002/Washer-Motor-Control-Arduino
 
*/

#include <Arduino.h>
#include <WString.h>

#include "WPMPhy.h"
#include "WPMControl.h"

#define VERSION 1
#define POLL_INTERVAL   500

WPMControl::WPMControl()
{
    _dbg = 0;
    _phyLayer = 0;
    _initialized = false;
    _busy = false;
    _speed = 0;
    _state = WPMControl::MS_Unknown;
}

WPMControl::WPMControl(Stream &stream, bool hasEcho)
{

    _dbg = 0;
    _initialized = false;
    _busy = false;
    _speed = 0;
    _state = WPMControl::MS_Unknown;

    connect(stream, hasEcho);
}

// connect to a stream
bool WPMControl::connect(Stream &stream, bool hasEcho)
{
   
    _phyLayer = new WPMPhy(stream, hasEcho);

    initialize();

    return _initialized;
}

void WPMControl::onCmdTimer()
{
    if (_busy) return;

    if (_initialized)
    {
        // query motor status if line is not busy
        executeCommand("pmc");
        executeCommand("pmi");
    }
}

void WPMControl::parseResponse(String response)
{
    if (response.length() < 4)
    {
        return;
    }

    char cmd = response[2];
    switch (cmd)
    {
        case 'c':   // current state
        {
            //c - status. 0x00: idle, 0x1a:acc/de-acc to target(L), 0x0a:at target speed(L), 0x18: breaking(L), 
            //0x15:acc/de-acc t0 target(R), 0x05:at target speed(R), 0x14:breaking(R)
            String stateHex = response.substring(5, 7);
            unsigned int stateVal = strtoul(stateHex.c_str(), 0, 16);
            if (stateVal == 0) _state = WPMControl::MS_Idle;
            else if (stateVal == 0x1A || stateVal == 0x15) _state = WPMControl::MS_Accelerating;
            else if (stateVal == 0x0A || stateVal == 0x05) _state = WPMControl::MS_AtTarget;
            else if (stateVal == 0x18 || stateVal == 0x14) _state = WPMControl::MS_Breaking;
            else _state = WPMControl::MS_Unknown;
        }
            break;
        case 'i':   // speed
        {
            String speedHex = response.substring(4, 8);
            _speed = (int)(strtoul(speedHex.c_str(), 0, 16) / 1.2);
        }
            break;
    }
}

bool WPMControl::executeCommand(const char *cmd)
{
    return executeCommand(String(cmd));
}

bool WPMControl::executeCommand(String cmd)
{
    if (_busy) return false;

    _busy = true;

    if (_dbg) {
        _dbg->print(millis());
        _dbg->print(" WPMControl: Sending command ");    
        _dbg->println(cmd);    
    }

    String resp = _phyLayer->sendReceive(cmd);

    // receives no response
    if (resp == "")
    {
        if (_dbg)
            _dbg->println("WPMControl: No response!");
        _busy = false;
        return false;
    }

    parseResponse(resp);

    _busy = false;

    return true;
}

int WPMControl::version()
{
    return VERSION;
}

void WPMControl::disconnect()
{
    _initialized = false;
}

void WPMControl::doTask()
{
    static unsigned long ts = millis();
    if (millis() - ts > POLL_INTERVAL)
    {
        ts = millis();
        onCmdTimer();
    }
}


WPMControl::MotorState WPMControl::getState()
{
    return _state;
}

int WPMControl::getSpeed()
{
    return _speed;
}

bool WPMControl::initialize()
{
    // Send some magic strings to MCU ...
    String cmd = "pmp020887";
    executeCommand(cmd);

    cmd = "pmp08122d";
    executeCommand(cmd);

    cmd = "pmp106e50";
    executeCommand(cmd);

    cmd = "pmp0a5f5a";
    executeCommand(cmd);

    cmd = "pmp04c828";
    _initialized = executeCommand(cmd);

    return _initialized;
}

/// <summary>
/// Start motor to given RPM
/// </summary>
/// <param name="speed">300 - 12000 RPM</param>
/// <param name="acceleration">64 - 3200</param>
/// <param name="direction">CCW or CW</param>
bool WPMControl::setSpeed(int speed, int acceleration, Direction direction)
{
    char buffer[10];

    if (!_initialized)
    {
        return false;
    }

    if (acceleration < 64) acceleration = 64;
    if (acceleration > 3200) acceleration = 3200;

    // command format: "pms{direction}{speed}{acceleration}"
    String cmd = "pms";
    cmd += WPMControl::DIR_CW ? "r" : "l";

    sprintf(buffer, "%04x", (int)(speed*1.2));
    cmd += buffer;

    sprintf(buffer, "%03x", acceleration);
    cmd += buffer;

    return executeCommand(cmd);
}

/// <summary>
/// Stop motor
/// </summary>
/// <param name="deceleration">64 - 3200</param>
/// <returns></returns>
bool WPMControl::stopMotor(int deceleration)
{
    char buffer[10];

    if (!_initialized)
    {
        return false;
    }

    if (deceleration < 64) deceleration = 64;
    if (deceleration > 3200) deceleration = 3200;

    String cmd = "pmsb0000";

    sprintf(buffer, "%03x", deceleration);
    cmd += buffer;

    return executeCommand(cmd);
}

void WPMControl::setDbgStream(Stream &stream)
{
    _dbg = &stream;
}
