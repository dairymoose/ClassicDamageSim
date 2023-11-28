#include "GlobalAbilityList.h"

#include "PlayerCharacter.h"
#include "Buff.h"
#include "DamageSimulation.h"

void GlobalAbilityList::resetFields()
{
    this->didPrintStats = false;
    this->didCharge = false;
}

GlobalAbilityList::GlobalAbilityList()
{    
    this->resetFields();
    
    this->PrintDps = new Ability("Print DPS");
    this->PrintDps->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 5.0f;});
    this->PrintDps->setOnCooldownTriggeredFunction([](float timestamp, PlayerCharacter *PC, int32_t rank){ if (PC->getDamageDone() > 0) COMBAT_LOG(timestamp, PC, "pDmg="<<PC->getDamageDone()<<", pDPS="<<PC->getDamageDone()/timestamp); });
    this->PrintDps->setIsGcdAbility(false);
 
    this->PrintPlayerStats = new Ability("Print Stats");
    this->PrintPlayerStats->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 5.0f;});
    this->PrintPlayerStats->setOnCooldownTriggeredFunction([&](float timestamp, PlayerCharacter *PC, int32_t rank) { if(!didPrintStats){didPrintStats=true;float specDamageBonus = PC->calculateGlobalDamageBonus();COMBAT_LOG(timestamp, PC, PC->getName()<<": Level "<<PC->getLevel()<<" "<<PC->getPlayerRace()<<" "<<PC->getPlayerClass().getClassName()<< ", "<<"AP="<<PC->calculateMeleeAttackPower()<<", "<<"WepMin="<<(int32_t)(specDamageBonus*PC->calculatedWeaponMinDamage())<<", "<<"WepMax="<<(int32_t)(specDamageBonus*PC->calculatedWeaponMaxDamage()));} });
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
    RendDebuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return std::min(21, rank*3+6);});
    RendDebuff->setOnDotTickDamage([](Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration){int32_t dmg = 0; switch(rank){
        case 1:dmg=15;
            break;
        case 2:dmg=28;
            break;
        case 3:dmg=45;
            break;
        case 4:dmg=66;
            break;
        case 5:dmg=98;
            break;
        case 6:dmg=126;
            break;
        case 7:dmg=147;
            break;
        }
        float talentDmgBoost = 1.0f;
        if (PlayerCharacter *PC = dynamic_cast<PlayerCharacter *>(Caster)) {
            int32_t impRend = PC->getTalentRank("Improved Rend");
            if (impRend > 0) {
                talentDmgBoost = 1.0f + impRend*0.10f + 0.05f;
            }
        }
        return DamageSimulation::dotTickDamageFromTotalDamage(dmg * talentDmgBoost, 3.0f, buffDuration);});
    this->Rend->setIgnoresArmor(true);
    this->Rend->setResourceCost(10);
    this->Rend->setGrantedDebuff(RendDebuff);
    
    this->BattleShout = new Ability("Battle Shout");
    this->BattleShout->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *BattleShoutBuff = new Buff("Battle Shout", this->BattleShout);
    BattleShoutBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 120;});
    BattleShoutBuff->setOnCalculateAttackPower([](int32_t rank, int32_t AP){int32_t b=0;
                                                                            if (rank == 1) b = 20;
                                                                            if (rank == 2) b = 40;
                                                                            if (rank == 3) b = 60;
                                                                            if (rank == 4) b = 94;
                                                                            if (rank == 5) b = 139;
                                                                            if (rank == 6) b = 193;
                                                                            if (rank == 7) b = 232;
                                                                            return AP + b;});
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
    this->MortalStrike->setDamageFunction([](PlayerCharacter *PC, int32_t rank){int32_t d=0;
                                                                                if (rank == 1){d=85;}
                                                                                if (rank == 2){d=110;}
                                                                                if (rank == 3){d=135;}
                                                                                if (rank == 4){d=160;}
                                                                                return PC->calculateSimulatedMainhandSwing() + d;});
    this->MortalStrike->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 6;});
    this->MortalStrike->setResourceCost(30);
    
    this->Slam = new Ability("Slam");
    this->Slam->setCastTime(1.5f);
    this->Slam->setAbilityDamageType(AbilityDamageType::Physical);
    this->Slam->setDamageFunction([](PlayerCharacter *PC, int32_t rank){int32_t d=0;
                                                                        if (rank == 1){d=32;}
                                                                        if (rank == 2){d=43;}
                                                                        if (rank == 3){d=68;}
                                                                        if (rank == 4){d=87;}
                                                                        return PC->calculateSimulatedMainhandSwing() + d;});
    this->Slam->setResourceCost(15);
    
    this->Execute = new Ability("Execute");
    this->Execute->setAbilityDamageType(AbilityDamageType::Physical);
    this->Execute->setDamageFunction([](PlayerCharacter *PC, int32_t rank){int32_t d=0; int32_t r=0; 
                                                                           if (rank == 1){d=125;r=3;}
                                                                           if (rank == 2){d=200;r=6;}
                                                                           if (rank == 3){d=325;r=9;}
                                                                           if (rank == 4){d=450;r=12;}
                                                                           if (rank == 5){d=600;r=15;}
                                                                           return d + PC->getResource()*r;});
    this->Execute->setResourceGenerationFunction([](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){ return -100; });
    this->Execute->setCanUseFunction([](PlayerCharacter *PC, int32_t rank){float tHp = PC->getTarget()->getCurrentHp(); float tMaxHp = PC->getTarget()->getMaxHp(); float hpPct = tHp/tMaxHp; if (hpPct <= 0.20f) return true; return false;});
    this->Execute->setResourceCost(15);
    
    this->Charge = new Ability("Charge");
    this->Charge->setResourceGenerationFunction([](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){ int32_t impCharge = PC->getTalentRank("Improved Charge"); return 9 + (3*(rank-1)) + impCharge*3; });
    this->Charge->setCanUseFunction([&](PlayerCharacter *PC, int32_t rank){ if (!didCharge && PC->getDamageDone() == 0) {didCharge = true; return true;} return false; });
    this->Charge->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){ return 15; });
    this->Charge->setCastTime(0.01f);
    this->Charge->setIsGcdAbility(false);
    this->Charge->setCastedAbilityResetsAutoAttack(false);
    
    this->Bloodrage = new Ability("Bloodrage");
    this->Bloodrage->setResourceGenerationFunction([](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){ return 10; });
    this->Bloodrage->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){ return 60; });
    this->Bloodrage->setIsGcdAbility(false);
    Buff *BloodrageBuff = new Buff("Bloodrage", this->Rend);
    BloodrageBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 10;});
    BloodrageBuff->setOnCalculateDotTickPeriod([](Combatant *Cbt){ return 1; });
    BloodrageBuff->setOnBuffTick([](Combatant *Source, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration){ if (PlayerCharacter *PC = dynamic_cast<PlayerCharacter *>(Source)){PC->setResource(PC->getResource() + 1);} });
    this->Bloodrage->setGrantedBuff(BloodrageBuff);
}
