#ifndef WEAPON_H
#define WEAPON_H

#include "Item.h"

class Weapon : public Item
{
    int32_t minDamage;
    int32_t maxDamage;
    float weaponSpeed;
public:
    Weapon();
    int32_t getMinDamage() const;
    void setMinDamage(const int32_t &value);
    int32_t getMaxDamage() const;
    void setMaxDamage(const int32_t &value);
    float getWeaponSpeed() const;
    void setWeaponSpeed(float value);
};

#endif // WEAPON_H
