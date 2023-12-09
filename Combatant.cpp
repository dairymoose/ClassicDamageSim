#include "Combatant.h"
#include "Ability.h"
#include "DamageSimulation.h"

float Combatant::getPhysicalDamageReduction() const
{
    return physicalDamageReduction;
}

void Combatant::setPhysicalDamageReduction(float value)
{
    physicalDamageReduction = value;
}

float Combatant::getMagicalDamageReduction() const
{
    return magicalDamageReduction;
}

void Combatant::setMagicalDamageReduction(float value)
{
    magicalDamageReduction = value;
}

int32_t Combatant::getCalculatedDefense()
{
    return this->level*5;
}

int32_t Combatant::getCalculatedWeaponSkill()
{
    return this->level*5;
}

bool Combatant::isSpellcastFinished(float timestamp)
{
    if (!this->isCasting)
        return true;
    if (this->castingAbility != nullptr) {
        float timeDiff = timestamp - this->castStartTime;
        timeDiff += FLOATING_POINT_SMALL_NUMBER;
        if (timeDiff >= this->castingAbility->getCastTime()) {
            return true;
        }
        return false;
    }
    return true;
}

void Combatant::triggerSpellcastFinished()
{
    this->isCasting = false;
    this->castingAbility = nullptr;
}

int32_t Combatant::applyPhysicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage * (1.0f - physicalDamageReduction);
        return this->applyDamageInternal("physical", attacker, calcDmg, mhr, timestamp, abilitySource);
    }
    return 0;
}

int32_t Combatant::applyMagicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage * (1.0f - magicalDamageReduction);
        return this->applyDamageInternal("magical", attacker, calcDmg, mhr, timestamp, abilitySource);
    }
    return 0;
}

int32_t Combatant::applyArmorIgnoreDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage;
        return this->applyDamageInternal("armor-ignore", attacker, calcDmg, mhr, timestamp, abilitySource);
    }
    return 0;
}

int32_t Combatant::applyPhysicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage * (1.0f - physicalDamageReduction);
        return this->applyDamageInternal("physical", attacker, calcDmg, mhr, timestamp, buffSource);
    }
    return 0;
}

int32_t Combatant::applyMagicalDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage * (1.0f - magicalDamageReduction);
        return this->applyDamageInternal("magical", attacker, calcDmg, mhr, timestamp, buffSource);
    }
    return 0;
}

int32_t Combatant::applyArmorIgnoreDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage;
        return this->applyDamageInternal("armor-ignore", attacker, calcDmg, mhr, timestamp, buffSource);
    }
    return 0;
}

PriorityAction *Combatant::getReplaceMeleeAction() const
{
    return replaceMeleeAction;
}

void Combatant::setReplaceMeleeAction(PriorityAction *value)
{
    replaceMeleeAction = value;
}

std::vector<AppliedBuff *>& Combatant::getBuffs()
{
    return Buffs;
}

std::vector<AppliedBuff *>& Combatant::getDebuffs()
{
    return Debuffs;
}

float Combatant::getGcdDuration() const
{
    return gcdDuration;
}

void Combatant::setGcdDuration(float value)
{
    gcdDuration = value;
}

std::unordered_map<Ability *, int32_t>& Combatant::getDamageDoneByAbility()
{
    return damageDoneByAbility;
}

std::unordered_map<Buff *, int32_t>& Combatant::getDamageDoneByBuff()
{
    return damageDoneByBuff;
}

int32_t Combatant::getLevel() const
{
    return level;
}

void Combatant::setLevel(const int32_t &value)
{
    level = value;
}

int32_t Combatant::getResource() const
{
    return resource;
}

void Combatant::setResource(const int32_t &value)
{
    resource = value;
    
    if (this->resource > this->resourceMax) {
        this->resource = this->resourceMax;
    }
    if (this->resource < 0) {
        this->resource = 0;
    }
}

int32_t Combatant::getResourceMax() const
{
    return resourceMax;
}

void Combatant::setResourceMax(const int32_t &value)
{
    resourceMax = value;
}

float Combatant::getLastDodgeTimestamp() const
{
    return lastDodgeTimestamp;
}

void Combatant::setLastDodgeTimestamp(float value)
{
    lastDodgeTimestamp = value;
}

