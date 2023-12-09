#include "Ability.h"
#include "PlayerCharacter.h"
#include "DamageSimulation.h"
#include "GlobalAbilityList.h"
#include <cfloat>

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

std::function<bool (PlayerCharacter *PC, int32_t abilityRank, float timestamp)> Ability::getCanUseFunction() const
{
    return canUseFunction;
}

void Ability::setCanUseFunction(const std::function<bool (PlayerCharacter *PC, int32_t abilityRank, float timestamp)> &value)
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

std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t initialCost)> Ability::getOnGetResourceCostModifier() const
{
    return onGetResourceCostModifier;
}

void Ability::setOnGetResourceCostModifier(const std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t initialCost)> &value)
{
    onGetResourceCostModifier = value;
}

bool Ability::getCannotBeBlockedOrDodgedOrParried() const
{
    return cannotBeBlockedOrDodgedOrParried;
}

void Ability::setCannotBeBlockedOrDodgedOrParried(bool value)
{
    cannotBeBlockedOrDodgedOrParried = value;
}

Ability::Ability(std::string name)
{
    this->name = name;
}

int32_t Ability::convertToPercentFormat(float f)
{
    return (int32_t)((FLOATING_POINT_SMALL_NUMBER + f)*100);
}

void Ability::printMeleeAttackTable(MeleeAttackTable &mat, std::ostream &stream)
{
    stream<<"Miss: "<<convertToPercentFormat(mat.miss)<<"%<br>";
    stream<<"Dodge: "<<convertToPercentFormat(mat.dodge)<<"%<br>";
    stream<<"Parry: "<<convertToPercentFormat(mat.parry)<<"%<br>";
    stream<<"Glancing Blow: "<<convertToPercentFormat(mat.glancingBlow)<<"%<br>";
    stream<<"Block: "<<convertToPercentFormat(mat.block)<<"%<br>";
    stream<<"Critical Hit: "<<convertToPercentFormat(mat.criticalHit)<<"%<br>";
    stream<<"Crushing Blow: "<<convertToPercentFormat(mat.crushingBlow)<<"%<br>";
    stream<<"Ordinary Hit: "<<convertToPercentFormat(mat.ordinaryHit)<<"%<br>";
    stream<<"Glancing Blow DR: "<<convertToPercentFormat(mat.glancingBlowDamageReduction)<<"%<br>";
}

void Ability::generateMeleeAttackTable(PlayerCharacter *PC, Enemy *target, MeleeAttackTable& mat)
{
    int32_t defenseWeaponSkillDelta = target->getCalculatedDefense() - PC->getCalculatedWeaponSkill();
    
    if (defenseWeaponSkillDelta <= 10) {
        float missBase2h = 0.05f;
        float missBaseDw = 0.024f;
        float baseMissChance = missBase2h;
        if (PC->isWhiteAttack(this) && PC->getOffHandItem() != nullptr) {
            baseMissChance = missBaseDw;
        }
        mat.miss = baseMissChance + (defenseWeaponSkillDelta)*0.001f;
    }
    else {
        float missBase2h = 0.06f;
        float missBaseDw = 0.025f;
        float baseMissChance = missBase2h;
        if (PC->isWhiteAttack(this) && PC->getOffHandItem() != nullptr) {
            baseMissChance = missBaseDw;
        }
        mat.miss = baseMissChance + (defenseWeaponSkillDelta - 10)*0.004f;
    }
    mat.miss = std::max(mat.miss, 0.0f);
    
    mat.dodge = 0.045f;
    mat.parry = 0.0f;
    if (PC->isWhiteAttack(this))
        mat.glancingBlow = 0.10f + 0.02f*(defenseWeaponSkillDelta);
    else
        mat.glancingBlow = 0.0f;
    if (mat.glancingBlow < 0.0f)
        mat.glancingBlow = 0.0f;
    mat.glancingBlowDamageReduction = std::max(0.0f, -0.15f + 0.03f*(defenseWeaponSkillDelta));
    
    mat.block = 0.0f;
    mat.criticalHit = PC->getCritChance();
    mat.crushingBlow = 0.0f;
    
    if (this->cannotBeBlockedOrDodgedOrParried) {
        mat.block = 0.0f;
        mat.dodge = 0.0f;
        mat.parry = 0.0f;
    }
    if (this == GlobalAbilityList::activeList->Overpower) {
        int32_t op = PC->getTalentRank("Improved Overpower");
        if (op > 0) {
            mat.criticalHit += op * 0.25f;
        }
    }
    mat.ordinaryHit = 1.0f - (mat.miss + mat.dodge + mat.parry + mat.glancingBlow + mat.block + mat.criticalHit + mat.crushingBlow);
}

