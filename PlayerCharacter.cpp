#include "PlayerCharacter.h"
#include "Weapon.h"
#include <random>
#include "DamageSimulation.h"
#include "GlobalAbilityList.h"

int32_t PlayerCharacter::getIntellect() const
{
    return intellect;
}

void PlayerCharacter::setIntellect(const int32_t &value)
{
    intellect = value;
}

int32_t PlayerCharacter::getArmor() const
{
    return armor;
}

void PlayerCharacter::setArmor(const int32_t &value)
{
    armor = value;
}

int32_t PlayerCharacter::getSpirit() const
{
    return spirit;
}

void PlayerCharacter::setSpirit(const int32_t &value)
{
    spirit = value;
}

int32_t PlayerCharacter::getStamina() const
{
    return stamina;
}

void PlayerCharacter::setStamina(const int32_t &value)
{
    stamina = value;
}

int32_t PlayerCharacter::getAgility() const
{
    return agility;
}

void PlayerCharacter::setAgility(const int32_t &value)
{
    agility = value;
}

int32_t PlayerCharacter::getStrength() const
{
    return strength;
}

void PlayerCharacter::setStrength(const int32_t &value)
{
    strength = value;
}

int32_t PlayerCharacter::getFinalStrengthValue()
{
    int32_t str = this->strength;
    for (int i=0; i<this->Buffs.size(); ++i) {
        int32_t rank = 0;
        if (this->Buffs[i]->getBuff()->getParent() != nullptr) {
            rank = this->Buffs[i]->getBuff()->getParent()->getRank();
        }
        if (this->Buffs[i]->getBuff()->getOnCalculateStrengthBoost() != nullptr) {
            str += this->Buffs[i]->getBuff()->getOnCalculateStrengthBoost()(this, rank);
        }
        if (this->Buffs[i]->getBuff()->getOnCalculateAllStatsBoost() != nullptr) {
            str += this->Buffs[i]->getBuff()->getOnCalculateAllStatsBoost()(this, rank);
        }
    }
    for (int i=0; i<this->Buffs.size(); ++i) {
        int32_t rank = 0;
        if (this->Buffs[i]->getBuff()->getParent() != nullptr) {
            rank = this->Buffs[i]->getBuff()->getParent()->getRank();
        }
        if (this->Buffs[i]->getBuff()->getOnCalculatePercentStatBoost() != nullptr) {
            str *= this->Buffs[i]->getBuff()->getOnCalculatePercentStatBoost()(this, rank);
        }
    }
    return str;
}

int32_t PlayerCharacter::getCalculatedWeaponSkill()
{
    int32_t racialBoost = 0;
    if (this->getPlayerRace() == "Human") {
        racialBoost = 5;
    }
    return Combatant::getCalculatedWeaponSkill() + racialBoost;
}

void PlayerCharacter::disableAutoAttack()
{
    PriorityAction *mhAttack = this->getPriorityActionList()->getMainHandAutoAttackAction();
    PriorityAction *ohAttack = this->getPriorityActionList()->getOffHandAutoAttackAction();
    if (mhAttack)
        mhAttack->setDisabled(true);
    if (ohAttack)
        ohAttack->setDisabled(true);
}

void PlayerCharacter::enableAndResetAutoAttack(float timestamp)
{
    PriorityAction *mhAttack = this->getPriorityActionList()->getMainHandAutoAttackAction();
    PriorityAction *ohAttack = this->getPriorityActionList()->getOffHandAutoAttackAction();
    if (mhAttack) {
        mhAttack->setDisabled(false);
        mhAttack->getAbility()->triggerCooldown(this, timestamp, false);
    }
    if (ohAttack) {
        ohAttack->setDisabled(false);
        ohAttack->getAbility()->triggerCooldown(this, timestamp, false);
    }
}

void PlayerCharacter::addTalent(std::string name, int32_t rank)
{
    this->talents.push_back(new Talent(name, rank));
}

int32_t PlayerCharacter::getTalentRank(std::string name)
{
    for (int i=0; i<this->talents.size(); ++i) {
        if (this->talents[i]->getName() == name) {
            return this->talents[i]->getRank();
        }
    }
    return 0;
}