int32_t Combatant::applyDamageInternal(std::string damageTypeText, Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        std::string abilityName = abilitySource->getName();
        this->combatLogInternal(damageTypeText, attacker, damage, mhr, timestamp, abilitySource);
        
        int32_t sourceDamage = 0;
        auto&& found = attacker->damageDoneByAbility.find(abilitySource);
        if (found != attacker->damageDoneByAbility.end()) {
            sourceDamage = found->second;
        }
        sourceDamage += damage;
        attacker->damageDoneByAbility[abilitySource] = sourceDamage;
        
        this->currentHp -= damage;
        attacker->setDamageDone(attacker->getDamageDone() + damage);
        return damage;
    }
    return 0;
}

int32_t Combatant::applyDamageInternal(std::string damageTypeText, Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource)
{
    if (attacker != nullptr) {
        std::string abilityName = buffSource->getName();
        this->combatLogInternal(damageTypeText, attacker, damage, mhr, timestamp, buffSource);
        
        int32_t sourceDamage = 0;
        auto&& found = attacker->damageDoneByBuff.find(buffSource);
        if (found != attacker->damageDoneByBuff.end()) {
            sourceDamage = found->second;
        }
        sourceDamage += damage;
        attacker->damageDoneByBuff[buffSource] = sourceDamage;
        
        this->currentHp -= damage;
        attacker->setDamageDone(attacker->getDamageDone() + damage);
        return damage;
    }
    return 0;
}

AppliedBuff *Combatant::applyBuff(Combatant *attacker, float timestamp, Buff *buff, bool isFree)
{
    if (attacker != nullptr) {
        if (buff->getOnCalculateDuration() != nullptr) {
            int32_t buffRank = 0;
            if (buff->getParent() != nullptr) {
                buffRank = buff->getParent()->getRank();
            }
            float duration = buff->getOnCalculateDuration()(this, buffRank);
            if (duration > 0.0f) {
                this->removeBuffByType(buff, timestamp);
                
                COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" applied "<<(isFree ? "FREE " : "")<<"buff "<<FONT_GREEN<<buff->getName()<<END_FONT<<" to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" with duration "<<(int32_t)duration);
                AppliedBuff *AB = new AppliedBuff(buff);
                AB->setAppliedTimestamp(timestamp);
                AB->setLastTickedTimestamp(timestamp);
                AB->setCapturedDuration(duration);
                this->Buffs.push_back(AB);
                return AB;
            }
        }
    }
    return nullptr;
}

void Combatant::applyDebuff(Combatant *attacker, float timestamp, Buff *debuff)
{
    if (attacker != nullptr) {
        if (debuff->getOnCalculateDuration() != nullptr) {
            int32_t rank = 0;
            if (debuff->getParent() != nullptr) {
                rank = debuff->getParent()->getRank();
            }
            float duration = debuff->getOnCalculateDuration()(this, rank);
            if (duration > 0.0f) {
                this->removeDebuffByType(debuff, timestamp);
                        
                COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" applied debuff "<<FONT_RED<<debuff->getName()<<END_FONT<<" to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" with duration "<<(int32_t)duration);
                AppliedBuff *AB = new AppliedBuff(debuff);
                AB->setAppliedTimestamp(timestamp);
                AB->setLastTickedTimestamp(timestamp);
                AB->setCapturedDuration(duration);
                this->Debuffs.push_back(AB);
            }
        }
    }
}

bool Combatant::isDead()
{
    return this->currentHp <= 0;
}

bool Combatant::hasBuff(Buff *Buff)
{
    for (auto buff : this->Buffs) {
        if (buff->getBuff() == Buff)
            return true;
    }
    return false;
}

bool Combatant::hasDebuff(Buff *Debuff)
{
    for (auto debuff : this->Debuffs) {
        if (debuff->getBuff() == Debuff)
            return true;
    }
    return false;
}

bool Combatant::removeBuffByType(Buff *toRemove, float timestamp)
{
    for (auto&& it=this->Buffs.begin(); it!=this->Buffs.end(); ++it) {
        if ((*it)->getBuff() == toRemove) {
            COMBAT_LOG(timestamp, this, "Buff "<<FONT_GREEN<<toRemove->getName()<<END_FONT<<" faded from "<<this->getName());
            this->Buffs.erase(it);
            return true;
        }
    }
    return false;
}

