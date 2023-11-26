#ifndef PRIORITYACTIONLIST_H
#define PRIORITYACTIONLIST_H

#include <vector>
#include "PriorityAction.h"

enum class FailureReason {
    Success,
    Predicate,
    Cooldown,
    GCD,
    OutOfResource,
    CanUseCondition
};


class PriorityActionList
{
    std::vector<PriorityAction *> priorityActions;
public:
    PriorityActionList();
    std::vector<PriorityAction *>& getPriorityActions();
    PriorityAction *getNextAction(PlayerCharacter *PC, float timestamp);
    PriorityAction *getActionFromAbilityName(std::string name);
    void resetAllAbilities();
};

#endif // PRIORITYACTIONLIST_H
