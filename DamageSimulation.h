#ifndef DAMAGESIMULATION_H
#define DAMAGESIMULATION_H

#include "PlayerCharacter.h"
#include "PriorityActionList.h"
#include "Enemy.h"
#include <random>

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
    float maxCombatTime = 120.0f;
    int32_t maxActionsPerTimeStep = 20;
public:
    DamageSimulation();
    PlayerCharacter *getPC() const;
    void setPC(PlayerCharacter *value);
    
    void simulate(PriorityActionList *priorityActions);
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
    std::vector<Enemy *>& getEnemyList();
    GlobalAbilityList *getGlobalAbilityList() const;
    void setGlobalAbilityList(GlobalAbilityList *value);
    static std::mt19937& getRandEngine();
};

#endif // DAMAGESIMULATION_H
