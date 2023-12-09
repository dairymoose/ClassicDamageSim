#include "DamageSimulation.h"
#include "GlobalAbilityList.h"
#include "CombatLog.h"
#include <cfloat>

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

void DamageSimulation::resetIterationsData()
{
    this->iterationCount = 0;
    this->iterationsTotalTimeLength = 0.0f;
    this->iterationsDpsSummation = 0.0f;
    this->iterationsMinDps = -1.0f;
    this->iterationsMaxDps = -1.0f;
    this->iterationsDamageDoneByBuff.clear();
    this->iterationsDamageDoneByAbility.clear();
}

void DamageSimulation::reset()
{
    std::vector<Combatant *> combatants;
    this->gatherAllCombatants(combatants);
    for (auto combatant : combatants) {
        combatant->setHp(combatant->getMaxHp());
        combatant->setIsCasting(false);
        combatant->setIsGcdActive(false);
        combatant->setDamageDone(0);
        combatant->setLastDodgeTimestamp(-1.0f);
        combatant->clearAllBuffsAndDebuffsAndFreeMemory();
    }
    if (this->PC != nullptr) {
        this->PC->initResourceValue();
        this->PC->getCombatLog()->clear();
        this->PC->resetAllTalentTimestamps();
        this->PC->getDamageDoneByAbility().clear();
        this->PC->getDamageDoneByBuff().clear();
        this->PC->setLastUsedAction(nullptr);
    }
    this->time = 0.0f;
    if (this->globalAbilityList != nullptr) {
        this->globalAbilityList->resetFields();
    }
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
        PC->tickBuffs(this->PC, this->time);
        for (auto combatant : combatants) {
            combatant->applyDotDamage(this->PC, this->time);
            combatant->removeExpiredBuffs(this->time);
            combatant->removeExpiredDebuffs(this->time);
            if (combatant->getIsCasting()) {
                if (combatant->isSpellcastFinished(this->time) && combatant == PC) {
                    COMBAT_LOG(this->time, PC, ATTACKER_FONT_COLOR<<PC->getName()<<END_FONT<<" finished casting "<<ABILITY_FONT_COLOR<<combatant->getCastingAbility()->getName()<<END_FONT);
                    PriorityAction PA(combatant->getCastingAbility());
                    PA.execute(PC, this->enemyList, this->time);
                    if (combatant->getCastingAbility()->getCastedAbilityResetsAutoAttack())
                        PC->enableAndResetAutoAttack(this->time);
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
                        if (PA->getAbility()->getCastedAbilityResetsAutoAttack())
                            PC->disableAutoAttack();
                    } else if (PA->getAbility() != nullptr && PA->getAbility()->getReplacesNextMelee()) {
                        PC->setReplaceMeleeAction(PA);
                    } else {
                        if (PA->isMainhandAutoAttack() && PC->getReplaceMeleeAction() != nullptr) {
                            PC->getReplaceMeleeAction()->execute(this->PC, this->enemyList, this->time);
                            PC->setReplaceMeleeAction(nullptr);
                            PA->getAbility()->triggerCooldown(PC, this->time, PA->getIgnoreGcd());
                        } else {
                            PA->execute(this->PC, this->enemyList, this->time);
                        }
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
    this->iterationsTotalTimeLength += this->time;
    float dps = PC->getDamageDone()/this->time;
    if (iterationsMinDps == -1.0f) {
        iterationsMinDps = dps;
    }
    if (iterationsMaxDps == -1.0f) {
        iterationsMaxDps = dps;
    }
    if (dps < iterationsMinDps) {
        iterationsMinDps = dps;
    }
    if (dps > iterationsMaxDps) {
        iterationsMaxDps = dps;
    }
    this->iterationsDpsSummation += dps;
    for (auto& it : PC->getDamageDoneByAbility()) {
        auto&& found = this->iterationsDamageDoneByAbility.find(it.first);
        int32_t damage = 0;
        if (found != this->iterationsDamageDoneByAbility.end()) {
            damage = found->second;
        }
        damage += it.second;
        this->iterationsDamageDoneByAbility[it.first] = damage;
    }
    for (auto& it : PC->getDamageDoneByBuff()) {
        auto&& found = this->iterationsDamageDoneByBuff.find(it.first);
        int32_t damage = 0;
        if (found != this->iterationsDamageDoneByBuff.end()) {
            damage = found->second;
        }
        damage += it.second;
        this->iterationsDamageDoneByBuff[it.first] = damage;
    }
    ++iterationCount;
}

struct DpsElement {
    std::string name;
    int32_t damage;
    
    DpsElement(std::string name, int32_t damage) {
        this->name = name;
        this->damage = damage;
    }
};

struct DpsElementSort {
    bool operator()(const DpsElement& a, const DpsElement& b) {
        return a.damage > b.damage;
    }
};

void DamageSimulation::printIterationSummary(std::ostream &stream)
{
    stream<<"Damage Summary<br>";
    stream<<"Level "<<PC->getLevel()<<" "<<PC->getPlayerRace()<<" "<<PC->getPlayerClass().getClassName()<<": "<<PC->getName()<<"<br>";
    stream<<"Iterations: "<<this->iterationCount<<"<br>";
    stream<<"Average DPS: "<<(this->iterationsDpsSummation/this->iterationCount)<<"<br>";
    stream<<"Min DPS: "<<this->iterationsMinDps<<"<br>";
    stream<<"Max DPS: "<<this->iterationsMaxDps<<"<br>";
    stream<<"Combat Duration: "<<(this->iterationsTotalTimeLength/this->iterationCount)<<"<br>";
    stream<<"<br>";
    stream<<"Attack Table:";
    stream<<"<br>";
    MeleeAttackTable mat;
    PC->getPriorityActionList()->getMainHandAutoAttackAction()->getAbility()->generateMeleeAttackTable(PC, PC->getTarget(), mat);
    PC->getPriorityActionList()->getMainHandAutoAttackAction()->getAbility()->printMeleeAttackTable(mat, stream);
    
    stream<<"<br>";
    
    int64_t damageTotal = 0;
    std::vector<DpsElement> dpsElements;
    for (auto& it : this->iterationsDamageDoneByAbility) {
        damageTotal += it.second;
        dpsElements.emplace_back(it.first->getName(), it.second);
    }
    for (auto& it : this->iterationsDamageDoneByBuff) {
        damageTotal += it.second;
        dpsElements.emplace_back(it.first->getName(), it.second);
    }
    std::sort(dpsElements.begin(), dpsElements.end(), DpsElementSort());
    for (int i=0; i<dpsElements.size(); ++i) {
        float dmgPct = (100.0f*(float)dpsElements[i].damage/damageTotal);
        stream<<dpsElements[i].name<<": "<<(dpsElements[i].damage/this->iterationCount)<<" (<font color=\"green\">"<<std::fixed<<std::setprecision(2)<<dmgPct<<"</font>%)"<<"<br>";
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

float DamageSimulation::randomFloatBetween(float a, float b)
{
    std::uniform_real_distribution<> dist(a, std::nextafter(b, FLT_MAX));
    return dist(DamageSimulation::getRandEngine());
}

int32_t DamageSimulation::randomIntBetween(int32_t a, int32_t b)
{
    std::uniform_int_distribution<> dist(a, b);
    return dist(DamageSimulation::getRandEngine());
}

float DamageSimulation::getTime() const
{
    return time;
}

DamageSimulation::DamageSimulation()
{
    
}
