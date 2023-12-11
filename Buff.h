#ifndef BUFF_H
#define BUFF_H

#include <functional>
#include "AbilityDamageType.h"
class PlayerCharacter;
class Ability;
class Combatant;

class Buff
{
    std::string name;
    Ability *parent = nullptr;
    std::function<int32_t(Combatant *Cbt, int32_t rank, int32_t attackPowerIn)> onCalculateAttackPower = nullptr;
    std::function<int32_t (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> onDotTickDamage = nullptr;
    std::function<void (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> onBuffTick = nullptr;
    std::function<int32_t (Combatant *Cbt)> onCalculateDotTickPeriod = [](Combatant *Cbt){return 3;};
    std::function<float (Combatant *Cbt, int32_t rank)> onCalculateDuration = nullptr;
    std::function<float (Combatant *Cbt)> onGetMeleeAutoAttackMultiplier = nullptr;
    std::function<void (Combatant *Cbt)> onAutoAttack = nullptr;
    std::function<void (Combatant *Cbt, float timestamp)> onAbilityDamageMelee = nullptr;
    
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> onCalculateStrengthBoost = nullptr;
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> onCalculateAllStatsBoost = nullptr;
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> onCalculateArmorBoost = nullptr;
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> onCalculatePercentStatBoost = nullptr;
    
    AbilityDamageType abilityDamageType = AbilityDamageType::Other;
    bool ignoresArmor = false;
public:
    Buff(std::string name, Ability *parent);
    std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t attackPowerIn)> getOnCalculateAttackPower() const;
    void setOnCalculateAttackPower(const std::function<int32_t (Combatant *Cbt, int32_t, int32_t)> &value);
    std::function<int32_t (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> getOnDotTickDamage() const;
    void setOnDotTickDamage(const std::function<int32_t (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> &value);
    std::function<int32_t (Combatant *Cbt)> getOnCalculateDotTickPeriod() const;
    void setOnCalculateDotTickPeriod(const std::function<int32_t (Combatant *Cbt)> &value);
    std::string getName() const;
    void setName(const std::string &value);
    std::function<float (Combatant *Cbt, int32_t rank)> getOnCalculateDuration() const;
    void setOnCalculateDuration(const std::function<float (Combatant *Cbt, int32_t rank)> &value);
    Ability *getParent() const;
    void setParent(Ability *value);
    std::function<void (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> getOnBuffTick() const;
    void setOnBuffTick(const std::function<void (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> &value);
    std::function<float (Combatant *Cbt)> getOnGetMeleeAutoAttackMultiplier() const;
    void setOnGetMeleeAutoAttackMultiplier(const std::function<float (Combatant *Cbt)> &value);
    std::function<void (Combatant *Cbt)> getOnAutoAttack() const;
    void setOnAutoAttack(const std::function<void (Combatant *Cbt)> &value);
    std::function<void (Combatant *Cbt, float timestamp)> getOnAbilityDamageMelee() const;
    void setOnAbilityDamageMelee(const std::function<void (Combatant *Cbt, float timestamp)> &value);
    AbilityDamageType getAbilityDamageType() const;
    void setAbilityDamageType(const AbilityDamageType &value);
    bool getIgnoresArmor() const;
    void setIgnoresArmor(bool value);
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> getOnCalculateStrengthBoost() const;
    void setOnCalculateStrengthBoost(const std::function<int32_t (PlayerCharacter *PC, int32_t rank)> &value);
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> getOnCalculateAllStatsBoost() const;
    void setOnCalculateAllStatsBoost(const std::function<int32_t (PlayerCharacter *PC, int32_t rank)> &value);
    std::function<int32_t (PlayerCharacter *PC, int32_t rank)> getOnCalculateArmorBoost() const;
    void setOnCalculateArmorBoost(const std::function<int32_t (PlayerCharacter *PC, int32_t rank)> &value);
    std::function<float (PlayerCharacter *PC, int32_t rank)> getOnCalculatePercentStatBoost() const;
    void setOnCalculatePercentStatBoost(const std::function<float (PlayerCharacter *PC, int32_t rank)> &value);
};

#endif // BUFF_H
