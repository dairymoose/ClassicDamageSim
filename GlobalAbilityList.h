#ifndef GLOBALABILITYLIST_H
#define GLOBALABILITYLIST_H

#include "Ability.h"

class GlobalAbilityList
{
public:
    Ability *PrintDps = nullptr;
    Ability *PrintPlayerStats = nullptr;
    Ability *MeleeMainhandAutoAttack = nullptr;
    Ability *MeleeOffhandAutoAttack = nullptr;
    Ability *RangedAutoAttack = nullptr;
    Ability *Rend = nullptr;
    Ability *BattleShout = nullptr;
    Ability *Whirlwind = nullptr;
    Ability *MortalStrike = nullptr;
    Ability *Slam = nullptr;
    Ability *Execute = nullptr;
    Ability *Charge = nullptr;
    Ability *Bloodrage = nullptr;
    Ability *HeroicStrike = nullptr;
    Ability *QuickStrike = nullptr;
    Ability *RagingBlow = nullptr;
    Ability *FreeWildStrikes = nullptr;
    Ability *Overpower = nullptr;
    Buff *Enrage = nullptr;
    Buff *WildStrikes = nullptr;
    Buff *DeepWounds = nullptr;
    bool inExtraAttack = false;
    
    bool didPrintStats = false;
    bool didCharge = false;
    
    void resetFields();
    
    GlobalAbilityList();
    
    static GlobalAbilityList* activeList;
};

#endif // GLOBALABILITYLIST_H
