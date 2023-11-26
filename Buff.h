#ifndef BUFF_H
#define BUFF_H

#include <functional>
class PlayerCharacter;
class Ability;
class Combatant;

class Buff
{
    std::string name;
    Ability *parent = nullptr;
    std::function<int32_t(int32_t rank, int32_t attackPowerIn)> onCalculateAttackPower = nullptr;
    std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t tickNumber)> onDotTickDamage = nullptr;
    std::function<int32_t (Combatant *Cbt)> onCalculateDotTickPeriod = [](Combatant *Cbt){return 3;};
    std::function<float (Combatant *Cbt, int32_t rank)> onCalculateDuration = nullptr;
public:
    Buff(std::string name, Ability *parent);
    std::function<int32_t (int32_t rank, int32_t attackPowerIn)> getOnCalculateAttackPower() const;
    void setOnCalculateAttackPower(const std::function<int32_t (int32_t rank, int32_t attackPowerIn)> &value);
    std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t tickNumber)> getOnDotTickDamage() const;
    void setOnDotTickDamage(const std::function<int32_t (Combatant *Cbt, int32_t rank, int32_t tickNumber)> &value);
    std::function<int32_t (Combatant *Cbt)> getOnCalculateDotTickPeriod() const;
    void setOnCalculateDotTickPeriod(const std::function<int32_t (Combatant *Cbt)> &value);
    std::string getName() const;
    void setName(const std::string &value);
    std::function<float (Combatant *Cbt, int32_t rank)> getOnCalculateDuration() const;
    void setOnCalculateDuration(const std::function<float (Combatant *Cbt, int32_t rank)> &value);
    Ability *getParent() const;
    void setParent(Ability *value);
};

#endif // BUFF_H
