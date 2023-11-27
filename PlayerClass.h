#ifndef PLAYERCLASS_H
#define PLAYERCLASS_H

#include <string>
#include <unordered_map>

#define STR(x) #x
#define PCLASS(cls) uint8_t cls=addNewClass(STR(cls))

class PlayerClass {
private:
    static std::unordered_map<uint8_t, std::string> classLookup;
    uint8_t pclass_inc;
    uint8_t addNewClass(std::string className) {
        for (auto&& pair : classLookup) {
            if (pair.second == className) {
                return pair.first;
            }
        }
        classLookup.insert({pclass_inc, className});
        return pclass_inc++;
    }
public:
    PlayerClass(){}
    ~PlayerClass() {
    }
    
    PCLASS(Warrior);
    PCLASS(Paladin);
    PCLASS(Shaman);
    PCLASS(Rogue);
    PCLASS(Mage);
    PCLASS(Warlock);
    PCLASS(Priest);
    PCLASS(Hunter);
    PCLASS(Druid);
    
    uint8_t cls;
    
    static PlayerClass getClassByName(std::string name) {
        PlayerClass pc;
        pc.cls = 0;
        for (auto& it : classLookup) {
            if (it.second == name) {
                pc.cls = it.first;
                break;
            }
        }
        return pc;
    }
    
    std::string getClassName() {
        auto&& it = classLookup.find(this->cls);
        if (it != classLookup.end()) {
            return it->second;
        }
        return "";
    }
};

#endif // PLAYERCLASS_H
