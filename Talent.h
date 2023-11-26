#ifndef TALENT_H
#define TALENT_H

#include <string>

class Talent
{
    std::string name;
    int32_t rank;
    float lastUsedTimestamp = 0.0f;
public:
    Talent();
    Talent(std::string name, int32_t rank);
    std::string getName() const;
    void setName(const std::string &value);
    int32_t getRank() const;
    void setRank(const int32_t &value);
    float getLastUsedTimestamp() const;
    void setLastUsedTimestamp(float value);
};

#endif // TALENT_H
