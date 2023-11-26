#include "Talent.h"

std::string Talent::getName() const
{
    return name;
}

void Talent::setName(const std::string &value)
{
    name = value;
}

int32_t Talent::getRank() const
{
    return rank;
}

void Talent::setRank(const int32_t &value)
{
    rank = value;
}

float Talent::getLastUsedTimestamp() const
{
    return lastUsedTimestamp;
}

void Talent::setLastUsedTimestamp(float value)
{
    lastUsedTimestamp = value;
}

Talent::Talent()
{
    
}

Talent::Talent(std::string name, int32_t rank)
{
    this->name = name;
    this->rank = rank;
}
