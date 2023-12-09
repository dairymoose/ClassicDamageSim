#ifndef RUNE_H
#define RUNE_H

#include <string>

class Rune
{
    std::string name;
public:
    Rune(std::string name);
    std::string getName() const;
    void setName(const std::string &value);
};

#endif // RUNE_H
