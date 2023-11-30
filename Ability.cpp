#include "Ability.h"
#include "PlayerCharacter.h"
#include "DamageSimulation.h"

std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank)> Ability::getDamageFunction() const
{
    return damageFunction;
}

void Ability::setDamageFunction(const std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank)> &value)
{
    damageFunction = value;
}

std::function<float (PlayerCharacter *PC, int32_t abilityRank)> Ability::getCooldownFunction() const
{
    return cooldownFunction;
}

void Ability::setCooldownFunction(const std::function<float (PlayerCharacter *PC, int32_t abilityRank)> &value)
{
    cooldownFunction = value;
}

bool Ability::getIsGcdAbility() const
{
    return isGcdAbility;
}

void Ability::setIsGcdAbility(bool value)
{
    isGcdAbility = value;
}

Buff *Ability::getGrantedBuff() const
{
    return GrantedBuff;
}

void Ability::setGrantedBuff(Buff *value)
{
    GrantedBuff = value;
}

Buff *Ability::getGrantedDebuff() const
{
    return GrantedDebuff;
}

void Ability::setGrantedDebuff(Buff *value)
{
    GrantedDebuff = value;
}

bool Ability::getIgnoresArmor() const
{
    return ignoresArmor;
}

void Ability::setIgnoresArmor(bool value)
{
    ignoresArmor = value;
}

bool Ability::hasEnoughResource(PlayerCharacter *PC, float timestamp)
{
    return PC->getResource() >= this->getResourceCost();
}

bool Ability::isOnCooldown(PlayerCharacter *PC, float timestamp)
{
    if (this->isCoolingDown) {
        float timeDiff = timestamp - this->lastUsedTimestamp;
        if (DamageSimulation::almostEqual(std::min(timeDiff, this->capturedCooldownValue), this->capturedCooldownValue)) {
            //COMBAT_LOG(timestamp, PC, "Cooldown expired for ability "<<this->getName()<<" after "<<timeDiff<<"s");
            this->isCoolingDown = false;
            return false;
        } else {
            //COMBAT_LOG(timestamp, PC, "Cooldown NOT EXPIRED YET for ability "<<this->getName()<<" after "<<timeDiff<<"s");
            return true;
        }
    }
    return false;
}

void Ability::triggerCooldown(PlayerCharacter *PC, float timestamp, bool actionIgnoresGcd)
{
    this->lastUsedTimestamp = timestamp;
    this->isCoolingDown = true;
    if (!actionIgnoresGcd && this->getIsGcdAbility()) {
        PC->setIsGcdActive(true);
        PC->setGcdStartTime(timestamp);
    }
    if (this->cooldownFunction != nullptr) {
        this->capturedCooldownValue = this->getCooldown(PC);
        if (this->onCooldownTriggeredFunction != nullptr) {
            this->onCooldownTriggeredFunction(timestamp, PC, this->rank);
        }
    }
}

void Ability::triggerResourceCost(PlayerCharacter *PC, float timestamp)
{
    PC->setResource(PC->getResource() - this->getResourceCost());
}

void Ability::triggerResourceGeneration(PlayerCharacter *PC, int32_t damageDone, bool isCritical, float timestamp)
{
    if (this->getResourceGenerationFunction() != nullptr) {
        int32_t generated = this->getResourceGenerationFunction()(PC, this->getRank(), damageDone, isCritical);
        PC->setResource(PC->getResource() + generated);
    }
}

std::string Ability::getName() const
{
    return name;
}

void Ability::setName(const std::string &value)
{
    name = value;
}

std::function<float (PlayerCharacter *PC, int32_t abilityRank)> Ability::getCanUseFunction() const
{
    return canUseFunction;
}

void Ability::setCanUseFunction(const std::function<float (PlayerCharacter *PC, int32_t abilityRank)> &value)
{
    canUseFunction = value;
}

int32_t Ability::getRank() const
{
    return rank;
}

void Ability::setRank(const int32_t &value)
{
    rank = value;
}

std::function<void (float timestamp, PlayerCharacter *PC, int32_t abilityRank)> Ability::getOnCooldownTriggeredFunction() const
{
    return onCooldownTriggeredFunction;
}

void Ability::setOnCooldownTriggeredFunction(const std::function<void (float timestamp, PlayerCharacter *PC, int32_t abilityRank)> &value)
{
    onCooldownTriggeredFunction = value;
}

int32_t Ability::getResourceCost() const
{
    return resourceCost;
}

void Ability::setResourceCost(const int32_t &value)
{
    resourceCost = value;
}

int32_t Ability::getAoeMaxTargets() const
{
    return aoeMaxTargets;
}

void Ability::setAoeMaxTargets(const int32_t &value)
{
    aoeMaxTargets = value;
}

std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t damageDone, bool isCritical)> Ability::getResourceGenerationFunction() const
{
    return resourceGenerationFunction;
}

void Ability::setResourceGenerationFunction(const std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t damageDone, bool isCritical)> &value)
{
    resourceGenerationFunction = value;
}

float Ability::getCastTime() const
{
    return castTime;
}

void Ability::setCastTime(float value)
{
    castTime = value;
}

bool Ability::getCastedAbilityResetsAutoAttack() const
{
    return castedAbilityResetsAutoAttack;
}

void Ability::setCastedAbilityResetsAutoAttack(bool value)
{
    castedAbilityResetsAutoAttack = value;
}

std::vector<int32_t> &Ability::getLearnLevels()
{
    return learnLevels;
}

bool Ability::getReplacesNextMelee() const
{
    return replacesNextMelee;
}

void Ability::setReplacesNextMelee(bool value)
{
    replacesNextMelee = value;
}

std::string Ability::getTooltipText() const
{
    return tooltipText;
}

void Ability::setTooltipText(const std::string &value)
{
    tooltipText = value;
}

std::function<std::string (std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability)> Ability::getOnGetTooltip() const
{
    return onGetTooltip;
}

void Ability::setOnGetTooltip(const std::function<std::string (std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability)> &value)
{
    onGetTooltip = value;
}

Ability::Ability(std::string name)
{
    this->name = name;
}

void Ability::reset()
{
    this->lastUsedTimestamp = 0.0f;
    this->isCoolingDown = false;
    this->capturedCooldownValue = 0.0f;
}

float Ability::getRemainingCooldown(float timestamp)
{
    if (!this->isCoolingDown)
        return 0.0f;
    float timeDiff = timestamp - this->lastUsedTimestamp;
    timeDiff += FLOATING_POINT_SMALL_NUMBER;
    return this->capturedCooldownValue - timeDiff;
}

float Ability::getTimeSinceLastUsed(float timestamp)
{
    float timeDiff = timestamp - this->lastUsedTimestamp;
    timeDiff += FLOATING_POINT_SMALL_NUMBER;
    return timeDiff;
}

AbilityDamageType Ability::getAbilityDamageType() const
{
    return abilityDamageType;
}

void Ability::setAbilityDamageType(const AbilityDamageType &value)
{
    abilityDamageType = value;
}

float Ability::getCooldown(PlayerCharacter *PC)
{
    if (this->cooldownFunction != nullptr) {
        return this->cooldownFunction(PC, this->rank);
    }
    return 0.0f;
}

int32_t Ability::getDamage(PlayerCharacter *PC)
{
    if (this->damageFunction != nullptr) {
        return this->damageFunction(PC, this->rank);
    }
    return 0;
}

bool Ability::isDamageAbility()
{
    if (this->damageFunction != nullptr) {
        return true;
    }
    return false;
}
