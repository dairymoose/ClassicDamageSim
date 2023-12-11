#include "ActionsDialog.h"
#include "ui_ActionsDialog.h"
#include <QLabel>
#include <algorithm>
#include <regex>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include "InstantTooltipLabel.h"

ActionsDialog::ActionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionsDialog)
{
    ui->setupUi(this);
}

ActionsDialog::~ActionsDialog()
{
    delete ui;
}

void ActionsDialog::clearAllPriorityActions()
{
    for (int i=0; i<priorityActionWidgets.size(); ++i) {
        delete priorityActionWidgets[i];
    }
    priorityActionWidgets.clear();
}

void ActionsDialog::transferPriorityActionFromExistingDialog(ActionsDialog *removeFrom, PriorityAction *action)
{
    if (action == nullptr)
        return;

    this->addPriorityAction(action);
    removeFrom->removePriorityAction(action);
    
    if (this->currentPriorityActionList != nullptr) {
        this->currentPriorityActionList->addNewAction(action);
    }
}

void ActionsDialog::removePriorityAction(PriorityAction *toRemove)
{
    if (this->currentPriorityActionList != nullptr) {
        this->currentPriorityActionList->removeExistingAction(toRemove);
    }
    for (int i=0; i<this->priorityActionWidgets.size(); ++i) {
        if (this->priorityActionWidgets[i]->getPriorityAction() == toRemove) {
            delete this->priorityActionWidgets[i];
            this->priorityActionWidgets.erase(this->priorityActionWidgets.begin() + i);
            return;
        }
    }
}

void ActionsDialog::addPriorityAction(PriorityAction *action) {
    std::string abilityName = "";
    if (action->getAbility() != nullptr) {
        abilityName = action->getAbility()->getName();
    }
    this->addPriorityActionUi(action, action->getNameOverride(), abilityName, action->getPredicateText());
}

void ActionsDialog::addPriorityActionByInternalName(std::string internalName)
{
    if (!this->isClassActionDialog && this->getCurrentPriorityActionList() != nullptr) {
        this->transferPriorityActionFromExistingDialog(this->classActionsDialog, this->getClassActionsDialog()->getCurrentPriorityActionList()->getActionFromInternalName(internalName));
    }
}

bool ActionsDialog::hasPriorityAction(PriorityAction *action)
{
    if (action == nullptr)
        return false;
    for (auto& widget : this->priorityActionWidgets) {
        if (widget->getPriorityAction() == action)
            return true;
    }
    return false;
}

bool ActionsDialog::hasPriorityActionByInternalName(std::string name)
{
    if (this->getCurrentPriorityActionList() != nullptr) {
        PriorityAction *action = this->getCurrentPriorityActionList()->getActionFromInternalName(name);
        return this->hasPriorityAction(action);
    }
    return false;
}

void ActionsDialog::clearAllActions()
{
    std::vector<PriorityAction *> actions;
    for (auto& action : this->getCurrentPriorityActionList()->getPriorityActions()) {
        actions.push_back(action);
    }
    for (auto& action : actions) {
        this->getClassActionsDialog()->transferPriorityActionFromExistingDialog(this, action);
    }
}

void ActionsDialog::addPriorityActionUi(PriorityAction *action, std::string nameOverride, std::string name, std::string condition)
{
    SingularPriorityAction *SPA = new SingularPriorityAction();
    SPA->setPriorityAction(action);
    QLabel *label = SPA->findChild<QLabel *>("label");
    if (label != nullptr) {
        if (nameOverride.length() > 0) {
            label->setText(QString::fromStdString(nameOverride));
        } else {
            label->setText(QString::fromStdString(name));
        }
    }
    std::string iconText = name;
    iconText = std::regex_replace(iconText, std::regex(" "), "_");
    std::transform(iconText.begin(), iconText.end(), iconText.begin(), tolower);
    std::string abilityIcon = ":/img/";
    abilityIcon += iconText;
    abilityIcon += ".jpg";
    QLabel *abilityIconLabel = SPA->findChild<QLabel *>("abilityIcon");
    if (abilityIconLabel != nullptr) {
        InstantTooltipLabel *itl = (InstantTooltipLabel *) abilityIconLabel;
        itl->setEnemyListPtr(this->getEnemyListPtr());
        itl->setAbility(action->getAbility());
        itl->setPC(this->PC);
        abilityIconLabel->setPixmap(QPixmap(QString::fromStdString(abilityIcon)));
    }
    QPushButton *cond = SPA->findChild<QPushButton *>("conditionButton");
    if (cond != nullptr) {
        if (condition.length() > 0) {
            cond->setEnabled(true);
            cond->setProperty("predicate", QString::fromStdString(condition));
            connect(cond, &QPushButton::clicked, this, &ActionsDialog::handleConditionsButtonClicked);
        } else {
            cond->setHidden(true);
        }
    }
    if (this->isClassActionDialog) {
        QPushButton *upBtn = SPA->findChild<QPushButton *>("upButton");
        QPushButton *downBtn = SPA->findChild<QPushButton *>("downButton");
        QPushButton *removeBtn = SPA->findChild<QPushButton *>("removeButton");

        if (upBtn != nullptr) {
            upBtn->hide();
        }
        if (downBtn != nullptr) {
            downBtn->hide();
        }
        if (removeBtn != nullptr) {
            removeBtn->setIcon(QIcon(QPixmap(":/img/green_plus.png")));
            removeBtn->setToolTip("Add class action to player action list");
        }
    }
    this->priorityActionWidgets.push_back(SPA);
    this->ui->scrollAreaWidgetContents->layout()->addWidget(SPA);
}