Talent *PlayerCharacter::getTalent(std::string name)
{
    for (int i=0; i<this->talents.size(); ++i) {
        if (this->talents[i]->getName() == name) {
            return this->talents[i];
        }
    }
    return nullptr;
}

bool PlayerCharacter::hasTwoHandedWeaponEquipped()
{
    if (this->mainHandItem != nullptr) {
        if (Weapon *wep = dynamic_cast<Weapon *>(this->mainHandItem)) {
            if (wep->getSlot() == ItemSlot::TwoHand) {
                return true;
            }
        }
    }
    return false;
}

int32_t PlayerCharacter::applyMeleeApBuffs(int32_t AP)
{
    for (int i=0; i<this->Buffs.size(); ++i) {
        if (this->Buffs[i]->getBuff()->getOnCalculateAttackPower() != nullptr && this->Buffs[i]->getBuff()->getParent() != nullptr) {
            AP = this->Buffs[i]->getBuff()->getOnCalculateAttackPower()(this, this->Buffs[i]->getBuff()->getParent()->getRank(), AP);
        }
    }
    return AP;
}

//strictly less than 1
float PlayerCharacter::getRandomFloat()
{
    std::uniform_real_distribution<> dist(0.0f, 1.0f);
    return dist(DamageSimulation::getRandEngine());
}

int32_t PlayerCharacter::maybeApplyCritDamage(Ability *source, int32_t value, bool& didCrit)
{
    didCrit = false;
    if (this->canCrit) {
        if (this->alwaysUseAverageDamage && this->bakeCritIntoAverageDamage) {
            float nonCritPortion = 1.0f - this->critChance;
            float critPortion = this->critChance;
            int32_t weightedAverage = nonCritPortion*value + critPortion*(value*this->getCritStrikeDamageMultiplier(this->isWhiteAttack(source)));
            return weightedAverage;
        } else {
            float critDiceRoll = this->getRandomFloat();
            if (critDiceRoll < this->critChance) {
                didCrit = true;
            }
            if (didCrit)
                value *= this->getCritStrikeDamageMultiplier(this->isWhiteAttack(source));
            return value;
        }
    }
    return value;
}

int32_t PlayerCharacter::applyCritDamage(Ability *source, int32_t value)
{
    if (this->canCrit) {
        if (this->alwaysUseAverageDamage && this->bakeCritIntoAverageDamage) {
            float nonCritPortion = 1.0f - this->critChance;
            float critPortion = this->critChance;
            int32_t weightedAverage = nonCritPortion*value + critPortion*(value*this->getCritStrikeDamageMultiplier(this->isWhiteAttack(source)));
            return weightedAverage;
        } else {
            value *= this->getCritStrikeDamageMultiplier(this->isWhiteAttack(source));
            return value;
        }
    }
    return value;
}

bool PlayerCharacter::isWhiteAttack(Ability *ability)
{
    if (ability != nullptr) {
        if (ability->getName() == "Main-hand attack" || ability->getName() == "Off-hand attack") {
            return true;
        }
    }
    return false;
}

float PlayerCharacter::getCritStrikeDamageMultiplier(bool whiteAttack)
{
    if (!whiteAttack) {
        int32_t impale = this->getTalentRank("Impale");
        if (impale > 0) {
            return (this->baseCritStrikeDamageMultipier + 0.10f*impale);
        }
    }
    return this->baseCritStrikeDamageMultipier;
}

void PlayerCharacter::downrankAbilitiesIfRequired()
{
    if (this->getPriorityActionList() != nullptr) {
        for (auto& action : this->getPriorityActionList()->getPriorityActions()) {
            if (action->getAbility() != nullptr) {
                std::vector<int32_t>& learnLevels = action->getAbility()->getLearnLevels();
                for (int i=0; i<learnLevels.size(); ++i) {
                    int32_t learnLevel = learnLevels[i];
                    if (this->getLevel() < learnLevel) {
                        int32_t rank = i;
                        action->getAbility()->setRank(rank);
                        break;
                    }
                }
            }
        }
    }
}

