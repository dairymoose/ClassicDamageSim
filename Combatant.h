#ifndef COMBATANT_H
#define COMBATANT_H

#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Buff.h"
#include "AppliedBuff.h"
#include "CombatLog.h"

#define COMBAT_LOG(timestamp, attacker, text) ({std::stringstream ss;ss<<FONT_BLUE<<"["<<std::fixed<<std::setprecision(1)<<timestamp<<"]"<<END_FONT<<" "<<text;attacker->getCombatLog()->getLogText().push_back(ss.str());})
class Ability;
class PriorityAction;

class Combatant
{
protected:
    std::string name = "";
    
    int32_t currentHp;
    int32_t maxHp;
    
    bool isGcdActive = false;
    float gcdStartTime = 0.0f;
    
    bool isCasting = false;
    Ability *castingAbility = nullptr;
    float castStartTime = 0.0f;
    
    int32_t damageDone = 0;
    
    float gcdDuration = 1.5f;
    
    float physicalDamageReduction = 0.0f;
    float magicalDamageReduction = 0.0f;
    
    CombatLog *combatLog = new CombatLog();
    
    std::vector<AppliedBuff *> Buffs;
    std::vector<AppliedBuff *> Debuffs;
    
    PriorityAction *replaceMeleeAction = nullptr;
    
    int32_t applyDamageInternal(std::string damageTypeText, Combatant *attacker, int32_t damage, bool isCritical, float timestamp, Ability *abilitySource);
public:
    Combatant();
    virtual ~Combatant();
    float getPhysicalDamageReduction() const;
    void setPhysicalDamageReduction(float value);
    float getMagicalDamageReduction() const;
    void setMagicalDamageReduction(float value);
    
    bool isSpellcastFinished(float timestamp);
    void triggerSpellcastFinished();
    void applyDotDamage(PlayerCharacter *PC, float timestamp);
    void tickBuffs(PlayerCharacter *PC, float timestamp);
    int32_t applyDamage(Combatant *attacker, int32_t damage, bool isCritical, float timestamp, Ability *abilitySource);
    int32_t applyPhysicalDamage(Combatant *attacker, int32_t damage, bool isCritical, float timestamp, Ability *abilitySource);
    int32_t applyMagicalDamage(Combatant *attacker, int32_t damage, bool isCritical, float timestamp, Ability *abilitySource);
    int32_t applyArmorIgnoreDamage(Combatant *attacker, int32_t damage, bool isCritical, float timestamp, Ability *abilitySource);
    void applyBuff(Combatant *attacker, float timestamp, Buff *buff, bool isFree);
    void applyDebuff(Combatant *attacker, float timestamp, Buff *debuff);
    bool isDead();
    bool hasBuff(Buff *Buff);
    bool hasDebuff(Buff *Debuff);
    bool removeBuffByType(Buff *toRemove);
    bool removeDebuffByType(Buff *toRemove);
    bool removeBuff(AppliedBuff *toRemove);
    bool removeDebuff(AppliedBuff *toRemove);
    void removeExpiredBuffs(float timestamp);
    void removeExpiredDebuffs(float timestamp);
    void clearAllBuffsAndDebuffsAndFreeMemory();
    std::string getName() const;
    void setName(const std::string &value);
    CombatLog *getCombatLog() const;
    int32_t getCurrentHp() const;
    void setCurrentHp(const int32_t &value);
    int32_t getMaxHp() const;
    void setMaxHp(const int32_t &value);
    void setHp(const int32_t &value);
    int32_t getDamageDone() const;
    void setDamageDone(const int32_t &value);
    bool getIsGcdActive(float timestamp);
    void setIsGcdActive(bool value);
    float getGcdStartTime() const;
    void setGcdStartTime(float value);
    bool getIsCasting() const;
    void setIsCasting(bool value);
    Ability *getCastingAbility() const;
    void setCastingAbility(Ability *value);
    float getCastStartTime() const;
    void setCastStartTime(float value);
    PriorityAction *getReplaceMeleeAction() const;
    void setReplaceMeleeAction(PriorityAction *value);
};

#endif // COMBATANT_H