MeleeHitResult Ability::rollAttackTableForHitResult(MeleeAttackTable &mat)
{
    float attackTableDiceRanges[8];
    attackTableDiceRanges[0] = mat.miss; //8
    attackTableDiceRanges[1] = attackTableDiceRanges[0] + mat.dodge; //12.5
    attackTableDiceRanges[2] = attackTableDiceRanges[1] + mat.parry; //12.5
    attackTableDiceRanges[3] = attackTableDiceRanges[2] + mat.glancingBlow; //52.5
    attackTableDiceRanges[4] = attackTableDiceRanges[3] + mat.block;
    attackTableDiceRanges[5] = attackTableDiceRanges[4] + mat.criticalHit;
    attackTableDiceRanges[6] = attackTableDiceRanges[5] + mat.crushingBlow;
    attackTableDiceRanges[7] = attackTableDiceRanges[6] + mat.ordinaryHit; //100.0
    MeleeHitResult results[8];
    results[0] = MeleeHitResult::Miss;
    results[1] = MeleeHitResult::Dodge;
    results[2] = MeleeHitResult::Parry;
    results[3] = MeleeHitResult::GlancingBlow;
    results[4] = MeleeHitResult::Block;
    results[5] = MeleeHitResult::CriticalHit;
    results[6] = MeleeHitResult::CrushingBlow;
    results[7] = MeleeHitResult::OrdinaryHit;
    float dieRoll = DamageSimulation::randomFloatBetween(0.0f, 1.0f);
    //std::cout<<"Miss = "<<attackTableDiceRanges[0]<<'\n';
    for (int i=0; i<8; ++i) {
        if (dieRoll < attackTableDiceRanges[i]) {
            //std::cout<<"Got meleeHitResult of "<<(int)(results[i])<<" from die roll of "<<dieRoll<<" with upper range of "<<attackTableDiceRanges[i]<<'\n';
            return results[i];
        }
    }
}

void Ability::triggerAbilityDamageMeleeHooks(PlayerCharacter *PC, float timestamp)
{
    for (int i=0; i<PC->getBuffs().size(); ++i) {
        if (PC->getBuffs()[i]->getBuff()->getOnAbilityDamageMelee() != nullptr) {
            PC->getBuffs()[i]->getBuff()->getOnAbilityDamageMelee()(PC, timestamp);
        }
    }
}

