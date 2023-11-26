#include "Weapon.h"

int32_t Weapon::getMinDamage() const
{
    return minDamage;
}

void Weapon::setMinDamage(const int32_t &value)
{
    minDamage = value;
}

int32_t Weapon::getMaxDamage() const
{
    return maxDamage;
}

void Weapon::setMaxDamage(const int32_t &value)
{
    maxDamage = value;
}

float Weapon::getWeaponSpeed() const
{
    return weaponSpeed;
}

void Weapon::setWeaponSpeed(float value)
{
    weaponSpeed = value;
}

Weapon::Weapon()
{
    ;
}
