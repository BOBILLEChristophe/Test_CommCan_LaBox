/*

  LaBoxCmd.h

  Christophe Bobille - Locoduino

  The ESP32 requires to be connected to a CAN transceiver

*/

#ifndef __LA_BOX_CMD__
#define __LA_BOX_CMD__

#include "Arduino.h"
#include "CanMsg.h"

struct Loco
{
  uint32_t address;
  uint16_t speed;
  uint8_t direction;
  byte fn[28];
};

class LaBoxCmd
{
private:
    uint32_t mID;
    bool mDebug;
    bool mPower;
public:
    LaBoxCmd(uint8_t);
    ~LaBoxCmd();
    void setup();
    bool setPower(bool);
    bool setThrottle(Loco *);
    bool toggleThrottleDir(Loco *);
    bool setFunction(Loco *, byte);
    bool toggleFunction(Loco *, byte);
    bool emergency();
};

#endif
