#ifndef ACTIONSDIALOG_H
#define ACTIONSDIALOG_H

#include <QDialog>
#include "PriorityActionList.h"
#include "SingularPriorityAction.h"

namespace Ui {
class ActionsDialog;
}

class ActionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ActionsDialog(QWidget *parent = nullptr);
    ~ActionsDialog();
    void clearAllPriorityActions();
    void transferPriorityActionFromExistingDialog(ActionsDialog *removeFrom, PriorityAction *action);
    void removePriorityAction(PriorityAction *toRemove);
    void addPriorityAction(PriorityAction *action);
    void addPriorityActionList(PriorityActionList *PAL);
    void handleConditionsButtonClicked();
    void addClassPriorityActionList(PriorityActionList *PAL);
    
    bool getIsClassActionDialog() const;
    void setIsClassActionDialog(bool value);
    
    ActionsDialog *getBaseActionsDialog() const;
    void setBaseActionsDialog(ActionsDialog *value);
    
    void showEvent(QShowEvent *showEvent);
    
    void moveActionUp(PriorityAction *action);
    void moveActionDown(PriorityAction *action);
    
    SingularPriorityAction *getActionWidgetFromAction(PriorityAction *action);
    
    PriorityActionList *getCurrentPriorityActionList() const;
    void setCurrentPriorityActionList(PriorityActionList *value);
    
    ActionsDialog *getClassActionsDialog() const;
    void setClassActionsDialog(ActionsDialog *value);
    
    PlayerCharacter *getPC() const;
    void setPC(PlayerCharacter *value);
    
private slots:
    void on_ActionsDialog_rejected();
    
    void on_addButton_clicked();
    
private:
    Ui::ActionsDialog *ui;

    std::vector<SingularPriorityAction *> priorityActionWidgets;
    
    void swapActionsByIndex(int32_t indexFrom, int32_t indexTo);
    void addPriorityActionUi(PriorityAction *action, std::string nameOverride, std::string name, std::string condition);
    ActionsDialog *baseActionsDialog = nullptr;
    bool isClassActionDialog = false;
    PriorityActionList *currentPriorityActionList = nullptr;
    PriorityActionList *classPriorityActionList = nullptr;
    ActionsDialog *classActionsDialog = nullptr;
    PlayerCharacter *PC = nullptr;
};

#endif // ACTIONSDIALOG_H