float PlayerCharacter::calculateGlobalDamageBonus()
{
    int32_t spec2h = this->getTalentRank("Two-Handed Weapon Specialization");
    float specDamageBonus = 1.0f;
    if (this->hasTwoHandedWeaponEquipped()) {
        specDamageBonus = 1.0f + spec2h/100.0f;
    }
    if (this->hasBuff(GlobalAbilityList::activeList->Enrage)) {
        specDamageBonus *= 1.20f;
    }
    specDamageBonus *= this->globalDamageModifier;
    return specDamageBonus;
}

int32_t PlayerCharacter::calculateSimulatedMainhandSwing()
{
    if (this->alwaysUseAverageDamage) {
        return this->calculatedMainhandWeaponAverageDamage();
    }
    return DamageSimulation::randomIntBetween(this->calculatedMainhandWeaponMinDamage(), this->calculatedMainhandWeaponMaxDamage());
}

int32_t PlayerCharacter::calculateSimulatedOffhandSwing()
{
    if (this->alwaysUseAverageDamage) {
        return this->calculatedOffhandWeaponAverageDamage();
    }
    return DamageSimulation::randomIntBetween(this->calculatedOffhandWeaponMinDamage(), this->calculatedOffhandWeaponMaxDamage());
}

float PlayerCharacter::calculateDpsFromAttackPower(int32_t attackPower)
{
    return attackPower/14.0f;
}

/**
 * @brief PlayerCharacter::calculateMeleeAttackPower
 * @return 
 * 
 * 
 * AP:
    Death knight	(Level x 3) + (Strength x 2 - 20)	
    Druid	(Level x 3) + (Strength x 2 - 20)	
    Druid (Bear)	[(Level x 3) + (Strength x 2 - 20) + (Agility x 2 - 20)] x 125%	
    Druid Druid (Cat)	[(Level x 3) + (Strength x 2 - 20) + (Agility x 2 - 20)] x 125%	
    Hunter	(Level x 2) + (Strength - 10) + (Agility - 10), ranged=(Level x 2) + (Agility x 2 - 20)
    Mage	Strength x 2 - 20	
    Paladin	(Level x 3) + (Strength x 2 - 20)	
    Priest	Strength x 2 - 20	
    Rogue	(Level x 2) + (Strength - 10) + (Agility x 2 - 20), ranged=Level + (Agility - 10)
    Shaman	(Level x 2) + (Strength - 10) + (Agility x 2 - 20)	
    Warlock	Strength x 2 - 20	
    Warrior	(Level x 3) + (Strength x 2 - 20), ranged=Level + (Agility - 10)
 * 
 * 
 */
int32_t PlayerCharacter::calculateMeleeAttackPower()
{
    if (this->attackPowerOverride == -1) {
        if (this->playerClass.cls == this->playerClass.WARRIOR) {
            return applyMeleeApBuffs(this->level*3 + (this->getFinalStrengthValue()*2-20));
        }
        return 0;
    } else {
        return this->attackPowerOverride;
    }
}

int32_t PlayerCharacter::calculatedWeaponMinDamage()
{
    int32_t mhMinDamage = 0;
    int32_t ohMinDamage = 0;
    mhMinDamage = this->calculatedMainhandWeaponMinDamage();
    ohMinDamage = this->calculatedOffhandWeaponMinDamage();
    return mhMinDamage + ohMinDamage;
}

int32_t PlayerCharacter::calculatedWeaponMaxDamage()
{
    int32_t mhMaxDamage = 0;
    int32_t ohMaxDamage = 0;
    mhMaxDamage = this->calculatedMainhandWeaponMaxDamage();
    ohMaxDamage = this->calculatedOffhandWeaponMaxDamage();
    return mhMaxDamage + ohMaxDamage;
}

int32_t PlayerCharacter::calculatedMainhandWeaponMinDamage()
{
    int32_t mhMinDamage = 0;
    if (this->mainHandItem != nullptr) {
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->mainHandItem)) {
            mhMinDamage = weapon->getMinDamage();
        }
    }
    mhMinDamage += this->calculatedMainhandWeaponSpeed()*this->calculateDpsFromAttackPower(this->calculateMeleeAttackPower());
    return mhMinDamage;
}

