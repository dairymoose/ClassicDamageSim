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

int32_t Combatant::applyPhysicalDamage(Combatant *attacker, int32_t damage, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage * (1.0f - physicalDamageReduction);
        COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" did "<<DAMAGE_FONT_COLOR<<calcDmg<<END_FONT<<" physical damage to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" from "<<ABILITY_FONT_COLOR<<abilitySource->getName()<<END_FONT);
        this->currentHp -= calcDmg;
        attacker->setDamageDone(attacker->getDamageDone() + calcDmg);
        return calcDmg;
    }
    return 0;
}

int32_t Combatant::applyMagicalDamage(Combatant *attacker, int32_t damage, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage * (1.0f - magicalDamageReduction);
        COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" did "<<DAMAGE_FONT_COLOR<<calcDmg<<END_FONT<<" magical damage to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" from "<<ABILITY_FONT_COLOR<<abilitySource->getName()<<END_FONT);
        this->currentHp -= calcDmg;
        attacker->setDamageDone(attacker->getDamageDone() + calcDmg);
        return calcDmg;
    }
    return 0;
}

int32_t Combatant::applyArmorIgnoreDamage(Combatant *attacker, int32_t damage, float timestamp, Ability *abilitySource)
{
    if (attacker != nullptr) {
        int32_t calcDmg = damage;
        COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" did "<<DAMAGE_FONT_COLOR<<calcDmg<<END_FONT<<" armor-ignore damage to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" from "<<ABILITY_FONT_COLOR<<abilitySource->getName()<<END_FONT);
        this->currentHp -= damage;
        attacker->setDamageDone(attacker->getDamageDone() + calcDmg);
        return calcDmg;
    }
    return 0;
}

void Combatant::applyBuff(Combatant *attacker, float timestamp, Buff *buff, bool isFree)
{
    if (attacker != nullptr) {
        if (buff->getOnCalculateDuration() != nullptr) {
            float duration = buff->getOnCalculateDuration()(this, buff->getParent()->getRank());
            if (duration > 0.0f) {
                if (this->removeBuffByType(buff)) {
                    COMBAT_LOG(timestamp, attacker, "Buff "<<FONT_GREEN<<buff->getName()<<END_FONT<<" faded from "<<this->getName());
                }
                COMBAT_LOG(timestamp, attacker, ATTACKER_FONT_COLOR<<attacker->getName()<<END_FONT<<" applied "<<(isFree ? "FREE " : "")<<"buff "<<FONT_GREEN<<buff->getName()<<END_FONT<<" to "<<RECEIVER_FONT_COLOR<<this->getName()<<END_FONT<<" with duration "<<(int32_t)duration);
                AppliedBuff *AB = new AppliedBuff(buff);
                AB->setAppliedTimestamp(timestamp);
                AB->setLastTickedTimestamp(timestamp);
                AB->setCapturedDuration(duration);
                this->Buffs.push_back(AB);
            }
        }
    }
}

void Combatant::applyDebuff(Combatant *attacker, float timestamp, Buff *debuff)
{
    if (attacker != nullptr) {
        if (debuff->getOnCalculateDuration() != nullptr) {
            float duration = debuff->getOnCalculateDuration()(this, debuff->getParent()->getRank());
            if (duration > 0.0f) {
                if (this->removeDebuffByType(debuff)) {
                    COMBAT_LOG(timestamp, attacker, "Debuff "<<FONT_RED<<debuff->getName()<<END_FONT<<" faded from "<<this->getName());
                }
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

bool Combatant::removeBuffByType(Buff *toRemove)
{
    for (auto&& it=this->Buffs.begin(); it!=this->Buffs.end(); ++it) {
        if ((*it)->getBuff() == toRemove) {
            this->Buffs.erase(it);
            return true;
        }
    }
    return false;
}

bool Combatant::removeDebuffByType(Buff *toRemove)
{
    for (auto&& it=this->Debuffs.begin(); it!=this->Debuffs.end(); ++it) {
        if ((*it)->getBuff() == toRemove) {
            this->Debuffs.erase(it);
            return true;
        }
    }
    return false;
}

bool Combatant::removeBuff(AppliedBuff *toRemove)
{
    for (auto&& it=this->Buffs.begin(); it!=this->Buffs.end(); ++it) {
        if (*it == toRemove) {
            this->Buffs.erase(it);
            return true;
        }
    }
    return false;
}

bool Combatant::removeDebuff(AppliedBuff *toRemove)
{
    for (auto&& it=this->Debuffs.begin(); it!=this->Debuffs.end(); ++it) {
        if (*it == toRemove) {
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
                int32_t damage = buff->getBuff()->getOnDotTickDamage()(this, buff->getBuff()->getParent()->getRank(), buff->getTickCount());
                this->applyDamage((Combatant *)PC, damage, timestamp, buff->getBuff()->getParent());
            }
        }
//        int32_t lastTickCount = buff->getLastTickedCount();
//        int32_t newTickCount = buff->getTimesTicked(this, timestamp);
//        if (newTickCount > lastTickCount) {
//            int32_t damage = buff->getOnDotTickDamage()(this, buff->getParent()->getRank(), newTickCount);
//            this->applyDamage((Combatant *)PC, damage, timestamp, buff->getParent());
//        }
    }
}

int32_t Combatant::applyDamage(Combatant *attacker, int32_t damage, float timestamp, Ability *abilitySource)
{
    if (abilitySource != nullptr) {
        if (abilitySource->getIgnoresArmor()) {
            return this->applyArmorIgnoreDamage(attacker, damage, timestamp, abilitySource);
        }
        else {
            if (abilitySource->getAbilityDamageType() == AbilityDamageType::Physical) {
                return this->applyPhysicalDamage(attacker, damage, timestamp, abilitySource);
            } else if (abilitySource->getAbilityDamageType() == AbilityDamageType::Magical) {
                return this->applyMagicalDamage(attacker, damage, timestamp, abilitySource);
            } else {
                return this->applyArmorIgnoreDamage(attacker, damage, timestamp, abilitySource);
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
        this->removeBuff(buff);
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
        this->removeDebuff(buff);
    }
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
