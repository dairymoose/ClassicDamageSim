#ifndef INSTANTTOOLTIPLABEL_H
#define INSTANTTOOLTIPLABEL_H

#include <QLabel>
#include "AbilityTooltip.h"
#include "Ability.h"
#include "PlayerCharacter.h"

class InstantTooltipLabel : public QLabel
{
    AbilityTooltip *tooltip = nullptr;
    Ability *ability = nullptr;
    PlayerCharacter *PC = nullptr;
    std::vector<Enemy *> *enemyListPtr = nullptr;
    void resetPcBuffsAndResources();
public:
    InstantTooltipLabel(QWidget *parent)
        : QLabel(parent)
    {
        this->tooltip = new AbilityTooltip();
        this->tooltip->hide();
        this->setMouseTracking(true);
    }
    
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    Ability *getAbility() const;
    void setAbility(Ability *value);
    PlayerCharacter *getPC() const;
    void setPC(PlayerCharacter *value);
    std::vector<Enemy *> *getEnemyListPtr() const;
    void setEnemyListPtr(std::vector<Enemy *> *value);
};

#endif // INSTANTTOOLTIPLABEL_H