int32_t PlayerCharacter::calculatedMainhandWeaponMaxDamage()
{
    int32_t mhMaxDamage = 0;
    if (this->mainHandItem != nullptr) {
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->mainHandItem)) {
            mhMaxDamage = weapon->getMaxDamage();
        }
    }
    mhMaxDamage += this->calculatedMainhandWeaponSpeed()*this->calculateDpsFromAttackPower(this->calculateMeleeAttackPower());
    return mhMaxDamage;
}

int32_t PlayerCharacter::calculatedMainhandWeaponAverageDamage()
{
    return (this->calculatedMainhandWeaponMinDamage() + this->calculatedMainhandWeaponMaxDamage())/2;
}

int32_t PlayerCharacter::calculatedOffhandWeaponMinDamage()
{
    int32_t ohMinDamage = 0;
    if (this->offHandItem != nullptr) {
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->offHandItem)) {
            ohMinDamage = weapon->getMinDamage() + this->calculatedOffhandWeaponSpeed()*this->calculateDpsFromAttackPower(this->calculateMeleeAttackPower());
        }
        ohMinDamage /= 2;
    }
    return ohMinDamage;
}

int32_t PlayerCharacter::calculatedOffhandWeaponMaxDamage()
{
    int32_t ohMaxDamage = 0;
    if (this->offHandItem != nullptr) {
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->offHandItem)) {
            ohMaxDamage = weapon->getMaxDamage() + this->calculatedOffhandWeaponSpeed()*this->calculateDpsFromAttackPower(this->calculateMeleeAttackPower());
        }
        ohMaxDamage /= 2;
    }
    return ohMaxDamage;
}

int32_t PlayerCharacter::calculatedOffhandWeaponAverageDamage()
{
    return (this->calculatedOffhandWeaponMinDamage() + this->calculatedOffhandWeaponMaxDamage())/2;
}

int32_t PlayerCharacter::calculatedWeaponAverageDamage()
{
    return (this->calculatedWeaponMinDamage() + this->calculatedWeaponMaxDamage())/2;
}

float PlayerCharacter::calculatedMainhandWeaponSpeed()
{
    if (this->mainHandItem != nullptr) {
        float speedBoost = 1.0f;
        if (this->hasRune("Frenzied Assault") && this->hasTwoHandedWeaponEquipped()) {
            speedBoost = 1.20f;
        }
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->mainHandItem)) {
            return weapon->getWeaponSpeed()/speedBoost;
        }
    }
    return 2.0f;
}

float PlayerCharacter::calculatedOffhandWeaponSpeed()
{
    if (this->offHandItem != nullptr) {
        float speedBoost = 1.0f;
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->offHandItem)) {
            return weapon->getWeaponSpeed()/speedBoost;
        }
    }
    return 0.0f;
}

float PlayerCharacter::calculatedMainhandWeaponDps()
{
    return this->calculatedWeaponAverageDamage()/this->calculatedMainhandWeaponSpeed();
}

Item *PlayerCharacter::getMainHandItem() const
{
    return mainHandItem;
}

void PlayerCharacter::setMainHandItem(Item *value)
{
    mainHandItem = value;
}

Item *PlayerCharacter::getOffHandItem() const
{
    return offHandItem;
}

void PlayerCharacter::setOffHandItem(Item *value)
{
    offHandItem = value;
}

PlayerClass& PlayerCharacter::getPlayerClass()
{
    return playerClass;
}

void PlayerCharacter::setPlayerClass(const PlayerClass &value)
{
    playerClass = value;
    
    this->initResourceValue();
}

void PlayerCharacter::initResourceValue()
{
    if (this->getPlayerClass().cls == this->getPlayerClass().WARRIOR) {
        this->setResource(0);
        this->setResourceMax(100);
    } else {
        this->setResource(this->getResourceMax());
    }
}

int32_t PlayerCharacter::getLevel() const
{
    return level;
}

void PlayerCharacter::setLevel(const int32_t &value)
{
    level = value;
    
    this->downrankAbilitiesIfRequired();
}

