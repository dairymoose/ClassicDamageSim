#include "GlobalAbilityList.h"

#include "PlayerCharacter.h"
#include "Buff.h"
#include "DamageSimulation.h"

GlobalAbilityList::GlobalAbilityList()
{    
    this->PrintDps = new Ability("Print DPS");
    this->PrintDps->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 5.0f;});
    this->PrintDps->setOnCooldownTriggeredFunction([](float timestamp, PlayerCharacter *PC, int32_t rank){ if (PC->getDamageDone() > 0) COMBAT_LOG(timestamp, PC, "pDmg="<<PC->getDamageDone()<<", pDPS="<<PC->getDamageDone()/timestamp); });
    this->PrintDps->setIsGcdAbility(false);
 
    bool didPrintStats = false;
    this->PrintPlayerStats = new Ability("Print Stats");
    this->PrintPlayerStats->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 5.0f;});
    this->PrintPlayerStats->setOnCooldownTriggeredFunction([&](float timestamp, PlayerCharacter *PC, int32_t rank){ if(!didPrintStats){didPrintStats=true;float specDamageBonus = PC->calculateGlobalDamageBonus();COMBAT_LOG(timestamp, PC, PC->getName()<<": Level "<<PC->getLevel()<<" "<<PC->getPlayerRace()<<" "<<PC->getPlayerClass().getClassName()<< ", "<<"AP="<<PC->calculateMeleeAttackPower()<<", "<<"WepMin="<<(int32_t)(specDamageBonus*PC->calculatedWeaponMinDamage())<<", "<<"WepMax="<<(int32_t)(specDamageBonus*PC->calculatedWeaponMaxDamage()));} });
    this->PrintPlayerStats->setIsGcdAbility(false);
    
    this->MeleeMainhandAutoAttack = new Ability("Main-hand attack");
    this->MeleeMainhandAutoAttack->setAbilityDamageType(AbilityDamageType::Physical);
    this->MeleeMainhandAutoAttack->setDamageFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculateSimulatedMainhandSwing();});
    this->MeleeMainhandAutoAttack->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculatedMainhandWeaponSpeed();});
    this->MeleeMainhandAutoAttack->setIsGcdAbility(false);
    this->MeleeMainhandAutoAttack->setResourceGenerationFunction(
                [](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){
        if (PC->getPlayerClass().cls == PC->getPlayerClass().Warrior)
        {
            float d=damageDone;int32_t lvl=PC->getLevel();
            float c=((0.0091107836*lvl*lvl) + 3.225598133*lvl) + 4.2652911;
            float f=(isCritical ? 7.0f : 3.5f);
            float s=PC->calculatedMainhandWeaponSpeed();
            return (int32_t)(
                        (15*d)/(4*c) + (f*s)/2
                        );
        } 
        return 0;
    });
    
    this->MeleeOffhandAutoAttack = new Ability("Off-hand attack");
    this->MeleeOffhandAutoAttack->setAbilityDamageType(AbilityDamageType::Physical);
    this->MeleeOffhandAutoAttack->setCanUseFunction([](PlayerCharacter *PC, int32_t rank){return PC->getOffHandItem() != nullptr;});
    this->MeleeOffhandAutoAttack->setDamageFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculateSimulatedOffhandSwing();});
    this->MeleeOffhandAutoAttack->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculatedOffhandWeaponSpeed();});
    this->MeleeOffhandAutoAttack->setIsGcdAbility(false);
    
    this->Rend = new Ability("Rend");
    this->Rend->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *RendDebuff = new Buff("Rend", this->Rend);
    RendDebuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 18;});
    RendDebuff->setOnDotTickDamage([](Combatant *Cbt, int32_t rank, int32_t tickNumber){return DamageSimulation::Round(3*89.0f/18);});
    this->Rend->setIgnoresArmor(true);
    this->Rend->setResourceCost(10);
    this->Rend->setGrantedDebuff(RendDebuff);
    
    this->BattleShout = new Ability("Battle Shout");
    this->BattleShout->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *BattleShoutBuff = new Buff("Battle Shout", this->BattleShout);
    BattleShoutBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 120;});
    BattleShoutBuff->setOnCalculateAttackPower([](int32_t rank, int32_t AP){return AP + 92;});
    this->BattleShout->setResourceCost(10);
    this->BattleShout->setGrantedBuff(BattleShoutBuff);
    
    this->Whirlwind = new Ability("Whirlwind");
    this->Whirlwind->setAbilityDamageType(AbilityDamageType::Physical);
    this->Whirlwind->setDamageFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculateSimulatedMainhandSwing();});
    this->Whirlwind->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 10;});
    this->Whirlwind->setResourceCost(25);
    this->Whirlwind->setAoeMaxTargets(4);
    
    this->MortalStrike = new Ability("Mortal Strike");
    this->MortalStrike->setAbilityDamageType(AbilityDamageType::Physical);
    this->MortalStrike->setDamageFunction([](PlayerCharacter *PC, int32_t rank){float bonus = PC->calculateGlobalDamageBonus();return PC->calculateSimulatedMainhandSwing() + 85;});
    this->MortalStrike->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 6;});
    this->MortalStrike->setResourceCost(30);
    
    this->Slam = new Ability("Slam");
    this->Slam->setCastTime(1.5f);
    this->Slam->setAbilityDamageType(AbilityDamageType::Physical);
    this->Slam->setDamageFunction([](PlayerCharacter *PC, int32_t rank){float bonus = PC->calculateGlobalDamageBonus();return PC->calculateSimulatedMainhandSwing() + 43;});
    this->Slam->setResourceCost(15);
    
    this->Execute = new Ability("Execute");
    this->Execute->setAbilityDamageType(AbilityDamageType::Physical);
    this->Execute->setDamageFunction([](PlayerCharacter *PC, int32_t rank){return 325 + PC->getResource()*9;});
    this->Execute->setResourceGenerationFunction([](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){ return -100; });
    this->Execute->setCanUseFunction([](PlayerCharacter *PC, int32_t rank){float tHp = PC->getTarget()->getCurrentHp(); float tMaxHp = PC->getTarget()->getMaxHp(); float hpPct = tHp/tMaxHp; if (hpPct <= 0.20f) return true; return false;});
    this->Execute->setResourceCost(15);
    
    this->Charge = new Ability("Charge");
    this->Charge->setResourceGenerationFunction([](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){ int32_t impCharge = PC->getTalentRank("Improved Charge"); return 12 + impCharge*3; });
    bool didCharge = false;
    this->Charge->setCanUseFunction([&](PlayerCharacter *PC, int32_t rank){ if (!didCharge) {didCharge = true; return true;} return false; });
    this->Charge->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){ return 15; });
    this->Charge->setCastTime(0.2f);
    this->Charge->setIsGcdAbility(false);
}
