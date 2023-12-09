#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GlobalAbilityList.h"
#include "ActionsDialog.h"
#include "CharacterSheet.h"
#include "DamageSimulation.h"
#include "PlayerCharacter.h"
#include "TalentsDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_simButton_clicked();    
    void on_actionsButton_clicked();
    
    void on_characterButton_clicked();
    
    void on_talentsButton_clicked();
    
    void on_iterateButton_clicked();
    
private:
    void simSetup();
    void simCleanup();
    Ui::MainWindow *ui;
    
    GlobalAbilityList *gal = nullptr;
    DamageSimulation *sim = nullptr;
    PlayerCharacter *PC = nullptr;
    Enemy *enemy = nullptr;
    PriorityActionList *PAL = nullptr;
    PriorityActionList *availableActionsForClass = nullptr;
    
    ActionsDialog *actionsDialog = new ActionsDialog();
    CharacterSheet *characterSheet = new CharacterSheet();
    TalentsDialog *talentsDialog = new TalentsDialog();
};

#endif // MAINWINDOW_H