bool PlayerCharacter::getAlwaysUseAverageDamage() const
{
    return alwaysUseAverageDamage;
}

void PlayerCharacter::setAlwaysUseAverageDamage(bool value)
{
    alwaysUseAverageDamage = value;
}

Enemy *PlayerCharacter::getTarget() const
{
    return target;
}

void PlayerCharacter::setTarget(Enemy *value)
{
    target = value;
}

std::string PlayerCharacter::getPlayerRace() const
{
    return playerRace;
}

void PlayerCharacter::setPlayerRace(const std::string &value)
{
    playerRace = value;
}

PriorityActionList *PlayerCharacter::getPriorityActionList() const
{
    return priorityActionList;
}

void PlayerCharacter::setPriorityActionList(PriorityActionList *value)
{
    priorityActionList = value;
}

void PlayerCharacter::resetAllTalentTimestamps()
{
    for (int i=0; i<this->talents.size(); ++i) {
        this->talents[i]->setLastUsedTimestamp(0.0f);
    }
}

float PlayerCharacter::getCritChance() const
{
    return critChance;
}

void PlayerCharacter::setCritChance(float value)
{
    critChance = value;
}

Item *PlayerCharacter::getAmmoItem() const
{
    return ammoItem;
}

void PlayerCharacter::setAmmoItem(Item *value)
{
    ammoItem = value;
}

Item *PlayerCharacter::getRangedItem() const
{
    return rangedItem;
}

void PlayerCharacter::setRangedItem(Item *value)
{
    rangedItem = value;
}

Item *PlayerCharacter::getTrinket2Item() const
{
    return trinket2Item;
}

void PlayerCharacter::setTrinket2Item(Item *value)
{
    trinket2Item = value;
}

Item *PlayerCharacter::getTrinket1Item() const
{
    return trinket1Item;
}

void PlayerCharacter::setTrinket1Item(Item *value)
{
    trinket1Item = value;
}

Item *PlayerCharacter::getFinger2Item() const
{
    return finger2Item;
}

void PlayerCharacter::setFinger2Item(Item *value)
{
    finger2Item = value;
}

Item *PlayerCharacter::getFinger1Item() const
{
    return finger1Item;
}

void PlayerCharacter::setFinger1Item(Item *value)
{
    finger1Item = value;
}

Item *PlayerCharacter::getFeetItem() const
{
    return feetItem;
}

void PlayerCharacter::setFeetItem(Item *value)
{
    feetItem = value;
}

Item *PlayerCharacter::getLegItem() const
{
    return legItem;
}

void PlayerCharacter::setLegItem(Item *value)
{
    legItem = value;
}

Item *PlayerCharacter::getBeltItem() const
{
    return beltItem;
}

void PlayerCharacter::setBeltItem(Item *value)
{
    beltItem = value;
}

Item *PlayerCharacter::getHandItem() const
{
    return handItem;
}

void PlayerCharacter::setHandItem(Item *value)
{
    handItem = value;
}

Item *PlayerCharacter::getWristItem() const
{
    return wristItem;
}

void PlayerCharacter::setWristItem(Item *value)
{
    wristItem = value;
}

Item *PlayerCharacter::getTabardItem() const
{
    return tabardItem;
}

void PlayerCharacter::setTabardItem(Item *value)
{
    tabardItem = value;
}

Item *PlayerCharacter::getShirtItem() const
{
    return shirtItem;
}

void PlayerCharacter::setShirtItem(Item *value)
{
    shirtItem = value;
}

Item *PlayerCharacter::getChestItem() const
{
    return chestItem;
}

void PlayerCharacter::setChestItem(Item *value)
{
    chestItem = value;
}

Item *PlayerCharacter::getBackItem() const
{
    return backItem;
}

void PlayerCharacter::setBackItem(Item *value)
{
    backItem = value;
}

Item *PlayerCharacter::getShoulderItem() const
{
    return shoulderItem;
}

void PlayerCharacter::setShoulderItem(Item *value)
{
    shoulderItem = value;
}

Item *PlayerCharacter::getNeckItem() const
{
    return neckItem;
}

