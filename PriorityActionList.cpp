#include "PriorityActionList.h"
#include "PlayerCharacter.h"

std::vector<PriorityAction *>& PriorityActionList::getPriorityActions()
{
    return priorityActions;
}

PriorityActionList::PriorityActionList()
{
    
}

PriorityAction *PriorityActionList::getNextAction(PlayerCharacter *PC, float timestamp)
{
    if (PC->getIsCasting()) {
        return nullptr;
    }
    
    FailureReason failureReason;
    for (int i=0; i<this->priorityActions.size(); ++i) {
        bool shouldCheckAbility = true;
        if (this->priorityActions[i]->getPredicate() != nullptr) {
            shouldCheckAbility = this->priorityActions[i]->getPredicate()(PC, timestamp);
            if (!shouldCheckAbility) {
                failureReason = FailureReason::Predicate;
            }
        }
        if (shouldCheckAbility) {
            if (this->priorityActions[i]->getAbility() != nullptr) {
                bool meetsGcdCondition = 
                        this->priorityActions[i]->getIgnoreGcd() ||
                        !this->priorityActions[i]->getAbility()->getIsGcdAbility() ||
                        !PC->getIsGcdActive(timestamp);
                if (meetsGcdCondition) {
                    bool cdCondition = !this->priorityActions[i]->getAbility()->isOnCooldown(PC, timestamp);
                    bool resourceCondition = 
                                this->priorityActions[i]->getIgnoreResourceCost() ||
                                this->priorityActions[i]->getAbility()->hasEnoughResource(PC, timestamp);
                    bool meetsCdAndResourceCondition = 
                            cdCondition && resourceCondition;
                    if (meetsCdAndResourceCondition) {
                        if (this->priorityActions[i]->getAbility()->getCanUseFunction() == nullptr || 
                                this->priorityActions[i]->getAbility()->getCanUseFunction()(PC, this->priorityActions[i]->getAbility()->getRank())) {
                            failureReason = FailureReason::Success;
                            return this->priorityActions[i];
                        } else {
                            failureReason = FailureReason::CanUseCondition;
                        }
                    }
                    else {
                        if (!cdCondition)
                            failureReason = FailureReason::Cooldown;
                        else if (!resourceCondition)
                            failureReason = FailureReason::OutOfResource;
                    }
                }
                else {
                    failureReason = FailureReason::GCD;
                }
            }
        }
        
        if (failureReason == FailureReason::OutOfResource) {
            //Do not try lower priority skills if we were out of resource
            break;
        }
    }
    return nullptr;
}

PriorityAction *PriorityActionList::getActionFromAbilityName(std::string name)
{
    for (int i=0; i<this->priorityActions.size(); ++i) {
        if (this->priorityActions[i]->getAbility() != nullptr && this->priorityActions[i]->getAbility()->getName() == name) {
            return this->priorityActions[i];
        }
    }
    return nullptr;
}

PriorityAction *PriorityActionList::getActionFromInternalName(std::string name)
{
    for (int i=0; i<this->priorityActions.size(); ++i) {
        if (this->priorityActions[i]->getInternalName() == name) {
            return this->priorityActions[i];
        }
    }
    return nullptr;
}

PriorityAction *PriorityActionList::addNewAction(PriorityAction *action)
{
    if (action != nullptr) {
        this->priorityActions.push_back(action);
    }
    return action;
}

bool PriorityActionList::removeExistingAction(PriorityAction *action)
{
    if (action == nullptr)
        return false;
    for (auto&& it=this->priorityActions.begin(); it!=this->priorityActions.end(); ++it) {
        if (*it == action) {
            this->priorityActions.erase(it);
            return true;
        }
    }
    return false;
}

void PriorityActionList::resetAllAbilities()
{
    for (int i=0; i<this->priorityActions.size(); ++i) {
        if (this->priorityActions[i]->getAbility() != nullptr) {
            this->priorityActions[i]->getAbility()->reset();
        }
    }
}