bool Combatant::removeDebuffByType(Buff *toRemove, float timestamp)
{
    for (auto&& it=this->Debuffs.begin(); it!=this->Debuffs.end(); ++it) {
        if ((*it)->getBuff() == toRemove) {
            COMBAT_LOG(timestamp, this, "Debuff "<<FONT_RED<<toRemove->getName()<<END_FONT<<" faded from "<<this->getName());
            this->Debuffs.erase(it);
            return true;
        }
    }
    return false;
}

bool Combatant::removeBuff(AppliedBuff *toRemove, float timestamp)
{
    for (auto&& it=this->Buffs.begin(); it!=this->Buffs.end(); ++it) {
        if (*it == toRemove) {
            COMBAT_LOG(timestamp, this, "Buff "<<FONT_GREEN<<toRemove->getBuff()->getName()<<END_FONT<<" faded from "<<this->getName());
            this->Buffs.erase(it);
            return true;
        }
    }
    return false;
}

bool Combatant::removeDebuff(AppliedBuff *toRemove, float timestamp)
{
    for (auto&& it=this->Debuffs.begin(); it!=this->Debuffs.end(); ++it) {
        if (*it == toRemove) {
            COMBAT_LOG(timestamp, this, "Debuff "<<FONT_RED<<toRemove->getBuff()->getName()<<END_FONT<<" faded from "<<this->getName());
            this->Debuffs.erase(it);
            return true;
        }
    }
    return false;
}

void Combatant::applyDotDamage(PlayerCharacter *PC, float timestamp)
{
    for (auto buff : this->Debuffs) {
        if (buff->getBuff()->getOnDotTickDamage() != nullptr) {
            if (buff->isTickTimerExpired(this, timestamp)) {
                buff->triggerTickTimer(timestamp);
                float buffDuration = buff->getCapturedDuration();
                int32_t rank = 0;
                if (buff->getBuff()->getParent() != nullptr) {
                    rank = buff->getBuff()->getParent()->getRank();
                }
                int32_t damage = buff->getBuff()->getOnDotTickDamage()(PC, this, rank, buff->getTickCount(), buffDuration);
                this->applyDamage((Combatant *)PC, damage, MeleeHitResult::OrdinaryHit, timestamp, buff->getBuff());
            }
        }
    }
}

void Combatant::tickBuffs(PlayerCharacter *PC, float timestamp)
{
    for (auto buff : this->Buffs) {
        if (buff->getBuff()->getOnBuffTick() != nullptr) {
            if (buff->isTickTimerExpired(this, timestamp)) {
                buff->triggerTickTimer(timestamp);
                float buffDuration = buff->getCapturedDuration();
                int32_t resourceBefore = PC->getResource();
                buff->getBuff()->getOnBuffTick()(PC, this, buff->getBuff()->getParent()->getRank(), buff->getTickCount(), buffDuration);
                int32_t resourceAfter = PC->getResource();
                if (resourceAfter > resourceBefore) {
                    int32_t resourceDiff = resourceAfter - resourceBefore;
                    COMBAT_LOG(timestamp, PC, ATTACKER_FONT_COLOR<<PC->getName()<<END_FONT<<" gained "<<resourceDiff<<" rage from "<<ABILITY_FONT_COLOR<<buff->getBuff()->getName()<<END_FONT);
                }
            }
        }
    }
}

int32_t Combatant::applyDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Ability *abilitySource)
{
    if (abilitySource != nullptr) {
        if (abilitySource->getIgnoresArmor()) {
            return this->applyArmorIgnoreDamage(attacker, damage, mhr, timestamp, abilitySource);
        }
        else {
            if (abilitySource->getAbilityDamageType() == AbilityDamageType::Physical) {
                return this->applyPhysicalDamage(attacker, damage, mhr, timestamp, abilitySource);
            } else if (abilitySource->getAbilityDamageType() == AbilityDamageType::Magical) {
                return this->applyMagicalDamage(attacker, damage, mhr, timestamp, abilitySource);
            } else {
                return this->applyArmorIgnoreDamage(attacker, damage, mhr, timestamp, abilitySource);
            }
        }
    }
}

