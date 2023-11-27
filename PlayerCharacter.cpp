#include "PlayerCharacter.h"
#include "Weapon.h"
#include <random>
#include "DamageSimulation.h"

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
        if (this->Buffs[i]->getBuff()->getOnCalculateAttackPower() != nullptr) {
            AP = this->Buffs[i]->getBuff()->getOnCalculateAttackPower()(this->Buffs[i]->getBuff()->getParent()->getRank(), AP);
        }
    }
    return AP;
}

float PlayerCharacter::calculateGlobalDamageBonus()
{
    int32_t spec2h = this->getTalentRank("Two-Handed Weapon Specialization");
    float specDamageBonus = 1.0f;
    if (this->hasTwoHandedWeaponEquipped()) {
        specDamageBonus = 1.0f + spec2h/100.0f;
    }
    return specDamageBonus;
}

int32_t PlayerCharacter::calculateSimulatedMainhandSwing()
{
    if (this->alwaysUseAverageDamage) {
        return this->calculatedMainhandWeaponAverageDamage();
    }
    std::uniform_int_distribution<> dist(this->calculatedMainhandWeaponMinDamage(), this->calculatedMainhandWeaponMaxDamage());
    return dist(DamageSimulation::getRandEngine());
}

int32_t PlayerCharacter::calculateSimulatedOffhandSwing()
{
    if (this->alwaysUseAverageDamage) {
        return this->calculatedOffhandWeaponAverageDamage();
    }
    std::uniform_int_distribution<> dist(this->calculatedOffhandWeaponMinDamage(), this->calculatedOffhandWeaponMaxDamage());
    return dist(DamageSimulation::getRandEngine());
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
    if (this->playerClass.cls == this->playerClass.Warrior) {
        return applyMeleeApBuffs(this->level*3 + (this->strength*2-20));
    }
    return 0;
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
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->mainHandItem)) {
            return weapon->getWeaponSpeed();
        }
    }
    return 2.0f;
}

float PlayerCharacter::calculatedOffhandWeaponSpeed()
{
    if (this->offHandItem != nullptr) {
        if (Weapon *weapon = dynamic_cast<Weapon *>(this->offHandItem)) {
            return weapon->getWeaponSpeed();
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
    if (this->getPlayerClass().cls == this->getPlayerClass().Warrior) {
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
}

int32_t PlayerCharacter::getResource() const
{
    return resource;
}

void PlayerCharacter::setResource(const int32_t &value)
{
    resource = value;
    
    if (this->resource > this->resourceMax) {
        this->resource = this->resourceMax;
    }
    if (this->resource < 0) {
        this->resource = 0;
    }
}

int32_t PlayerCharacter::getResourceMax() const
{
    return resourceMax;
}

void PlayerCharacter::setResourceMax(const int32_t &value)
{
    resourceMax = value;
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
