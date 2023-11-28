#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

#include <cstdint>
#include "Item.h"
#include "PlayerClass.h"
#include "Combatant.h"
#include "Enemy.h"
#include "Talent.h"
class PriorityActionList;

class PlayerCharacter : public Combatant
{
    PlayerClass playerClass;
    std::string playerRace = "Human";
    
    Enemy *target = nullptr;
    PriorityActionList *priorityActionList;
    
    int32_t level;
    int32_t resource;
    int32_t resourceMax;
    
    std::vector<Talent *> talents;
    
    bool alwaysUseAverageDamage = false;
    bool canCrit = true;
    bool bakeCritIntoAverageDamage = false;
    
    float baseCritStrikeDamageMultipier = 2.0f;
    float critChance = 0.05f;
    
    int32_t strength;
    int32_t agility;
    int32_t stamina;
    int32_t intellect;
    int32_t spirit;
    int32_t armor;
    
    Item *headItem = nullptr;
    Item *neckItem = nullptr;
    Item *shoulderItem = nullptr;
    Item *backItem = nullptr;
    Item *chestItem = nullptr;
    Item *shirtItem = nullptr;
    Item *tabardItem = nullptr;
    Item *wristItem = nullptr;
    Item *handItem = nullptr;
    Item *beltItem = nullptr;
    Item *legItem = nullptr;
    Item *feetItem = nullptr;
    Item *finger1Item = nullptr;
    Item *finger2Item = nullptr;
    Item *trinket1Item = nullptr;
    Item *trinket2Item = nullptr;
    Item *mainHandItem = nullptr;
    Item *offHandItem = nullptr;
    Item *rangedItem = nullptr;
    Item *ammoItem = nullptr;
public:
    PlayerCharacter();
    ~PlayerCharacter();
    int32_t getArmor() const;
    void setArmor(const int32_t &value);
    int32_t getSpirit() const;
    void setSpirit(const int32_t &value);
    int32_t getIntellect() const;
    void setIntellect(const int32_t &value);
    int32_t getStamina() const;
    void setStamina(const int32_t &value);
    int32_t getAgility() const;
    void setAgility(const int32_t &value);
    int32_t getStrength() const;
    void setStrength(const int32_t &value);
    
    void disableAutoAttack();
    void enableAndResetAutoAttack(float timestamp);
    
    void addTalent(std::string name, int32_t rank);
    int32_t getTalentRank(std::string name);
    Talent *getTalent(std::string name);
    
    bool hasTwoHandedWeaponEquipped();
    
    int32_t applyMeleeApBuffs(int32_t AP);
    
    float getRandomFloat();
    int32_t maybeApplyCritDamage(Ability *source, int32_t value, bool& didCrit);
    bool isWhiteAttack(Ability *ability);
    float getCritStrikeDamageMultiplier(bool whiteAttack);
    
    float calculateGlobalDamageBonus();
    int32_t calculateSimulatedMainhandSwing();
    int32_t calculateSimulatedOffhandSwing();
    float calculateDpsFromAttackPower(int32_t attackPower);
    int32_t calculateMeleeAttackPower();
    int32_t calculatedWeaponMinDamage();
    int32_t calculatedWeaponMaxDamage();
    int32_t calculatedMainhandWeaponMinDamage();
    int32_t calculatedMainhandWeaponMaxDamage();
    int32_t calculatedMainhandWeaponAverageDamage();
    int32_t calculatedOffhandWeaponMinDamage();
    int32_t calculatedOffhandWeaponMaxDamage();
    int32_t calculatedOffhandWeaponAverageDamage();
    int32_t calculatedWeaponAverageDamage();
    float calculatedMainhandWeaponSpeed();
    float calculatedOffhandWeaponSpeed();
    float calculatedMainhandWeaponDps();
    Item *getMainHandItem() const;
    void setMainHandItem(Item *value);
    Item *getOffHandItem() const;
    void setOffHandItem(Item *value);
    PlayerClass &getPlayerClass();
    void setPlayerClass(const PlayerClass &value);
    void initResourceValue();
    int32_t getLevel() const;
    void setLevel(const int32_t &value);
    int32_t getResource() const;
    void setResource(const int32_t &value);
    int32_t getResourceMax() const;
    void setResourceMax(const int32_t &value);
    bool getAlwaysUseAverageDamage() const;
    void setAlwaysUseAverageDamage(bool value);
    Enemy *getTarget() const;
    void setTarget(Enemy *value);
    std::string getPlayerRace() const;
    void setPlayerRace(const std::string &value);
    PriorityActionList *getPriorityActionList() const;
    void setPriorityActionList(PriorityActionList *value);
    void resetAllTalentTimestamps();
};

#endif // PLAYERCHARACTER_H
