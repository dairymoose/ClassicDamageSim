#include "DamageSimulation.h"
#include "GlobalAbilityList.h"
#include "CombatLog.h"

std::random_device DamageSimulation::rd;
std::mt19937 DamageSimulation::randEngine(DamageSimulation::rd());

PlayerCharacter *DamageSimulation::getPC() const
{
    return PC;
}

void DamageSimulation::setPC(PlayerCharacter *value)
{
    PC = value;
}

bool DamageSimulation::almostEqual(float a, float b, float threshold)
{
    float diff = a - b;
    if (diff < 0.0) {
        diff = -diff;
    }
    return diff <= threshold;
}

void DamageSimulation::simulate(PriorityActionList *priorityActions)
{
    PC->setPriorityActionList(priorityActions);
    priorityActions->resetAllAbilities();
    for(;;) {
        if (this->noEnemiesExist()) {
            COMBAT_LOG(this->time, this->PC, "Combat has ended because no enemies exist!");
            break;
        }
        if (PC->getTarget() == nullptr && this->enemyList.size() > 0) {
            PC->setTarget(this->enemyList[0]);
        }
        
        std::vector<Combatant *> combatants;
        this->gatherAllCombatants(combatants);
        for (auto combatant : combatants) {
            combatant->applyDotDamage(this->PC, this->time);
            combatant->removeExpiredBuffs(this->time);
            combatant->removeExpiredDebuffs(this->time);
            if (combatant->getIsCasting()) {
                if (combatant->isSpellcastFinished(this->time) && combatant == PC) {
                    COMBAT_LOG(this->time, PC, ATTACKER_FONT_COLOR<<PC->getName()<<END_FONT<<" finished casting "<<ABILITY_FONT_COLOR<<combatant->getCastingAbility()->getName()<<END_FONT);
                    PriorityAction PA(combatant->getCastingAbility());
                    PA.execute(PC, this->enemyList, this->time);
                    combatant->triggerSpellcastFinished();
                }
            }
        }
        if (Talent *AM = PC->getTalent("Anger Management")) {
            if (AM->getRank() > 0) {
                float timeDiff = this->time - AM->getLastUsedTimestamp();
                timeDiff += FLOATING_POINT_SMALL_NUMBER;
                if(timeDiff >= 3.0f) {
                    AM->setLastUsedTimestamp(this->time);
                    PC->setResource(PC->getResource() + 1);
                    COMBAT_LOG(this->time, PC, ATTACKER_FONT_COLOR<<PC->getName()<<END_FONT<<" gained 1 rage from "<<ABILITY_FONT_COLOR<<"Anger Management"<<END_FONT);
                }
            }
        }
        if (!this->allEnemiesAreDead()) {
            int32_t actionsTaken = 0;
            while (actionsTaken < this->maxActionsPerTimeStep) {
                ++actionsTaken;

                PriorityAction *PA = priorityActions->getNextAction(this->PC, this->time);
                if (PA != nullptr) {
                    if (PA->getAbility() != nullptr && PA->getAbility()->getCastTime() > 0.0f) {
                        COMBAT_LOG(this->time, PC, ATTACKER_FONT_COLOR<<PC->getName()<<END_FONT<<" begins casting "<<ABILITY_FONT_COLOR<<PA->getAbility()->getName()<<END_FONT);
                        PC->setCastingAbility(PA->getAbility());
                        PC->setCastStartTime(this->time);
                        PC->setIsCasting(true);
                        PriorityAction *mhAttack = PC->getPriorityActionList()->getActionFromAbilityName("Main-hand attack");
                        PriorityAction *ohAttack = PC->getPriorityActionList()->getActionFromAbilityName("Off-hand attack");
                        if (mhAttack)
                            mhAttack->getAbility()->triggerCooldown(PC, this->time, false);
                        if (ohAttack)
                            ohAttack->getAbility()->triggerCooldown(PC, this->time, false);
                    } else {
                        PA->execute(this->PC, this->enemyList, this->time);
                    }
                    if (this->allEnemiesAreDead()) {
                        break;
                    }
                }
                else {
                    break;
                }
            }
        }
        if (this->allEnemiesAreDead()) {
            COMBAT_LOG(this->time, this->PC, "Combat has ended because all enemies have perished");
            break;
        }
        this->time += this->timeStep;
        if (this->maxCombatTime > 0.0f && this->time >= this->maxCombatTime) {
            COMBAT_LOG(this->time, this->PC, "Combat has ended because time has exceeded the max time of "<<this->maxCombatTime);
            break;
        }
    }
    if (globalAbilityList != nullptr) {
        if (globalAbilityList->PrintDps != nullptr) {
            PriorityAction PA(globalAbilityList->PrintDps);
            PA.execute(this->PC, this->enemyList, this->time);
        }
    }
}

void DamageSimulation::gatherAllCombatants(std::vector<Combatant *> &combatants) {
    combatants.push_back(this->PC);
    for (auto enemy : this->enemyList) {
        combatants.push_back(enemy);
    }
}

float DamageSimulation::getTimeStep() const
{
    return timeStep;
}

void DamageSimulation::setTimeStep(float value)
{
    timeStep = value;
}

bool DamageSimulation::noEnemiesExist()
{
    return this->enemyList.empty();
}

bool DamageSimulation::allEnemiesAreDead()
{
    for (int i=0; i<this->enemyList.size(); ++i) {
        if (!this->enemyList[i]->isDead()) {
            return false;
        }
    }
    return true;
}

std::vector<Enemy *>& DamageSimulation::getEnemyList()
{
    return enemyList;
}

GlobalAbilityList *DamageSimulation::getGlobalAbilityList() const
{
    return globalAbilityList;
}

void DamageSimulation::setGlobalAbilityList(GlobalAbilityList *value)
{
    globalAbilityList = value;
}

std::mt19937& DamageSimulation::getRandEngine()
{
    return randEngine;
}

DamageSimulation::DamageSimulation()
{
    
}
