#ifndef ABILITYTOOLTIP_H
#define ABILITYTOOLTIP_H

#include <QWidget>

namespace Ui {
class AbilityTooltip;
}

class AbilityTooltip : public QWidget
{
    Q_OBJECT
    
public:
    explicit AbilityTooltip(QWidget *parent = nullptr);
    ~AbilityTooltip();
    
    void leaveEvent(QEvent *event);
    
private:
    Ui::AbilityTooltip *ui;
};

#endif // ABILITYTOOLTIP_H
