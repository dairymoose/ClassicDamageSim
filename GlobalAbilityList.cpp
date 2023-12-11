#include "GlobalAbilityList.h"

#include "PlayerCharacter.h"
#include "Buff.h"
#include "DamageSimulation.h"
#include <regex>

GlobalAbilityList* GlobalAbilityList::activeList = nullptr;

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
    this->PrintPlayerStats->setOnCooldownTriggeredFunction([&](float timestamp, PlayerCharacter *PC, int32_t rank) { if(!didPrintStats){didPrintStats=true;float specDamageBonus = PC->calculateGlobalDamageBonus();COMBAT_LOG(timestamp, PC, PC->getName()<<": Level "<<PC->getLevel()<<" "<<PC->getPlayerRace()<<" "<<PC->getPlayerClass().getClassName()<< ", "<<"AP="<<PC->calculateMeleeAttackPower()<<", "<<"WepMin="<<(int32_t)(specDamageBonus*PC->calculatedWeaponMinDamage())<<", "<<"WepMax="<<(int32_t)(specDamageBonus*PC->calculatedWeaponMaxDamage())<<", Speed="<<PC->calculatedMainhandWeaponSpeed()<<" speed");} });
    this->PrintPlayerStats->setIsGcdAbility(false);
    
    this->MeleeMainhandAutoAttack = new Ability("Main-hand attack");
    this->MeleeMainhandAutoAttack->setAbilityDamageType(AbilityDamageType::Physical);
    this->MeleeMainhandAutoAttack->setDamageFunction([](PlayerCharacter *PC, int32_t rank){
        return PC->calculateSimulatedMainhandSwing();
    });
    this->MeleeMainhandAutoAttack->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculatedMainhandWeaponSpeed();});
    this->MeleeMainhandAutoAttack->setIsGcdAbility(false);
    this->MeleeMainhandAutoAttack->setResourceGenerationFunction(
                [](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){
        if (PC->getPlayerClass().cls == PC->getPlayerClass().WARRIOR)
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
    this->MeleeOffhandAutoAttack->setCanUseFunction([](PlayerCharacter *PC, int32_t rank, float timestamp){return PC->getOffHandItem() != nullptr;});
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
            
            if (PC->hasRune("Blood Frenzy")) {
                PC->setResource(PC->getResource() + 3);
            }
        }
        return DamageSimulation::dotTickDamageFromTotalDamage(dmg * talentDmgBoost, 3.0f, buffDuration);});
    this->Rend->setIgnoresArmor(true);
    this->Rend->setResourceCost(10);
    this->Rend->setGrantedDebuff(RendDebuff);
    this->Rend->setTooltipText("Wounds the target causing them to bleed for <dmg> damage over <time> sec.");
    this->Rend->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return DamageSimulation::regexReplaceTooltipDotDuration(DamageSimulation::regexReplaceTooltipDotDamage(tooltipText, ability, PC), ability, PC);
    });
    
    this->BattleShout = new Ability("Battle Shout");
    this->BattleShout->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *BattleShoutBuff = new Buff("Battle Shout", this->BattleShout);
    BattleShoutBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 99999;});
    BattleShoutBuff->setOnCalculateAttackPower([](Combatant *Cbt, int32_t rank, int32_t AP){
        if (PlayerCharacter *PC = dynamic_cast<PlayerCharacter *>(Cbt)) {
            float talentBoost = 1.0f + PC->getTalentRank("Improved Battle Shout")*0.05f;
            int32_t b=0;
            if (rank == 1) b = 15;
            if (rank == 2) b = 40;
            if (rank == 3) b = 60;
            if (rank == 4) b = 94;
            if (rank == 5) b = 139;
            if (rank == 6) b = 193;
            if (rank == 7) b = 232;
            return (int32_t)(AP + b*talentBoost);
        }
        return AP;
    });
    this->BattleShout->setResourceCost(10);
    this->BattleShout->setGrantedBuff(BattleShoutBuff);
    this->BattleShout->getLearnLevels().push_back(1);
    this->BattleShout->getLearnLevels().push_back(12);
    this->BattleShout->getLearnLevels().push_back(22);
    this->BattleShout->getLearnLevels().push_back(32);
    this->BattleShout->getLearnLevels().push_back(42);
    this->BattleShout->getLearnLevels().push_back(52);
    this->BattleShout->getLearnLevels().push_back(60);
    this->BattleShout->setTooltipText("The warrior shouts, increasing the melee attack power of all party members within 20 yards by <AP>.  Lasts 2 min.");
    this->BattleShout->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        std::stringstream ss;
        ss<<ability->getGrantedBuff()->getOnCalculateAttackPower()(PC, ability->getRank(), 0);
        return std::regex_replace(tooltipText, std::regex("<AP>"), ss.str());
    });
    
    this->Whirlwind = new Ability("Whirlwind");
    this->Whirlwind->setAbilityDamageType(AbilityDamageType::Physical);
    this->Whirlwind->setDamageFunction([](PlayerCharacter *PC, int32_t rank){return PC->calculateSimulatedMainhandSwing();});
    this->Whirlwind->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 10;});
    this->Whirlwind->setResourceCost(25);
    this->Whirlwind->setAoeMaxTargets(4);
    this->Whirlwind->setTooltipText("In a whirlwind of steel you attack up to 4 enemies within 8 yards, causing weapon damage to each enemy. (<dmg> damage)");
    this->Whirlwind->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return DamageSimulation::regexReplaceTooltipDirectDamage(tooltipText, ability, PC);
    });
    
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
    this->MortalStrike->setTooltipText("A vicious strike that deals <dmg> damage and wounds the target, reducing the effectiveness of any healing by 50% for 10 sec.");
    this->MortalStrike->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return DamageSimulation::regexReplaceTooltipDirectDamage(tooltipText, ability, PC);
    });
    
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
    this->Execute->setCanUseFunction([](PlayerCharacter *PC, int32_t rank, float timestamp){float tHp = PC->getTarget()->getCurrentHp(); float tMaxHp = PC->getTarget()->getMaxHp(); float hpPct = tHp/tMaxHp; if (hpPct <= 0.20f) return true; return false;});
    this->Execute->setResourceCost(15);
    this->Execute->setOnGetResourceCostModifier([](PlayerCharacter *PC, int32_t rank, int32_t resourceCost){int32_t impEx=PC->getTalentRank("Improved Execute");if (impEx == 0) return resourceCost; if (impEx == 1) return resourceCost-2; if (impEx == 2) return resourceCost-5;});
    
    this->Charge = new Ability("Charge");
    this->Charge->setResourceGenerationFunction([](PlayerCharacter *PC, int32_t rank, int32_t damageDone, bool isCritical){ int32_t impCharge = PC->getTalentRank("Improved Charge"); return 9 + (3*(rank-1)) + impCharge*3; });
    this->Charge->setCanUseFunction([&](PlayerCharacter *PC, int32_t rank, float timestamp){ if (!didCharge && PC->getDamageDone() == 0) {didCharge = true; return true;} return false; });
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
    this->Bloodrage->setTooltipText("Generates 10 rage at the cost of health, and then generates an additional 10 rage over 10 sec.  The warrior is considered in combat for the duration.");
    this->Bloodrage->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return tooltipText;
    });
            
    this->HeroicStrike = new Ability("Heroic Strike");
    this->HeroicStrike->setIsGcdAbility(false);
    this->HeroicStrike->setAbilityDamageType(AbilityDamageType::Physical);
    this->HeroicStrike->setDamageFunction([](PlayerCharacter *PC, int32_t rank){int32_t d=0;
                                                                        if (rank == 1){d=11;}
                                                                        if (rank == 2){d=21;}
                                                                        if (rank == 3){d=32;}
                                                                        if (rank == 4){d=44;}
                                                                        if (rank == 5){d=58;}
                                                                        if (rank == 6){d=80;}
                                                                        if (rank == 7){d=111;}
                                                                        if (rank == 8){d=138;}
                                                                        if (rank == 9){d=157;}
                                                                        return PC->calculateSimulatedMainhandSwing() + d;});
    this->HeroicStrike->setReplacesNextMelee(true);
    this->HeroicStrike->setResourceCost(15);
    this->HeroicStrike->setOnGetResourceCostModifier([](PlayerCharacter *PC, int32_t rank, int32_t resourceCost){int32_t impHS=PC->getTalentRank("Improved Heroic Strike");return resourceCost-impHS;});
    
    this->QuickStrike = new Ability("Quick Strike");
    this->QuickStrike->setAbilityDamageType(AbilityDamageType::Physical);
    this->QuickStrike->setDamageFunction([](PlayerCharacter *PC, int32_t rank){
        float rangeMin = 0.15f;
        float rangeMax = 0.25f;
        float rangeAvg = (rangeMin + rangeMax)/2.0f;
        float r;
        if (PC->getAlwaysUseAverageDamage()) {
            r = rangeAvg;
        } else {
            r = DamageSimulation::randomFloatBetween(rangeMin, rangeMax);
        }
        return r*PC->calculateMeleeAttackPower();
    });
    this->QuickStrike->setResourceCost(20);
    this->QuickStrike->setTooltipText("A reckless instant melee attack with your two-handed weapon dealing (Attack power * 15 / 100) to (Attack power * 25 / 100) physical damage. This ability benefits from and triggers all effects associated with Heroic Strike. (<dmg> damage)");
    this->QuickStrike->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return DamageSimulation::regexReplaceTooltipDirectDamage(tooltipText, ability, PC);
    });
    this->QuickStrike->setOnGetResourceCostModifier([](PlayerCharacter *PC, int32_t rank, int32_t resourceCost){int32_t impHS=PC->getTalentRank("Improved Heroic Strike");return resourceCost-impHS;});
    
    this->RagingBlow = new Ability("Raging Blow");
    this->RagingBlow->setAbilityDamageType(AbilityDamageType::Physical);
    this->RagingBlow->setDamageFunction([](PlayerCharacter *PC, int32_t rank){
        return PC->calculateSimulatedMainhandSwing();});
    this->RagingBlow->setCanUseFunction([](PlayerCharacter *PC, int32_t rank, float timestamp){
        if (
                PC->hasBuff(GlobalAbilityList::activeList->Bloodrage->getGrantedBuff()) ||
                PC->hasBuff(GlobalAbilityList::activeList->Enrage)) 
            return true;
        return false;
    });
    this->RagingBlow->setCooldownFunction([](PlayerCharacter *PC, int32_t abilityRank){return 8;});
    this->RagingBlow->setTooltipText("A ferocious strike that deals 100% weapon damage, but can only be used while Enrage, Berserker Rage, or Bloodrage is active. (<dmg> damage)");
    this->RagingBlow->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return DamageSimulation::regexReplaceTooltipDirectDamage(tooltipText, ability, PC);
    });
    
    this->Enrage = new Buff("Enrage", nullptr);
    this->Enrage->setOnCalculateDuration([](Combatant *Cbt, int32_t rank){return 12.0f;});
    this->Enrage->setOnGetMeleeAutoAttackMultiplier([](Combatant *Cbt){return 1.20f;});
    
    this->WildStrikes = new Buff("Wild Strikes", nullptr);
    this->WildStrikes->setOnCalculateDuration([](Combatant *Cbt, int32_t rank){return 99999.0f;});
    this->WildStrikes->setOnAbilityDamageMelee([&](Combatant *Cbt, float timestamp){
        if (PlayerCharacter *PC = dynamic_cast<PlayerCharacter *>(Cbt)){
            int32_t AP=PC->calculateMeleeAttackPower();
            if (PC->getBakeWildStrikesIntoAverageDamage()) {
                int32_t wildStrikeAp = (int32_t)(AP * 1.20f);
                PC->setAttackPowerOverride(wildStrikeAp);
                PriorityAction *PA = PC->getPriorityActionList()->getMainHandAutoAttackAction();
                if (PA != nullptr) {
                    std::vector<Enemy *> enemyList;
                    enemyList.push_back(PC->getTarget());
                    inExtraAttack = true;
                    PC->setGlobalDamageModifier(0.20f);
                    PA->execute(PC, enemyList, timestamp, false);
                    PC->setGlobalDamageModifier(1.0f);
                    inExtraAttack = false;
                }
                PC->setAttackPowerOverride(-1);
            } else {
                float roll = DamageSimulation::randomFloatBetween(0.0f, 1.0f);
                float wildStrikeChance = 0.20f;
                if (roll < wildStrikeChance) {
                    COMBAT_LOG(timestamp, PC, "*** WILD STRIKE! ***");
                    int32_t wildStrikeAp = (int32_t)(AP * 1.20f);
                    PC->setAttackPowerOverride(wildStrikeAp);
                    PriorityAction *PA = PC->getPriorityActionList()->getMainHandAutoAttackAction();
                    if (PA != nullptr) {
                        std::vector<Enemy *> enemyList;
                        enemyList.push_back(PC->getTarget());
                        inExtraAttack = true;
                        PA->execute(PC, enemyList, timestamp, false);
                        inExtraAttack = false;
                    }
                    //int32_t wildStrikeSwing = PC->calculateSimulatedMainhandSwing();
                    PC->setAttackPowerOverride(-1);
                }
            }
        }});
    
    this->FreeWildStrikes = new Ability("Free: Wild Strikes");
    this->FreeWildStrikes->setAbilityDamageType(AbilityDamageType::Physical);
    this->FreeWildStrikes->setResourceCost(0);
    this->FreeWildStrikes->setIsGcdAbility(false);
    this->FreeWildStrikes->setGrantedBuff(this->WildStrikes);
    this->FreeWildStrikes->setTooltipText("Free wild strikes buff");
    this->FreeWildStrikes->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return tooltipText;
    });
    
    this->DeepWounds = new Buff("Deep Wounds", nullptr);
    this->DeepWounds->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 12;});
    this->DeepWounds->setOnDotTickDamage([](Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration){
        if (PlayerCharacter *PC = dynamic_cast<PlayerCharacter *>(Caster)) {
            float dmgPct = 0.20f * PC->getTalentRank("Deep Wounds");
            int32_t totalDamage = PC->calculateSimulatedMainhandSwing() * dmgPct;
            if (PC->hasRune("Blood Frenzy")) {
                PC->setResource(PC->getResource() + 3);
            }
            return DamageSimulation::dotTickDamageFromTotalDamage(totalDamage, 3.0f, buffDuration);
        }
    });
    this->DeepWounds->setIgnoresArmor(true);
    
    this->Overpower = new Ability("Overpower");
    this->Overpower->setAbilityDamageType(AbilityDamageType::Physical);
    this->Overpower->setDamageFunction([](PlayerCharacter *PC, int32_t rank){int32_t d=0;
                                                                                if (rank == 1){d=5;}
                                                                                if (rank == 2){d=15;}
                                                                                if (rank == 3){d=25;}
                                                                                if (rank == 4){d=35;}
                                                                                return PC->calculateSimulatedMainhandSwing() + d;});
    this->Overpower->setCooldownFunction([](PlayerCharacter *PC, int32_t rank){return 5;});
    this->Overpower->setResourceCost(5);
    this->Overpower->setCanUseFunction([](PlayerCharacter *PC, int32_t rank, float timestamp){if (PC->getTarget() != nullptr && PC->getTarget()->getLastDodgeTimestamp() != -1.0f && (timestamp - PC->getTarget()->getLastDodgeTimestamp()) < 5.0f) return true; return false;});
    this->Overpower->setOnCooldownTriggeredFunction([](float timestamp, PlayerCharacter *PC, int32_t rank){if (PC->getTarget() != nullptr) PC->getTarget()->setLastDodgeTimestamp(-1.0f);});
    this->Overpower->setTooltipText("Instantly overpower the enemy, causing weapon damage plus 5.  Only useable after the target dodges.  The Overpower cannot be blocked, dodged or parried. (<dmg> damage)");
    this->Overpower->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return DamageSimulation::regexReplaceTooltipDirectDamage(tooltipText, ability, PC);
    });
    this->Overpower->setCannotBeBlockedOrDodgedOrParried(true);
    
    this->FreeBlessingOfMight = new Ability("Blessing of Might");
    this->FreeBlessingOfMight->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *BlessingOfMightBuff = new Buff("Blessing of Might", this->FreeBlessingOfMight);
    BlessingOfMightBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 99999;});
    BlessingOfMightBuff->setOnCalculateAttackPower([](Combatant *Cbt, int32_t rank, int32_t AP){
        if (PlayerCharacter *PC = dynamic_cast<PlayerCharacter *>(Cbt)) {
            int32_t b=0;
            if (rank == 1) b = 20;
            if (rank == 2) b = 35;
            if (rank == 3) b = 55;
            if (rank == 4) b = 85;
            if (rank == 5) b = 115;
            if (rank == 6) b = 155;
            if (rank == 7) b = 185;
            return (int32_t)(AP + b);
        }
        return AP;
    });
    this->FreeBlessingOfMight->setResourceCost(0);
    this->FreeBlessingOfMight->setCanUseFunction([=](PlayerCharacter *PC, int32_t rank, float timestamp){if (!PC->hasBuff(BlessingOfMightBuff)) return true; return false;});
    this->FreeBlessingOfMight->setGrantedBuff(BlessingOfMightBuff);
    this->FreeBlessingOfMight->setTooltipText("Places a Blessing on the friendly target, increasing melee attack power by <AP> for 5 min.  Players may only have one Blessing on them per Paladin at any one time.");
    this->FreeBlessingOfMight->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        std::stringstream ss;
        ss<<ability->getGrantedBuff()->getOnCalculateAttackPower()(PC, ability->getRank(), 0);
        return std::regex_replace(tooltipText, std::regex("<AP>"), ss.str());
    });
    
    this->FreeBlessingOfKings = new Ability("Blessing of Kings");
    this->FreeBlessingOfKings->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *BlessingOfKingsBuff = new Buff("Blessing of Kings", this->FreeBlessingOfKings);
    BlessingOfKingsBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 99999;});
    BlessingOfKingsBuff->setOnCalculatePercentStatBoost([](PlayerCharacter *PC, int32_t rank){return 1.10f;});
    this->FreeBlessingOfKings->setResourceCost(0);
    this->FreeBlessingOfKings->setCanUseFunction([=](PlayerCharacter *PC, int32_t rank, float timestamp){if (!PC->hasBuff(BlessingOfKingsBuff)) return true; return false;});
    this->FreeBlessingOfKings->setGrantedBuff(BlessingOfKingsBuff);
    this->FreeBlessingOfKings->setTooltipText("Places a Blessing on the friendly target, increasing total stats by 10% for 5 min.  Players may only have one Blessing on them per Paladin at any one time.");
    this->FreeBlessingOfKings->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return tooltipText;
    });
    
    this->FreeMarkOfTheWild = new Ability("Mark Of The Wild");
    this->FreeMarkOfTheWild->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *MarkOfTheWildBuff = new Buff("Mark Of The Wild", this->FreeMarkOfTheWild);
    MarkOfTheWildBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 99999;});
    MarkOfTheWildBuff->setOnCalculateAllStatsBoost([](PlayerCharacter *PC, int32_t rank){return 4;});
    MarkOfTheWildBuff->setOnCalculateArmorBoost([](PlayerCharacter *PC, int32_t rank){return 105;});
    this->FreeMarkOfTheWild->setResourceCost(0);
    this->FreeMarkOfTheWild->setCanUseFunction([=](PlayerCharacter *PC, int32_t rank, float timestamp){if (!PC->hasBuff(MarkOfTheWildBuff)) return true; return false;});
    this->FreeMarkOfTheWild->setGrantedBuff(MarkOfTheWildBuff);
    this->FreeMarkOfTheWild->setTooltipText("Increases the friendly target's armor by 105 and all attributes by 4 for 30 min.");
    this->FreeMarkOfTheWild->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return tooltipText;
    });
    
    this->FreeStrengthBonus = new Ability("Strength");
    this->FreeStrengthBonus->setAbilityDamageType(AbilityDamageType::Physical);
    Buff *FreeStrengthBonusBuff = new Buff("Strength", this->FreeStrengthBonus);
    FreeStrengthBonusBuff->setOnCalculateDuration([&](Combatant *Cbt, int32_t rank){return 99999;});
    FreeStrengthBonusBuff->setOnCalculateStrengthBoost([](PlayerCharacter *PC, int32_t rank){return 5;});
    this->FreeStrengthBonus->setResourceCost(0);
    this->FreeStrengthBonus->setCanUseFunction([=](PlayerCharacter *PC, int32_t rank, float timestamp){if (!PC->hasBuff(FreeStrengthBonusBuff)) return true; return false;});
    this->FreeStrengthBonus->setGrantedBuff(FreeStrengthBonusBuff);
    this->FreeStrengthBonus->setTooltipText("Increases the target's strength by 5.");
    this->FreeStrengthBonus->setOnGetTooltip([](std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability){
        return tooltipText;
    });
    
    activeList = this;
}
