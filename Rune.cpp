#include "Rune.h"

std::string Rune::getName() const
{
    return name;
}

void Rune::setName(const std::string &value)
{
    name = value;
}

Rune::Rune(std::string name)
{
    this->name = name;
}
