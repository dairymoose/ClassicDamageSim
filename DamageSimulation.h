#ifndef DAMAGESIMULATION_H
#define DAMAGESIMULATION_H

#include "PlayerCharacter.h"
#include "PriorityActionList.h"
#include "Enemy.h"
#include <random>
#include <regex>

#define FLOATING_POINT_SMALL_NUMBER 1E-3f

class GlobalAbilityList;

class DamageSimulation
{
    static std::random_device rd;
    static std::mt19937 randEngine;
    
    PlayerCharacter *PC;
    std::vector<Enemy *> enemyList;
    GlobalAbilityList *globalAbilityList = nullptr;
    float timeStep = 0.10f;
    float time = 0.0f;
    float maxCombatTime = 10*60.0f;
    int32_t maxActionsPerTimeStep = 20;
    
    int32_t iterationCount = 0;
    float iterationsTotalTimeLength = 0.0f;
    float iterationsMinDps = -1.0f;
    float iterationsMaxDps = -1.0f;
    float iterationsDpsSummation = 0.0f;
    std::unordered_map<Ability *, TrackedDamageInfo> iterationsDamageDoneByAbility;
    std::unordered_map<Buff *, TrackedDamageInfo> iterationsDamageDoneByBuff;
    std::vector<float> iterationsRecordedDps;
public:
    DamageSimulation();
    PlayerCharacter *getPC() const;
    void setPC(PlayerCharacter *value);
    
    void resetIterationsData();
    void reset();
    
    void simulate(PriorityActionList *priorityActions);
    void printIterationSummary(std::ostream& stream);
    float getTimeStep() const;
    void setTimeStep(float value);
    bool noEnemiesExist();
    bool allEnemiesAreDead();
    void gatherAllCombatants(std::vector<Combatant *>& combatants);
    
    static bool almostEqual(float a, float b, float threshold=FLOATING_POINT_SMALL_NUMBER);
    static int32_t Round(float toRound) {
        return (int32_t)(toRound + 0.5f);
    }
    static int32_t dotTickDamageFromTotalDamage(int32_t totalDamage, float tickPeriod, float totalDuration) {
        return DamageSimulation::Round(tickPeriod*totalDamage/totalDuration);
    }
    static int32_t totalDotDamageForAbility(Ability *ability, PlayerCharacter *PC) {
        if (ability == nullptr) {
            return 0;
        }
        if (ability->getGrantedDebuff() == nullptr || ability->getGrantedDebuff()->getOnDotTickDamage() == nullptr) {
            return 0;
        }
        float totalDuration = DamageSimulation::getDotDuration(ability, PC);
        if (totalDuration > 0.0f) {
            float oneTickDamage = ability->getGrantedDebuff()->getOnDotTickDamage()(PC, PC, ability->getRank(), 1, totalDuration); 
            float tickPeriod = ability->getGrantedDebuff()->getOnCalculateDotTickPeriod()(PC);
            return DamageSimulation::Round(totalDuration*oneTickDamage/tickPeriod);
        }
        return 0;
    }
    static int32_t getDamageForAbility(Ability *ability, PlayerCharacter *PC) {
        float damage = ability->getDamage(PC);
        damage *= PC->calculateGlobalDamageBonus();
        return DamageSimulation::Round(damage);
    }
    static std::string regexReplaceTooltipDirectDamage(std::string tooltipText, Ability *ability, PlayerCharacter *PC) {
        std::stringstream ssDmg;
        bool avg = PC->getAlwaysUseAverageDamage();
        PC->setAlwaysUseAverageDamage(true);
        ssDmg<<DamageSimulation::getDamageForAbility(ability, PC);
        PC->setAlwaysUseAverageDamage(avg);
        return std::regex_replace(tooltipText, std::regex("<dmg>"), ssDmg.str());
    }
    static std::string regexReplaceTooltipDotDamage(std::string tooltipText, Ability *ability, PlayerCharacter *PC) {
        std::stringstream ssDmg;
        float duration = DamageSimulation::getDotDuration(ability, PC);
        ssDmg<<DamageSimulation::totalDotDamageForAbility(ability, PC);
        return std::regex_replace(tooltipText, std::regex("<dmg>"), ssDmg.str());
    }
    static std::string regexReplaceTooltipDotDuration(std::string tooltipText, Ability *ability, PlayerCharacter *PC) {
        std::stringstream ssDuration;
        float duration = DamageSimulation::getDotDuration(ability, PC);
        ssDuration<<duration;
        return std::regex_replace(tooltipText, std::regex("<time>"), ssDuration.str());
    }
    static int32_t getDotDuration(Ability *ability, PlayerCharacter *PC) {
        if (ability->getGrantedDebuff()->getOnCalculateDuration() == nullptr) {
            return 0;
        }
        float duration = ability->getGrantedDebuff()->getOnCalculateDuration()(PC, ability->getRank());
        return duration;
    }
    std::vector<Enemy *>& getEnemyList();
    GlobalAbilityList *getGlobalAbilityList() const;
    void setGlobalAbilityList(GlobalAbilityList *value);
    static std::mt19937& getRandEngine();
    static float randomFloatBetween(float a, float b);
    static int32_t randomIntBetween(int32_t a, int32_t b);
    float getTime() const;
};

#endif // DAMAGESIMULATION_H