int32_t Combatant::applyDamage(Combatant *attacker, int32_t damage, MeleeHitResult mhr, float timestamp, Buff *buffSource)
{
    if (buffSource != nullptr) {
        if (buffSource->getIgnoresArmor()) {
            return this->applyArmorIgnoreDamage(attacker, damage, mhr, timestamp, buffSource);
        }
        else {
            if (buffSource->getAbilityDamageType() == AbilityDamageType::Physical) {
                return this->applyPhysicalDamage(attacker, damage, mhr, timestamp, buffSource);
            } else if (buffSource->getAbilityDamageType() == AbilityDamageType::Magical) {
                return this->applyMagicalDamage(attacker, damage, mhr, timestamp, buffSource);
            } else {
                return this->applyArmorIgnoreDamage(attacker, damage, mhr, timestamp, buffSource);
            }
        }
    }
}

void Combatant::removeExpiredBuffs(float timestamp)
{
    std::vector<AppliedBuff *> toRemove;
    for (auto buff : this->Buffs) {
        if (buff->isBuffExpired(timestamp)) {
            toRemove.push_back(buff);
        }
    }
    
    for (auto buff : toRemove) {
        this->removeBuff(buff, timestamp);
    }
}

void Combatant::removeExpiredDebuffs(float timestamp)
{
    std::vector<AppliedBuff *> toRemove;
    for (auto buff : this->Debuffs) {
        if (buff->isBuffExpired(timestamp)) {
            toRemove.push_back(buff);
        }
    }
    
    for (auto buff : toRemove) {
        this->removeDebuff(buff, timestamp);
    }
}

void Combatant::clearAllBuffsAndDebuffsAndFreeMemory()
{
    for (int i=0; i<this->Buffs.size(); ++i) {
        delete this->Buffs[i];
    }
    this->Buffs.clear();
    for (int i=0; i<this->Debuffs.size(); ++i) {
        delete this->Debuffs[i];
    }
    this->Debuffs.clear();
}

std::string Combatant::getName() const
{
    return name;
}

void Combatant::setName(const std::string &value)
{
    name = value;
}

CombatLog *Combatant::getCombatLog() const
{
    return combatLog;
}

int32_t Combatant::getCurrentHp() const
{
    return currentHp;
}

void Combatant::setCurrentHp(const int32_t &value)
{
    currentHp = value;
}

int32_t Combatant::getMaxHp() const
{
    return maxHp;
}

void Combatant::setMaxHp(const int32_t &value)
{
    maxHp = value;
}

void Combatant::setHp(const int32_t &value)
{
    this->currentHp = value;
    this->maxHp = value;
}

int32_t Combatant::getDamageDone() const
{
    return damageDone;
}

void Combatant::setDamageDone(const int32_t &value)
{
    damageDone = value;
}

bool Combatant::getIsGcdActive(float timestamp)
{
    float timeDiff = timestamp - this->gcdStartTime;
    timeDiff += FLOATING_POINT_SMALL_NUMBER;
    if (timeDiff >= gcdDuration) {
        this->isGcdActive = false;
        return false;
    }
    return isGcdActive;
}

void Combatant::setIsGcdActive(bool value)
{
    isGcdActive = value;
}

float Combatant::getGcdStartTime() const
{
    return gcdStartTime;
}

void Combatant::setGcdStartTime(float value)
{
    gcdStartTime = value;
}

bool Combatant::getIsCasting() const
{
    return isCasting;
}

void Combatant::setIsCasting(bool value)
{
    isCasting = value;
}

Ability *Combatant::getCastingAbility() const
{
    return castingAbility;
}

void Combatant::setCastingAbility(Ability *value)
{
    castingAbility = value;
}

float Combatant::getCastStartTime() const
{
    return castStartTime;
}

void Combatant::setCastStartTime(float value)
{
    castStartTime = value;
}

Combatant::Combatant()
{
    
}

Combatant::~Combatant()
{
    this->clearAllBuffsAndDebuffsAndFreeMemory();
    
    delete combatLog;
    combatLog = nullptr;
}
