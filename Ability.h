#ifndef ABILITY_H
#define ABILITY_H

#include <cstdint>
#include <functional>
#include <vector>
#include "AbilityDamageType.h"
#include "Enemy.h"
#include "MeleeHitResult.h"

class Buff;
class PlayerCharacter;
class Ability
{
    std::string name;
    std::string tooltipText;
    AbilityDamageType abilityDamageType = AbilityDamageType::Other;
    bool ignoresArmor = false;
    int32_t rank = 1;
    std::function<void (float timestamp, PlayerCharacter *PC, int32_t abilityRank)> onCooldownTriggeredFunction = nullptr;
    std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank)> damageFunction = nullptr;
    std::function<float (PlayerCharacter *PC, int32_t abilityRank)> cooldownFunction = nullptr;
    std::function<float (PlayerCharacter *PC, int32_t abilityRank, float timestamp)> canUseFunction = nullptr;
    std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t damageDone, bool isCritical)> resourceGenerationFunction = nullptr;
    std::function<std::string (std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability)> onGetTooltip = nullptr;
    std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t initialCost)> onGetResourceCostModifier = nullptr;
    bool isGcdAbility = true;
    Buff *GrantedBuff = nullptr;
    Buff *GrantedDebuff = nullptr;
    
    bool replacesNextMelee = false;
    
    float lastUsedTimestamp = 0.0f;
    bool isCoolingDown = false;
    float capturedCooldownValue = 0.0f;
    
    int32_t resourceCost = 0;
    int32_t aoeMaxTargets = 0;
    float castTime = 0.0f;
    bool castedAbilityResetsAutoAttack = true;
    std::vector<int32_t> learnLevels;
    bool cannotBeBlockedOrDodgedOrParried = false;
    int32_t convertToPercentFormat(float f);
public:
    Ability(std::string name);
    
    void printMeleeAttackTable(MeleeAttackTable &mat, std::ostream& stream);
    void generateMeleeAttackTable(PlayerCharacter *PC, Enemy *target, MeleeAttackTable &mat);
    MeleeHitResult rollAttackTableForHitResult(MeleeAttackTable &mat);
    void execute(PlayerCharacter *PC, std::vector<Enemy *> &enemyList, float timestamp, bool ignoreGcd=false, bool ignoreResourceCost=false, bool shouldTriggerCooldown=true);
    void triggerAbilityDamageMeleeHooks(PlayerCharacter *PC, float timestamp);
    void reset();
    float getRemainingCooldown(float timestamp);
    float getTimeSinceLastUsed(float timestamp);
    float getCooldown(PlayerCharacter *PC);
    int32_t getDamage(PlayerCharacter *PC);
    bool isDamageAbility();
    std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank)> getDamageFunction() const;
    void setDamageFunction(const std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank)> &value);
    std::function<float (PlayerCharacter *PC, int32_t abilityRank)> getCooldownFunction() const;
    void setCooldownFunction(const std::function<float (PlayerCharacter *PC, int32_t abilityRank)> &value);
    bool getIsGcdAbility() const;
    void setIsGcdAbility(bool value);
    Buff *getGrantedBuff() const;
    void setGrantedBuff(Buff *value);
    Buff *getGrantedDebuff() const;
    void setGrantedDebuff(Buff *value);
    bool getIgnoresArmor() const;
    void setIgnoresArmor(bool value);
    bool hasEnoughResource(PlayerCharacter *PC, float timestamp);
    bool isOnCooldown(PlayerCharacter *PC, float timestamp);
    void triggerCooldown(PlayerCharacter *PC, float timestamp, bool actionIgnoresGcd);
    void partialRefundResourceCost(PlayerCharacter *PC, float timestamp);
    void triggerResourceCost(PlayerCharacter *PC, float timestamp);
    void triggerResourceGeneration(PlayerCharacter *PC, int32_t damageDone, bool isCritical, float timestamp);
    AbilityDamageType getAbilityDamageType() const;
    void setAbilityDamageType(const AbilityDamageType &value);
    std::string getName() const;
    void setName(const std::string &value);
    std::function<bool (PlayerCharacter *PC, int32_t abilityRank, float timestamp)> getCanUseFunction() const;
    void setCanUseFunction(const std::function<bool (PlayerCharacter *PC, int32_t rank, float timestamp)> &value);
    int32_t getRank() const;
    void setRank(const int32_t &value);
    std::function<void (float timestamp, PlayerCharacter *PC, int32_t abilityRank)> getOnCooldownTriggeredFunction() const;
    void setOnCooldownTriggeredFunction(const std::function<void (float timestamp, PlayerCharacter *PC, int32_t abilityRank)> &value);
    int32_t getResourceCost() const;
    void setResourceCost(const int32_t &value);
    int32_t getAoeMaxTargets() const;
    void setAoeMaxTargets(const int32_t &value);
    std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t damageDone, bool isCritical)> getResourceGenerationFunction() const;
    void setResourceGenerationFunction(const std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t damageDone, bool isCritical)> &value);
    float getCastTime() const;
    void setCastTime(float value);
    bool getCastedAbilityResetsAutoAttack() const;
    void setCastedAbilityResetsAutoAttack(bool value);
    std::vector<int32_t>& getLearnLevels();
    bool getReplacesNextMelee() const;
    void setReplacesNextMelee(bool value);
    std::string getTooltipText() const;
    void setTooltipText(const std::string &value);
    std::function<std::string (std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability)> getOnGetTooltip() const;
    void setOnGetTooltip(const std::function<std::string (std::string tooltipText, float timestamp, PlayerCharacter *PC, Ability *ability)> &value);
    std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t initialCost)> getOnGetResourceCostModifier() const;
    void setOnGetResourceCostModifier(const std::function<int32_t (PlayerCharacter *PC, int32_t abilityRank, int32_t initialCost)> &value);
    bool getCannotBeBlockedOrDodgedOrParried() const;
    void setCannotBeBlockedOrDodgedOrParried(bool value);
};

#endif // ABILITY_H