void Ability::execute(PlayerCharacter *PC, std::vector<Enemy *> &enemyList, float timestamp, bool ignoreGcd, bool ignoreResourceCost, bool shouldTriggerCooldown)
{
    this->triggerResourceCost(PC, timestamp);
    int32_t damage = DamageSimulation::getDamageForAbility(this, PC);
    bool isCritical = false;
    
    MeleeAttackTable mat;
    this->generateMeleeAttackTable(PC, PC->getTarget(), mat);
    MeleeHitResult mhr = this->rollAttackTableForHitResult(mat);
    if (mhr == MeleeHitResult::CriticalHit) {
        isCritical = true;
        damage = PC->applyCritDamage(this, damage);
    }
    
    int32_t defenseWeaponSkillDelta = PC->getTarget()->getCalculatedDefense() - PC->getCalculatedWeaponSkill();
    if (mhr == MeleeHitResult::GlancingBlow) {
        damage *= 1.0f - mat.glancingBlowDamageReduction;
    }
    
    if (PC->isWhiteAttack(this) && this == GlobalAbilityList::activeList->MeleeMainhandAutoAttack) {
        if (PC->hasBuff(GlobalAbilityList::activeList->Enrage)) {
            damage *= 1.20f;
        }
    }
    bool didNotConnect = false;
    if (mhr == MeleeHitResult::Miss || mhr == MeleeHitResult::Dodge || mhr == MeleeHitResult::Parry) {
        didNotConnect = true;
        damage = 0.0f;
    }
    if (mhr == MeleeHitResult::Dodge) {
        PC->getTarget()->setLastDodgeTimestamp(timestamp);
    }
    
    if (shouldTriggerCooldown)
        this->triggerCooldown(PC, timestamp, ignoreGcd);
    int32_t resourceBefore = PC->getResource();
    int32_t damageDone = 0;
    
    if (this->isDamageAbility()) {
        if (this->getAoeMaxTargets() > 0) {
            for (int i=0; i<this->getAoeMaxTargets(); ++i) {
                if (i >= enemyList.size()) {
                    break;
                }
                if (enemyList[i] != nullptr) {
                    damageDone += enemyList[i]->applyDamage(PC, damage, mhr, timestamp, this);
                }
            }
        }
        else {
            if (PC->getTarget() == nullptr && enemyList.size() > 0) {
                PC->setTarget(enemyList[0]);
            }
            if (PC->getTarget() != nullptr) {
                damageDone += PC->getTarget()->applyDamage(PC, damage, mhr, timestamp, this);
            }
        }
    }
    
    if (!didNotConnect) {
        if (this->getGrantedBuff() != nullptr) {
            bool isFree = ignoreResourceCost && ignoreGcd;
            PC->applyBuff(PC, timestamp, this->getGrantedBuff(), isFree);
        }
        if (this->getGrantedDebuff() != nullptr) {
            PC->getTarget()->applyDebuff(PC, timestamp, this->getGrantedDebuff());
        }
        this->triggerResourceGeneration(PC, damageDone, isCritical, timestamp);
        int32_t resourceAfter = PC->getResource();
        if (resourceAfter > resourceBefore || this->getResourceGenerationFunction() != nullptr) {
            if (PC->hasRune("Consumed by Rage")) {
                if (resourceBefore < 80 && resourceAfter >= 80) {
                    PC->applyBuff(PC, timestamp, GlobalAbilityList::activeList->Enrage, false);
                }
            }
            int32_t resourceDiff = resourceAfter - resourceBefore;
            COMBAT_LOG(timestamp, PC, "Rage is at "<<PC->getResource()<<" (gained "<<resourceDiff<<" rage)");
        }
        if (PC->isWhiteAttack(this) && this == GlobalAbilityList::activeList->MeleeMainhandAutoAttack) {
            if (!GlobalAbilityList::activeList->inExtraAttack) {
                for (int i=0; i<PC->getBuffs().size(); ++i) {
                    if (PC->getBuffs()[i]->getBuff()->getOnAutoAttack() != nullptr) {
                        PC->getBuffs()[i]->getBuff()->getOnAutoAttack()(PC);
                    }
                }
            }
        }
        if (damage > 0) {
            if (isCritical) {
                if (PC->getTalentRank("Deep Wounds") > 0) {
                    if (PC->getTarget() != nullptr) {
                        PC->getTarget()->applyDebuff(PC, timestamp, GlobalAbilityList::activeList->DeepWounds);
                    }
                }
            }
            if (!GlobalAbilityList::activeList->inExtraAttack) {
                this->triggerAbilityDamageMeleeHooks(PC, timestamp);
            }
        }
    }
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
