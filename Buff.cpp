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

Buff::Buff(std::string name, Ability *parent)
{
    this->name = name;
    this->parent = parent;
}

std::function<int32_t (int32_t rank, int32_t attackPowerIn)> Buff::getOnCalculateAttackPower() const
{
    return onCalculateAttackPower;
}

void Buff::setOnCalculateAttackPower(const std::function<int32_t (int32_t rank, int32_t attackPowerIn)> &value)
{
    onCalculateAttackPower = value;
}

std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t tickNumber)> Buff::getOnDotTickDamage() const
{
    return onDotTickDamage;
}

void Buff::setOnDotTickDamage(const std::function<int32_t (Combatant *Cbt, int32_t, int32_t)> &value)
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
