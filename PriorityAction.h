#ifndef PRIORITYACTION_H
#define PRIORITYACTION_H

#include <vector>
#include "Enemy.h"
#include "Ability.h"
class PlayerCharacter;

class PriorityAction
{
    Ability *ability = nullptr;
    std::function<bool (PlayerCharacter *PC, float timestamp)> predicate = nullptr;
    bool ignoreGcd = false;
    bool ignoreResourceCost = false;
public:
    PriorityAction(Ability *ability);
    Ability *getAbility() const;
    void setAbility(Ability *value);
    std::function<bool (PlayerCharacter *PC, float timestamp)> getPredicate() const;
    void setPredicate(const std::function<bool (PlayerCharacter *PC, float timestamp)> &value);
    
    void execute(PlayerCharacter *PC, std::vector<Enemy *>& enemyList, float timestamp);
    bool getIgnoreGcd() const;
    void setIgnoreGcd(bool value);
    bool getIgnoreResourceCost() const;
    void setIgnoreResourceCost(bool value);
};

#endif // PRIORITYACTION_H
