#ifndef COMBATANT_H
#define COMBATANT_H

#include <cstdint>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include "Buff.h"
#include "AppliedBuff.h"
#include "CombatLog.h"
#include "MeleeHitResult.h"

#define COMBAT_LOG(timestamp, attacker, text) ({if(attacker->getCombatLog() != nullptr && attacker->getCombatLog()->getEnabled()){std::stringstream ss;ss<<FONT_BLUE<<"["<<std::fixed<<std::setprecision(1)<<timestamp<<"]"<<END_FONT<<" "<<text;attacker->getCombatLog()->getLogText().push_back(ss.str());}})
class Ability;
class PriorityAction;

struct TrackedDamageInfo {
    int64_t damage = 0;
    int64_t nonCritDamage = 0;
    int32_t nonCritCount = 0;
    int64_t critOnlyDamage = 0;
    int32_t critCount = 0;
    int32_t count = 0;
    std::string event = "";
    Ability *ability = nullptr;
    Buff *buff = nullptr;
    
    float mostRecentBuffTimestamp = 0.0f;
    float mostRecentBuffDuration = 0.0f;
    float buffUptime = 0.0f;
};

class Combatant
{
protected:
    std::string name = "";
    
    int32_t level;
    int32_t resource;
    int32_t resourceMax;
    
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
    
    float lastDodgeTimestamp = -1.0f;
    
    std::unordered_map<Ability *, TrackedDamageInfo> damageDoneByAbility;
    std::unordered_map<Buff *, TrackedDamageInfo> damageDoneByBuff;
    
    void addNewAbilityToMap(Combatant *attacker, Ability *toAdd, MeleeHitResult mhr);
    void addNewBuffToMap(Combatant *attacker, Buff *toAdd, MeleeHitResult mhr);
    
    template<typename T>
    void combatLogInternal(std::string damageTypeText, Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, T *abilitySource) {
        std::string abilityName = abilitySource->getName();
        if (mhr == MeleeHitResult::CriticalHit || mhr == MeleeHitResult::OrdinaryHit)
            COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" did "<<DAMAGE_FONT_COLOR<<damage<<END_FONT<<(mhr == MeleeHitResult::CriticalHit ? " *CRITICAL*" : "")<<" "<<damageTypeText<<" damage to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" from "<<ABILITY_FONT_COLOR<<abilityName<<END_FONT);
        else if (mhr == MeleeHitResult::GlancingBlow)
            COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" did "<<DAMAGE_FONT_COLOR<<damage<<END_FONT<<" glancing "<<damageTypeText<<" damage to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" from "<<ABILITY_FONT_COLOR<<abilityName<<END_FONT);
        else if (mhr == MeleeHitResult::Miss)
            COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" missed "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" with "<<ABILITY_FONT_COLOR<<abilityName<<END_FONT);
        else if (mhr == MeleeHitResult::Dodge)
            COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" was dodged by "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" with "<<ABILITY_FONT_COLOR<<abilityName<<END_FONT);
    }
    int32_t applyDamageInternal(std::string damageTypeText, Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource);
    int32_t applyDamageInternal(std::string damageTypeText, Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource);
    void adjustUptimeForRemovedBuff(Combatant *attacker, Buff *buff, float timestamp);
public:
    Combatant();
    virtual ~Combatant();
    float getPhysicalDamageReduction() const;
    void setPhysicalDamageReduction(float value);
    float getMagicalDamageReduction() const;
    void setMagicalDamageReduction(float value);
    
    virtual int32_t getCalculatedDefense();
    virtual int32_t getCalculatedWeaponSkill();
    
    bool isSpellcastFinished(float timestamp);
    void triggerSpellcastFinished();
    void applyDotDamage(PlayerCharacter *PC, float timestamp);
    void tickBuffs(PlayerCharacter *PC, float timestamp);
    
    int32_t applyDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource);
    int32_t applyPhysicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource);
    int32_t applyMagicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource);
    int32_t applyArmorIgnoreDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource);
    
    int32_t applyDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource);
    int32_t applyPhysicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource);
    int32_t applyMagicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource);
    int32_t applyArmorIgnoreDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource);
    
    AppliedBuff *applyBuff(Combatant *attacker, float timestamp, Buff *buff, bool isFree);
    void applyDebuff(Combatant *attacker, float timestamp, Buff *debuff);
    bool isDead();
    bool hasBuff(Buff *Buff);
    bool hasDebuff(Buff *Debuff);
    bool removeBuffByType(Buff *toRemove, float timestamp);
    bool removeDebuffByType(Buff *toRemove, float timestamp);
    bool removeBuff(AppliedBuff *toRemove, float timestamp);
    bool removeDebuff(AppliedBuff *toRemove, float timestamp);
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
    std::vector<AppliedBuff *>& getBuffs();
    std::vector<AppliedBuff *>& getDebuffs();
    float getGcdDuration() const;
    void setGcdDuration(float value);
    std::unordered_map<Ability *, TrackedDamageInfo>& getDamageDoneByAbility();
    std::unordered_map<Buff *, TrackedDamageInfo>& getDamageDoneByBuff();
    int32_t getLevel() const;
    void setLevel(const int32_t &value);
    int32_t getResource() const;
    void setResource(const int32_t &value);
    int32_t getResourceMax() const;
    void setResourceMax(const int32_t &value);
    float getLastDodgeTimestamp() const;
    void setLastDodgeTimestamp(float value);
};

#endif // COMBATANT_H