ActionsDialog *ActionsDialog::getClassActionsDialog() const
{
    return classActionsDialog;
}

void ActionsDialog::setClassActionsDialog(ActionsDialog *value)
{
    classActionsDialog = value;
}

PlayerCharacter *ActionsDialog::getPC() const
{
    return PC;
}

void ActionsDialog::setPC(PlayerCharacter *value)
{
    PC = value;
    
    if (this->classActionsDialog != nullptr) {
        classActionsDialog->setPC(this->getPC());
    }
}

PriorityActionList *ActionsDialog::getCurrentPriorityActionList() const
{
    return currentPriorityActionList;
}

void ActionsDialog::setCurrentPriorityActionList(PriorityActionList *value)
{
    currentPriorityActionList = value;
}

void ActionsDialog::addPriorityActionList(PriorityActionList *PAL)
{
    if (PAL == nullptr) {
        return;
    }
    this->currentPriorityActionList = PAL;
    for (int i=0; i<PAL->getPriorityActions().size(); ++i) {
        this->addPriorityAction(PAL->getPriorityActions()[i]);
    }
    
    if (this->isClassActionDialog) {
        QDialogButtonBox *buttonBox = this->findChild<QDialogButtonBox *>("buttonBox");
        QPushButton *addBtn = this->findChild<QPushButton *>("addButton");
        QPushButton *saveBtn = this->findChild<QPushButton *>("saveButton");
        QPushButton *loadBtn = this->findChild<QPushButton *>("loadButton");
        
        if (buttonBox != nullptr) {
            buttonBox->hide();
        }
        if (addBtn != nullptr) {
            addBtn->setText("Close");
        }
        if (saveBtn != nullptr) {
            saveBtn->hide();
        }
        if (loadBtn != nullptr) {
            loadBtn->hide();
        }
    }
}

void ActionsDialog::handleConditionsButtonClicked()
{
    QPushButton *btn = (QPushButton *) this->sender();
    if (btn != nullptr) {
        QString pred = btn->property("predicate").toString();
        QMessageBox qmsg;
        qmsg.setText(pred);
        qmsg.exec();
    }
}

void ActionsDialog::addClassPriorityActionList(PriorityActionList *PAL)
{
    this->classPriorityActionList = PAL;
}

void ActionsDialog::on_ActionsDialog_rejected()
{
    this->hide();
}

void ActionsDialog::on_addButton_clicked()
{
    if (this->isClassActionDialog) {
        this->hide();
    } else {
        classActionsDialog->show();
        QPoint pt = this->pos();
        pt.setX(pt.x() + 80);
        pt.setY(pt.y() - 20);
        classActionsDialog->move(pt);
    }
}

std::vector<Enemy *> *ActionsDialog::getEnemyListPtr() const
{
    return enemyListPtr;
}

void ActionsDialog::setEnemyListPtr(std::vector<Enemy *> *value)
{
    enemyListPtr = value;
}

ActionsDialog *ActionsDialog::getBaseActionsDialog() const
{
    return baseActionsDialog;
}

void ActionsDialog::setBaseActionsDialog(ActionsDialog *value)
{
    baseActionsDialog = value;
}

