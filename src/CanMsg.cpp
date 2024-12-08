

#include "CanMsg.h"
#include "LaBoxCmd.h"

LaBoxCmd::LaBoxCmd(uint8_t id) :

                                               mID(id),
                                               mDebug(false),
                                               mPower(false)
{
}

LaBoxCmd::~LaBoxCmd()
{
    if (mDebug)
        Serial.println(F("### Destroying controller"));
}

void LaBoxCmd::setup()
{
}

bool LaBoxCmd::setPower(bool power)
{
    CanMsg::sendMsg(0x00, );
    mPower = power;
    return power;
}

bool LaBoxCmd::setSpeed(Loco *loco)
{
    CanMsg::sendMsg(0x04, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, loco->speed);
    return true;
}

bool LaBoxCmd::setDirection(Loco *loco)
{
    CanMsg::sendMsg(0x05, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, loco->direction);
    return true;
}

bool LaBoxCmd::toggleThrottleDir(Loco *loco)
{
    loco->direction = !loco->direction;
    CanMsg::sendMsg(0x05, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, loco->direction);
    return true;
}

bool LaBoxCmd::setFunction(Loco *loco, byte idx)
{
    CanMsg::sendMsg(0x06, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, idx, loco->fn[idx]);
    return true;
}

bool LaBoxCmd::toggleFunction(Loco *loco, byte idx)
{
    loco->fn[idx] = !loco->fn[idx];
    CanMsg::sendMsg(0x06, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, idx, loco->fn[idx]);
    return true;
}

bool LaBoxCmd::emergency()
{
    CanMsg::sendMsg(0x00);
    return true;
}
