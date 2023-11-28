#ifndef PRIORITYACTION_H
#define PRIORITYACTION_H

#include <vector>
#include "Enemy.h"
#include "Ability.h"
class PlayerCharacter;

#define STR(x) #x
#define SET_PREDICATE_WITH_TEXT(PA, predicate) PA->setPredicate(predicate); PA->setPredicateText(STR(predicate));

class PriorityAction
{
    Ability *ability = nullptr;
    std::function<bool (PlayerCharacter *PC, float timestamp)> predicate = nullptr;
    bool ignoreGcd = false;
    bool ignoreResourceCost = false;
    std::string predicateText = "";
    std::string nameOverride = "";
    std::string internalName = "";
    bool disabled = false;
public:
    PriorityAction(Ability *ability, int32_t rank=1);
    Ability *getAbility() const;
    void setAbility(Ability *value);
    std::function<bool (PlayerCharacter *PC, float timestamp)> getPredicate() const;
    void setPredicate(const std::function<bool (PlayerCharacter *PC, float timestamp)> &value);
    
    void execute(PlayerCharacter *PC, std::vector<Enemy *>& enemyList, float timestamp);
    bool getIgnoreGcd() const;
    void setIgnoreGcd(bool value);
    bool getIgnoreResourceCost() const;
    void setIgnoreResourceCost(bool value);
    std::string getPredicateText() const;
    void setPredicateText(const std::string &value);
    std::string getNameOverride() const;
    void setNameOverride(const std::string &value);
    bool hasNameOverride() {
        return this->nameOverride.length() > 0;
    }
    std::string getInternalName() const;
    void setInternalName(const std::string &value);
    bool getDisabled() const;
    void setDisabled(bool value);
};

#endif // PRIORITYACTION_H
