#ifndef APPLIEDBUFF_H
#define APPLIEDBUFF_H

#include "Buff.h"

class AppliedBuff
{
    Buff *buff = nullptr;
    float appliedTimestamp = 0.0f;
    float capturedDuration = 0.0f;
    int32_t lastTickedCount = 0;
    float lastTickedTimestamp = 0.0f;
    int32_t tickCount = 0;
public:
    AppliedBuff();
    AppliedBuff(Buff *buff);
    Buff *getBuff() const;
    void setBuff(Buff *value);
    float getAppliedTimestamp() const;
    void setAppliedTimestamp(float value);
    float getCapturedDuration() const;
    void setCapturedDuration(float value);
    int32_t getLastTickedCount() const;
    void setLastTickedCount(const int32_t &value);
    float getLastTickedTimestamp() const;
    void setLastTickedTimestamp(float value);
    int32_t getTickCount() const;
    void setTickCount(const int32_t &value);
    bool isBuffExpired(float timestamp);
    bool isTickTimerExpired(Combatant *Cbt, float timestamp);
    void triggerTickTimer(float timestamp);
};

#endif // APPLIEDBUFF_H
