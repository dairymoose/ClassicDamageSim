#include "PriorityAction.h"
#include "PlayerCharacter.h"

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

void PriorityAction::execute(PlayerCharacter *PC, std::vector<Enemy *> &enemyList, float timestamp)
{
    if (this->getAbility() != nullptr) {
        this->getAbility()->triggerResourceCost(PC, timestamp);
        float damage = this->getAbility()->getDamage(PC);
        damage *= PC->calculateGlobalDamageBonus();
        
        this->getAbility()->triggerCooldown(PC, timestamp, this->getIgnoreGcd());
        int32_t resourceBefore = PC->getResource();
        int32_t damageDone = 0;
        if (this->getAbility()->isDamageAbility()) {
            if (this->getAbility()->getAoeMaxTargets() > 0) {
                for (int i=0; i<this->getAbility()->getAoeMaxTargets(); ++i) {
                    if (i >= enemyList.size()) {
                        break;
                    }
                    damageDone += enemyList[i]->applyDamage(PC, damage, timestamp, this->getAbility());
                }
            }
            else {
                damageDone += PC->getTarget()->applyDamage(PC, damage, timestamp, this->getAbility());
            }
        }
        this->getAbility()->triggerResourceGeneration(PC, damageDone, false, timestamp);
        int32_t resourceAfter = PC->getResource();
        if (resourceAfter > resourceBefore || this->getAbility()->getResourceGenerationFunction() != nullptr) {
            int32_t resourceDiff = resourceAfter - resourceBefore;
            COMBAT_LOG(timestamp, PC, "Rage is at "<<PC->getResource()<<" (gained "<<resourceDiff<<" rage)");
        }
        if (this->getAbility()->getGrantedBuff() != nullptr) {
            bool isFree = this->getIgnoreResourceCost() && this->getIgnoreGcd();
            PC->applyBuff(PC, timestamp, this->getAbility()->getGrantedBuff(), isFree);
        }
        if (this->getAbility()->getGrantedDebuff() != nullptr) {
            PC->getTarget()->applyDebuff(PC, timestamp, this->getAbility()->getGrantedDebuff());
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

PriorityAction::PriorityAction(Ability *ability, int32_t rank)
{
    this->ability = ability;
    this->ability->setRank(rank);
}
