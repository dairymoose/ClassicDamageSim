#include "Buff.h"
#include "DamageSimulation.h"

std::string Buff::getName() const
{
    return name;
}

void Buff::setName(const std::string &value)
{
    name = value;
}

std::function<float (Combatant *Cbt, int32_t rank)> Buff::getOnCalculateDuration() const
{
    return onCalculateDuration;
}

void Buff::setOnCalculateDuration(const std::function<float (Combatant *Cbt, int32_t rank)> &value)
{
    onCalculateDuration = value;
}

Ability *Buff::getParent() const
{
    return parent;
}

void Buff::setParent(Ability *value)
{
    parent = value;
}

std::function<void (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> Buff::getOnBuffTick() const
{
    return onBuffTick;
}

void Buff::setOnBuffTick(const std::function<void (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> &value)
{
    onBuffTick = value;
}

std::function<float (Combatant *Cbt)> Buff::getOnGetMeleeAutoAttackMultiplier() const
{
    return onGetMeleeAutoAttackMultiplier;
}

void Buff::setOnGetMeleeAutoAttackMultiplier(const std::function<float (Combatant *Cbt)> &value)
{
    onGetMeleeAutoAttackMultiplier = value;
}

std::function<void (Combatant *Cbt)> Buff::getOnAutoAttack() const
{
    return onAutoAttack;
}

void Buff::setOnAutoAttack(const std::function<void (Combatant *Cbt)> &value)
{
    onAutoAttack = value;
}

std::function<void (Combatant *Cbt, float timestamp)> Buff::getOnAbilityDamageMelee() const
{
    return onAbilityDamageMelee;
}

void Buff::setOnAbilityDamageMelee(const std::function<void (Combatant *Cbt, float timestamp)> &value)
{
    onAbilityDamageMelee = value;
}

AbilityDamageType Buff::getAbilityDamageType() const
{
    return abilityDamageType;
}

void Buff::setAbilityDamageType(const AbilityDamageType &value)
{
    abilityDamageType = value;
}

bool Buff::getIgnoresArmor() const
{
    return ignoresArmor;
}

void Buff::setIgnoresArmor(bool value)
{
    ignoresArmor = value;
}

std::function<int32_t (PlayerCharacter *PC, int32_t rank)> Buff::getOnCalculateStrengthBoost() const
{
    return onCalculateStrengthBoost;
}

void Buff::setOnCalculateStrengthBoost(const std::function<int32_t (PlayerCharacter *PC, int32_t rank)> &value)
{
    onCalculateStrengthBoost = value;
}

std::function<int32_t (PlayerCharacter *PC, int32_t rank)> Buff::getOnCalculateAllStatsBoost() const
{
    return onCalculateAllStatsBoost;
}

void Buff::setOnCalculateAllStatsBoost(const std::function<int32_t (PlayerCharacter *PC, int32_t rank)> &value)
{
    onCalculateAllStatsBoost = value;
}

std::function<int32_t (PlayerCharacter *PC, int32_t rank)> Buff::getOnCalculateArmorBoost() const
{
    return onCalculateArmorBoost;
}

void Buff::setOnCalculateArmorBoost(const std::function<int32_t (PlayerCharacter *PC, int32_t rank)> &value)
{
    onCalculateArmorBoost = value;
}

std::function<float (PlayerCharacter *PC, int32_t rank)> Buff::getOnCalculatePercentStatBoost() const
{
    return onCalculatePercentStatBoost;
}

void Buff::setOnCalculatePercentStatBoost(const std::function<float (PlayerCharacter *PC, int32_t rank)> &value)
{
    onCalculatePercentStatBoost = value;
}

Buff::Buff(std::string name, Ability *parent)
{
    this->name = name;
    this->parent = parent;
}

std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t attackPowerIn)> Buff::getOnCalculateAttackPower() const
{
    return onCalculateAttackPower;
}

void Buff::setOnCalculateAttackPower(const std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t attackPowerIn)> &value)
{
    onCalculateAttackPower = value;
}

std::function<int32_t (Combatant *Caster, Combatant *Target, int32_t rank, int32_t tickNumber, float buffDuration)> Buff::getOnDotTickDamage() const
{
    return onDotTickDamage;
}

void Buff::setOnDotTickDamage(const std::function<int32_t (Combatant *Caster, Combatant *Target, int32_t, int32_t, float buffDuration)> &value)
{
    onDotTickDamage = value;
}

std::function<int32_t (Combatant *Cbt)> Buff::getOnCalculateDotTickPeriod() const
{
    return onCalculateDotTickPeriod;
}

void Buff::setOnCalculateDotTickPeriod(const std::function<int32_t (Combatant *Cbt)> &value)
{
    onCalculateDotTickPeriod = value;
}
