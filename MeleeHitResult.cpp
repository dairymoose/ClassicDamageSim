#include "MeleeHitResult.h"

std::string getMeleeHitResultName(MeleeHitResult mhr) {
    switch (mhr) {
    case MeleeHitResult::Miss:
        return "Miss";
    case MeleeHitResult::Dodge:
        return "Dodge";
    case MeleeHitResult::Parry:
        return "Parry";
    case MeleeHitResult::GlancingBlow:
        return "Glancing Blow";
    case MeleeHitResult::Block:
        return "Block";
    case MeleeHitResult::CriticalHit:
        return "Critical Hit";
    case MeleeHitResult::CrushingBlow:
        return "Crushing Blow";
    case MeleeHitResult::OrdinaryHit:
        return "Ordinary Hit";
    }
}
