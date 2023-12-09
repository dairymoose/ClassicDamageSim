#include "PriorityAction.h"
#include "PlayerCharacter.h"
#include "DamageSimulation.h"
#include "Ability.h"
#include "GlobalAbilityList.h"

Ability *PriorityAction::getAbility() const
{
    return ability;
}

void PriorityAction::setAbility(Ability *value)
{
    ability = value;
}

std::function<bool (PlayerCharacter *PC, float timestamp)> PriorityAction::getPredicate() const
{
    return predicate;
}

void PriorityAction::setPredicate(const std::function<bool (PlayerCharacter *PC, float timestamp)> &value)
{
    predicate = value;
}

void PriorityAction::execute(PlayerCharacter *PC, std::vector<Enemy *> &enemyList, float timestamp, bool shouldTriggerCooldown)
{
    if (this->getAbility() != nullptr) {
        this->getAbility()->execute(PC, enemyList, timestamp, this->getIgnoreGcd(), this->getIgnoreResourceCost(), shouldTriggerCooldown);
        if (!PC->isWhiteAttack(this->getAbility())) {
            PC->setLastUsedAction(this);
        }
    }
}

bool PriorityAction::getIgnoreGcd() const
{
    return ignoreGcd;
}

void PriorityAction::setIgnoreGcd(bool value)
{
    ignoreGcd = value;
}

bool PriorityAction::getIgnoreResourceCost() const
{
    return ignoreResourceCost;
}

void PriorityAction::setIgnoreResourceCost(bool value)
{
    ignoreResourceCost = value;
}

std::string PriorityAction::getPredicateText() const
{
    return predicateText;
}

void PriorityAction::setPredicateText(const std::string &value)
{
    predicateText = value;
}

std::string PriorityAction::getNameOverride() const
{
    return nameOverride;
}

void PriorityAction::setNameOverride(const std::string &value)
{
    nameOverride = value;
}

std::string PriorityAction::getInternalName() const
{
    return internalName;
}

void PriorityAction::setInternalName(const std::string &value)
{
    internalName = value;
}

bool PriorityAction::getDisabled() const
{
    return disabled;
}

void PriorityAction::setDisabled(bool value)
{
    disabled = value;
}

bool PriorityAction::isMainhandAutoAttack()
{
    if (this->getInternalName() == "mainhand_auto") {
        return true;
    }
    return false;
}

bool PriorityAction::isOffhandAutoAttack()
{
    if (this->getInternalName() == "offhand_auto") {
        return true;
    }
    return false;
}

bool PriorityAction::isAnyAutoAttack()
{
    return this->isMainhandAutoAttack() || this->isOffhandAutoAttack();
}

bool PriorityAction::getSkipToNextActionIfUseConditionFails() const
{
    return skipToNextActionIfUseConditionFails;
}

void PriorityAction::setSkipToNextActionIfUseConditionFails(bool value)
{
    skipToNextActionIfUseConditionFails = value;
}

PriorityAction::PriorityAction(Ability *ability, int32_t rank)
{
    this->ability = ability;
    if (this->ability != nullptr) {
        this->ability->setRank(rank);
    }
}
