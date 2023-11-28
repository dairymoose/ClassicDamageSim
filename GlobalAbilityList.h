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
    
    bool didPrintStats = false;
    bool didCharge = false;
    
    void resetFields();
    
    GlobalAbilityList();
};

#endif // GLOBALABILITYLIST_H
