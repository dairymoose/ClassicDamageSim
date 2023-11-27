#ifndef SINGULARPRIORITYACTION_H
#define SINGULARPRIORITYACTION_H

#include <QWidget>
#include "PriorityAction.h"

namespace Ui {
class SingularPriorityAction;
}

class SingularPriorityAction : public QWidget
{
    Q_OBJECT
    
public:
    explicit SingularPriorityAction(QWidget *parent = nullptr);
    ~SingularPriorityAction();
    
    PriorityAction *getPriorityAction() const;
    void setPriorityAction(PriorityAction *value);
    
private slots:
    void on_removeButton_clicked();
    
    void on_upButton_clicked();
    
    void on_downButton_clicked();
    
private:
    Ui::SingularPriorityAction *ui;
    PriorityAction *priorityAction = nullptr;
};

#endif // SINGULARPRIORITYACTION_H
