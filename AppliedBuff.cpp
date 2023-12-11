#include "AppliedBuff.h"
#include "DamageSimulation.h"

Buff *AppliedBuff::getBuff() const
{
    return buff;
}

void AppliedBuff::setBuff(Buff *value)
{
    buff = value;
}

float AppliedBuff::getAppliedTimestamp() const
{
    return appliedTimestamp;
}

void AppliedBuff::setAppliedTimestamp(float value)
{
    appliedTimestamp = value;
}

float AppliedBuff::getCapturedDuration() const
{
    return capturedDuration;
}

void AppliedBuff::setCapturedDuration(float value)
{
    capturedDuration = value;
}

int32_t AppliedBuff::getLastTickedCount() const
{
    return lastTickedCount;
}

void AppliedBuff::setLastTickedCount(const int32_t &value)
{
    lastTickedCount = value;
}

float AppliedBuff::getLastTickedTimestamp() const
{
    return lastTickedTimestamp;
}

void AppliedBuff::setLastTickedTimestamp(float value)
{
    lastTickedTimestamp = value;
}

int32_t AppliedBuff::getTickCount() const
{
    return tickCount;
}

void AppliedBuff::setTickCount(const int32_t &value)
{
    tickCount = value;
}

bool AppliedBuff::isBuffExpired(float timestamp)
{
    float timeDiff = timestamp - this->appliedTimestamp;
    //if (DamageSimulation::almostEqual(std::min(timeDiff, this->capturedDuration), this->capturedDuration)) {)
    timeDiff += FLOATING_POINT_SMALL_NUMBER;
    if (timeDiff >= this->capturedDuration) {
        return true;
    }
    return false;
}

bool AppliedBuff::isTickTimerExpired(Combatant *Cbt, float timestamp)
{
    float timeDiff = timestamp - this->lastTickedTimestamp;
    timeDiff += FLOATING_POINT_SMALL_NUMBER; //account for FP in-accuracy
    float tickPeriodSec = this->getBuff()->getOnCalculateDotTickPeriod()(Cbt);
    return timeDiff >= tickPeriodSec;
}

void AppliedBuff::triggerTickTimer(float timestamp)
{
    ++tickCount;
    this->lastTickedTimestamp = timestamp;
}

Combatant *AppliedBuff::getAppliedBy() const
{
    return appliedBy;
}

void AppliedBuff::setAppliedBy(Combatant *value)
{
    appliedBy = value;
}

AppliedBuff::AppliedBuff()
{
    
}

AppliedBuff::AppliedBuff(Buff *buff)
{
    this->buff = buff;
}