void PlayerCharacter::setNeckItem(Item *value)
{
    neckItem = value;
}

Item *PlayerCharacter::getHeadItem() const
{
    return headItem;
}

void PlayerCharacter::setHeadItem(Item *value)
{
    headItem = value;
}

std::vector<Rune *>& PlayerCharacter::getRunes()
{
    return runes;
}

bool PlayerCharacter::hasRune(std::string runeName)
{
    for (int i=0; i<this->runes.size(); ++i) {
        if (this->runes[i]->getName() == runeName)
            return true;
    }
    return false;
}

PriorityAction *PlayerCharacter::getLastUsedAction() const
{
    return lastUsedAction;
}

void PlayerCharacter::setLastUsedAction(PriorityAction *value)
{
    lastUsedAction = value;
}

bool PlayerCharacter::getBakeCritIntoAverageDamage() const
{
    return bakeCritIntoAverageDamage;
}

void PlayerCharacter::setBakeCritIntoAverageDamage(bool value)
{
    bakeCritIntoAverageDamage = value;
}

bool PlayerCharacter::getCanCrit() const
{
    return canCrit;
}

void PlayerCharacter::setCanCrit(bool value)
{
    canCrit = value;
}

int32_t PlayerCharacter::getAttackPowerOverride() const
{
    return attackPowerOverride;
}

void PlayerCharacter::setAttackPowerOverride(const int32_t &value)
{
    attackPowerOverride = value;
}

bool PlayerCharacter::getBakeWildStrikesIntoAverageDamage() const
{
    return bakeWildStrikesIntoAverageDamage;
}

void PlayerCharacter::setBakeWildStrikesIntoAverageDamage(bool value)
{
    bakeWildStrikesIntoAverageDamage = value;
}

float PlayerCharacter::getGlobalDamageModifier() const
{
    return globalDamageModifier;
}

void PlayerCharacter::setGlobalDamageModifier(float value)
{
    globalDamageModifier = value;
}

float PlayerCharacter::getHitChanceModifier() const
{
    return hitChanceModifier;
}

void PlayerCharacter::setHitChanceModifier(float value)
{
    hitChanceModifier = value;
}

PlayerCharacter::PlayerCharacter()
{
    ;
}

PlayerCharacter::~PlayerCharacter()
{
    for (int i=0; i<this->talents.size(); ++i) {
        delete this->talents[i];
    }
    this->talents.clear();
    
    if (headItem != nullptr) {delete headItem; headItem = nullptr;}
    if (neckItem != nullptr) {delete neckItem; neckItem = nullptr;}
    if (shoulderItem != nullptr) {delete shoulderItem; shoulderItem = nullptr;}
    if (backItem != nullptr) {delete backItem; backItem = nullptr;}
    if (chestItem != nullptr) {delete chestItem; chestItem = nullptr;}
    if (shirtItem != nullptr) {delete shirtItem; shirtItem = nullptr;}
    if (tabardItem != nullptr) {delete tabardItem; tabardItem = nullptr;}
    if (wristItem != nullptr) {delete wristItem; wristItem = nullptr;}
    if (handItem != nullptr) {delete handItem; handItem = nullptr;}
    if (beltItem != nullptr) {delete beltItem; beltItem = nullptr;}
    if (legItem != nullptr) {delete legItem; legItem = nullptr;}
    if (feetItem != nullptr) {delete feetItem; feetItem = nullptr;}
    if (finger1Item != nullptr) {delete finger1Item; finger1Item = nullptr;}
    if (finger2Item != nullptr) {delete finger2Item; finger2Item = nullptr;}
    if (trinket1Item != nullptr) {delete trinket1Item; trinket1Item = nullptr;}
    if (trinket2Item != nullptr) {delete trinket2Item; trinket2Item = nullptr;}
    if (mainHandItem != nullptr) {delete mainHandItem; mainHandItem = nullptr;}
    if (offHandItem != nullptr) {delete offHandItem; offHandItem = nullptr;}
    if (rangedItem != nullptr) {delete rangedItem; rangedItem = nullptr;}
    if (ammoItem != nullptr) {delete ammoItem; ammoItem = nullptr;}
}