void ActionsDialog::showEvent(QShowEvent *showEvent)
{
    QWidget::showEvent(showEvent);
    
    if (classActionsDialog == nullptr && this->classPriorityActionList != nullptr) {
        classActionsDialog = new ActionsDialog();
        classActionsDialog->setEnemyListPtr(this->getEnemyListPtr());
        classActionsDialog->setPC(this->getPC());
        classActionsDialog->setIsClassActionDialog(true);
        classActionsDialog->setBaseActionsDialog(this);
        classActionsDialog->clearAllPriorityActions();
        classActionsDialog->addPriorityActionList(this->classPriorityActionList);
        classActionsDialog->setWindowTitle("Class Actions");
    }
}

void ActionsDialog::swapActionsByIndex(int32_t indexFrom, int32_t indexTo)
{
    PriorityAction *action = this->currentPriorityActionList->getPriorityActions()[indexFrom];
    
    PriorityAction *existing = this->currentPriorityActionList->getPriorityActions()[indexTo];
    this->currentPriorityActionList->getPriorityActions()[indexTo] = action;
    this->currentPriorityActionList->getPriorityActions()[indexFrom] = existing;
    SingularPriorityAction *child = this->getActionWidgetFromAction(action);
    if (child != nullptr) {
        this->ui->scrollAreaWidgetContents->layout()->removeWidget(child);
        ((QVBoxLayout *)this->ui->scrollAreaWidgetContents->layout())->insertWidget(indexTo, child);
        //this->ui->scrollArea->ensureVisible(child->pos().x(), child->pos().y());
    }
}

void ActionsDialog::moveActionUp(PriorityAction *action)
{
    int32_t idx = -1;
    for (int i=0; i<this->currentPriorityActionList->getPriorityActions().size(); ++i) {
        if (this->currentPriorityActionList->getPriorityActions()[i] == action) {
            idx = i;
        }
    }
    if (idx != -1) {
        if (idx != 0) {
            int32_t destIdx = idx - 1;
            
            this->swapActionsByIndex(idx, destIdx);
        }
    }
}

void ActionsDialog::moveActionDown(PriorityAction *action)
{
    int32_t idx = -1;
    for (int i=0; i<this->currentPriorityActionList->getPriorityActions().size(); ++i) {
        if (this->currentPriorityActionList->getPriorityActions()[i] == action) {
            idx = i;
        }
    }
    if (idx != -1) {
        int32_t lastIndex = this->currentPriorityActionList->getPriorityActions().size() - 1;
        if (idx < lastIndex) {
            int32_t destIdx = idx + 1;
            
            this->swapActionsByIndex(idx, destIdx);
        }
    }
}

SingularPriorityAction *ActionsDialog::getActionWidgetFromAction(PriorityAction *action)
{
    for (int i=0; i<this->priorityActionWidgets.size(); ++i) {
        if (this->priorityActionWidgets[i]->getPriorityAction() == action) {
            return this->priorityActionWidgets[i];
        }
    }
    return nullptr;
}

bool ActionsDialog::getIsClassActionDialog() const
{
    return isClassActionDialog;
}

void ActionsDialog::setIsClassActionDialog(bool value)
{
    isClassActionDialog = value;
}

void ActionsDialog::on_saveButton_clicked()
{
    QFileDialog qfd;
    qfd.setDefaultSuffix(".txt");
    QString result = qfd.getSaveFileName(this, QString(), QString(), "*.txt");
    if (!result.isEmpty()) {
        QFile file(result);
        file.open(QIODevice::WriteOnly);
        for (auto& action : this->getCurrentPriorityActionList()->getPriorityActions()) {
            file.write(action->getInternalName().c_str());
            file.putChar('\n');
        }
        file.close();
    }
}

void ActionsDialog::on_loadButton_clicked()
{
    QFileDialog qfd;
    qfd.setDefaultSuffix(".txt");
    QString result = qfd.getOpenFileName(this, QString(), QString(), "*.txt");
    if (!result.isEmpty()) {
        this->clearAllActions();
        
        QFile file(result);
        file.open(QIODevice::ReadOnly);
        QByteArray bytes = file.readAll();
        QString text(bytes);
        QStringList tokens = text.split("\n");
        for (QString token : tokens) {
            if (!token.isEmpty()) {
                std::string internalName = token.toStdString();
                if (!this->hasPriorityActionByInternalName(internalName)) {
                    this->addPriorityActionByInternalName(internalName);
                }
            }
        }
        file.close();
    }
}
