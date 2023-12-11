#ifndef MELEEHITRESULT_H
#define MELEEHITRESULT_H

#include <string>

struct MeleeAttackTable {
    float miss;
    float dodge;
    float parry;
    float glancingBlow;
    float block;
    float criticalHit;
    float crushingBlow;
    float ordinaryHit;
    float glancingBlowDamageReduction;
};

enum class MeleeHitResult {
    Miss,
    Dodge,
    Parry,
    GlancingBlow,
    Block,
    CriticalHit,
    CrushingBlow,
    OrdinaryHit
};

std::string getMeleeHitResultName(MeleeHitResult mhr);

#endif // MELEEHITRESULT_H
