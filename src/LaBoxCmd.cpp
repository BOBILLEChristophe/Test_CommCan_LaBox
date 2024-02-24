

#include "CanMsg.h"
#include "LaBoxCmd.h"

LaBoxCmd::LaBoxCmd(uint8_t id, uint8_t idLabox) :

                                               mID(id),
                                               mIDlabox(idLabox),
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
    CanMsg::sendMsg(2, 0xFE, mID, mIDlabox, 0, power);
    mPower = power;
    return power;
}

bool LaBoxCmd::setThrottle(Loco *loco)
{
    CanMsg::sendMsg(2, 0xF0, mID, mIDlabox, 0, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, loco->speed, loco->direction);
    return true;
}

bool LaBoxCmd::toggleThrottleDir(Loco *loco)
{
    loco->direction = !loco->direction;
    CanMsg::sendMsg(2, 0xF0, mID, mIDlabox, 0, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, loco->speed, loco->direction);
    return true;
}

bool LaBoxCmd::setFunction(Loco *loco, byte idx)
{
    CanMsg::sendMsg(2, 0xF1, mID, mIDlabox, 0, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, idx, loco->fn[idx]);
    return true;
}

bool LaBoxCmd::toggleFunction(Loco *loco, byte idx)
{
    loco->fn[idx] = !loco->fn[idx];
    CanMsg::sendMsg(2, 0xF1, mID, mIDlabox, 0, (loco->address & 0xFF00) >> 8, loco->address & 0x00FF, idx, loco->fn[idx]);
    return true;
}

bool LaBoxCmd::emergency()
{
    CanMsg::sendMsg(0, 0xFF, mID, mIDlabox, 0);
    return true;
}
