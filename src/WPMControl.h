/*
    WPMControl.h - - Library for Arduino boards to control Whirlpool Duet washing machine 3 phase AC motor. For all Arduino boards.
  
    Auther: wfang2002@gmail.com
 
    https://github.com/wfang2002/Washer-Motor-Control-Arduino
 
*/

#ifndef _WPMCONTROL_H_
#define _WPMCONTROL_H_

#define WPMCONTROL_SERIAL_BAUD 1200
#define WPMCONTROL_SERIAL_CFG SERIAL_7E1

class WPMPhy;
class Stream;
class String;

class WPMControl
{
public:

    enum Direction
    {
        DIR_CW = 1, // Clockwise
        DIR_CCW = 2 // Counter-Clockwise
    };

    enum MotorState
    {
        MS_Idle,
        MS_Breaking,
        MS_Accelerating,
        MS_AtTarget,
        MS_Unknown
    };

    WPMControl();

    WPMControl(Stream &stream, bool hasEcho = true);

    int version();

    bool connect(Stream &stream, bool hasEcho = true);

    void disconnect();

    void doTask();

    MotorState getState();

    // returns current motor speed (RPM)
    int getSpeed();    

    /// <summary>
    /// Start motor to given RPM
    /// </summary>
    /// <param name="speed">300 - 12000 RPM</param>
    /// <param name="acceleration">64 - 3200</param>
    /// <param name="direction">CCW or CW</param>
    bool setSpeed(int speed, int acceleration, Direction direction);

    /// <summary>
    /// Stop motor
    /// </summary>
    /// <param name="acceleration">64 - 3200</param>
    /// <returns></returns>
    bool stopMotor(int acceleration);

    void setDbgStream(Stream &stream);

private:

    Stream *_dbg;
    WPMPhy *_phyLayer;
    bool _initialized;
    bool _busy;

    int _speed;
    MotorState _state;

    void parseResponse(String response);

    bool executeCommand(String cmd);
    bool executeCommand(const char *cmd);

    bool initialize();

    void onCmdTimer();

};

#endif //_WPMCONTROL_H_