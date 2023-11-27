#include "SingularPriorityAction.h"
#include "ui_SingularPriorityAction.h"
#include "ActionsDialog.h"

SingularPriorityAction::SingularPriorityAction(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingularPriorityAction)
{
    ui->setupUi(this);
}

SingularPriorityAction::~SingularPriorityAction()
{
    delete ui;
}

void SingularPriorityAction::on_removeButton_clicked()
{
    ActionsDialog *actionsDialog = (ActionsDialog *) this->parentWidget()->parentWidget()->parentWidget()->parentWidget();
    if (actionsDialog->getIsClassActionDialog()) {
        ActionsDialog *classActionsDialog = actionsDialog;
        
        ActionsDialog *playerActionsDialog = classActionsDialog->getBaseActionsDialog();
        playerActionsDialog->transferPriorityActionFromExistingDialog(classActionsDialog, this->getPriorityAction());
    } else {
        ActionsDialog *playerActionsDialog = actionsDialog;
        
        playerActionsDialog->getClassActionsDialog()->transferPriorityActionFromExistingDialog(playerActionsDialog, this->getPriorityAction());
    }
}

PriorityAction *SingularPriorityAction::getPriorityAction() const
{
    return priorityAction;
}

void SingularPriorityAction::setPriorityAction(PriorityAction *value)
{
    priorityAction = value;
}

void SingularPriorityAction::on_upButton_clicked()
{
    ActionsDialog *actionsDialog = (ActionsDialog *) this->parentWidget()->parentWidget()->parentWidget()->parentWidget();
    actionsDialog->moveActionUp(this->getPriorityAction());
}

void SingularPriorityAction::on_downButton_clicked()
{
    ActionsDialog *actionsDialog = (ActionsDialog *) this->parentWidget()->parentWidget()->parentWidget()->parentWidget();
    actionsDialog->moveActionDown(this->getPriorityAction());
}
