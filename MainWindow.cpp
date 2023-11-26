#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Weapon.h"
#include "DamageSimulation.h"
#include "PlayerClass.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_simButton_clicked()
{
    if (gal == nullptr) {
        gal = new GlobalAbilityList();
    }
    DamageSimulation sim;
    PlayerCharacter PC;
    PC.setName("Octorion");
    PC.setLevel(40);
    PC.setPlayerRace("Human");
    PC.setPlayerClass(PlayerClass::getClassByName("Warrior"));
    
    PC.setStrength(139);
    PC.setAgility(54);
    PC.setStamina(134);
    PC.setIntellect(26);
    PC.setSpirit(59);
    PC.setArmor(1310);
    PC.setAlwaysUseAverageDamage(true);
    PC.addTalent("Two-Handed Weapon Specialization", 4); 
    PC.addTalent("Improved Charge", 2);
    PC.addTalent("Anger Management", 1);
    PC.addTalent("Improved Rend", 3);
    Weapon *twoHander = new Weapon();
    twoHander->setMinDamage(92);
    twoHander->setMaxDamage(139);
    twoHander->setWeaponSpeed(3.7f);
    twoHander->setSlot(ItemSlot::TwoHand);
    PC.setMainHandItem(twoHander);
    sim.setPC(&PC);
    Enemy *enemy = new Enemy();
    enemy->setName("Target Dummy");
    enemy->setHp(3000);
    enemy->setPhysicalDamageReduction(0.25f);
    sim.getEnemyList().push_back(enemy);
    PriorityActionList PAL;
    {
        PriorityAction *PA = new PriorityAction(gal->BattleShout, 4);
        PA->setIgnoreGcd(true);
        PA->setIgnoreResourceCost(true);
        bool didPreBattleShout = false;
        PA->setPredicate([&](PlayerCharacter *PC, float timestamp){if (!didPreBattleShout){didPreBattleShout=true;return true;} return false;});
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->PrintPlayerStats);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->PrintDps);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->Charge, 2);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->MeleeMainhandAutoAttack);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->MeleeOffhandAutoAttack);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->Execute, 3);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->Rend, 5);
        PA->setPredicate([&](PlayerCharacter *PC, float timestamp){return !PC->getTarget()->hasDebuff(gal->Rend->getGrantedDebuff());});
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->MortalStrike, 1);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->Whirlwind, 1);
        PAL.getPriorityActions().push_back(PA);
    }
    {
        PriorityAction *PA = new PriorityAction(gal->Slam, 2);
        PA->setPredicate([](PlayerCharacter *PC, float timestamp){if (PC->getPriorityActionList()->getActionFromAbilityName("Main-hand attack")->getAbility()->getTimeSinceLastUsed(timestamp) < 0.20f) return true; return false;});
        //PAL.getPriorityActions().push_back(PA);
    }
    sim.setGlobalAbilityList(this->gal);
    sim.simulate(&PAL);
    this->ui->simOutput->clear();
    for (auto& text : PC.getCombatLog()->getLogText()) {
        //this->ui->simOutput->appendPlainText(QString::fromStdString(text));
        this->ui->simOutput->appendHtml(QString::fromStdString(text));
    }
}
