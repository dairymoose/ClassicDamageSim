#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Weapon.h"
#include "DamageSimulation.h"
#include "PlayerClass.h"
#include <QInputDialog>

void MainWindow::simSetup()
{
    if (gal == nullptr) {
        gal = new GlobalAbilityList();
    }
    if (this->sim == nullptr) {
        this->sim = new DamageSimulation();
        sim->setGlobalAbilityList(this->gal);
    }
    if (this->PC == nullptr) {
        this->PC = new PlayerCharacter();
        PC->setName("Octorion");
        PC->setLevel(40);
        PC->setPlayerRace("Human");
        PC->setPlayerClass(PlayerClass::getClassByName("WARRIOR"));
        
        PC->setStrength(139);
        PC->setAgility(54);
        PC->setStamina(134);
        PC->setIntellect(26);
        PC->setSpirit(59);
        PC->setArmor(1310);
        //PC->addTalent("Two-Handed Weapon Specialization", 4); 
        //PC->addTalent("Anger Management", 1);
        //PC->addTalent("Impale", 2);
        
        //current spec
        PC->addTalent("Improved Charge", 2);
        PC->addTalent("Improved Rend", 3);
        PC->addTalent("Deep Wounds", 1);
        PC->addTalent("Improved Heroic Strike", 2);
        PC->addTalent("Improved Overpower", 1);
        
        //fury spec
        //PC->addTalent("Improved Battle Shout", 5);
        //PC->addTalent("Improved Execute", 1);
        
        //proposed spec
//        PC->addTalent("Improved Charge", 1);
//        PC->addTalent("Improved Rend", 3);
//        PC->addTalent("Deep Wounds", 1);
//        PC->addTalent("Improved Heroic Strike", 3);
//        PC->addTalent("Improved Overpower", 2);
        
        PC->getRunes().push_back(new Rune("Consumed by Rage"));
        //PC->getRunes().push_back(new Rune("Frenzied Assault"));
        //PC->getRunes().push_back(new Rune("Blood Frenzy"));
        Weapon *twoHander = new Weapon();
        twoHander->setMinDamage(92);
        twoHander->setMaxDamage(139);
        twoHander->setWeaponSpeed(3.7f);
        twoHander->setSlot(ItemSlot::TwoHand);
        PC->setMainHandItem(twoHander);
        sim->setPC(PC);
    }
    if (enemy == nullptr) {
        enemy = new Enemy();
        enemy->setName("Target Dummy");
        enemy->setHp(20000);
        enemy->setPhysicalDamageReduction(0.20f);
        enemy->setLevel(27);
        sim->getEnemyList().push_back(enemy);
        if (actionsDialog != nullptr) {
            actionsDialog->setEnemyListPtr(&sim->getEnemyList());
        }
    }
    static bool didPreBattleShout;
    didPreBattleShout = false;
    static bool didPreWildStrikes;
    didPreWildStrikes = false;
    if (availableActionsForClass == nullptr) {
        availableActionsForClass = new PriorityActionList();
        
        {
            PriorityAction *PA = new PriorityAction(gal->BattleShout, 4);
            PA->setInternalName("free_battle_shout");
            PA->setNameOverride("Start: Free Battle Shout");
            PA->setIgnoreGcd(true);
            PA->setIgnoreResourceCost(true);
            //bool didPreBattleShout = false;
            SET_PREDICATE_WITH_TEXT(PA, [&](PlayerCharacter *PC, float timestamp) {if (!didPreBattleShout){didPreBattleShout=true;return true;} return false;});
            //PA->setPredicate([&](PlayerCharacter *PC, float timestamp) {if (!didPreBattleShout){didPreBattleShout=true;return true;} return false;});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->PrintPlayerStats);
            PA->setInternalName("print_player_stats");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->PrintDps);
            PA->setInternalName("print_dps");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Charge, 1);
            PA->setInternalName("charge");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->MeleeMainhandAutoAttack);
            PA->setInternalName("mainhand_auto");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->MeleeOffhandAutoAttack);
            PA->setInternalName("offhand_auto");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Execute, 1);
            PA->setInternalName("execute");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Rend, 3);
            PA->setInternalName("rend");
            //PA->setPredicate([=](PlayerCharacter *PC, float timestamp){return !PC->getTarget()->hasDebuff(gal->Rend->getGrantedDebuff());});
            SET_PREDICATE_WITH_TEXT(PA, [=](PlayerCharacter *PC, float timestamp){return !PC->getTarget()->hasDebuff(gal->Rend->getGrantedDebuff());});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Bloodrage, 1);
            PA->setInternalName("bloodrage");
            SET_PREDICATE_WITH_TEXT(PA, [=](PlayerCharacter *PC, float timestamp){return !PC->getTarget()->hasBuff(gal->Bloodrage->getGrantedBuff());});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->MortalStrike, 1);
            PA->setInternalName("mortal_strike");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Whirlwind, 1);
            PA->setInternalName("whirlwind");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Slam, 2);
            PA->setInternalName("slam");
           // PA->setPredicate([](PlayerCharacter *PC, float timestamp){if (PC->getPriorityActionList()->getActionFromAbilityName("Main-hand attack")->getAbility()->getTimeSinceLastUsed(timestamp) < 0.20f) return true; return false;});
            SET_PREDICATE_WITH_TEXT(PA, [](PlayerCharacter *PC, float timestamp){if (PC->getPriorityActionList()->getActionFromAbilityName("Main-hand attack")->getAbility()->getTimeSinceLastUsed(timestamp) < 0.20f) return true; return false;});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->HeroicStrike, 4);
            PA->setInternalName("heroic_strike");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->HeroicStrike, 4);
            PA->setNameOverride("Heroic Strike @ 100 Rage");
            PA->setInternalName("heroic_strike_100_rage");
            SET_PREDICATE_WITH_TEXT(PA, [](PlayerCharacter *PC, float timestamp){if (PC->getResource() == PC->getResourceMax()) return true; return false;});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->QuickStrike, 1);
            PA->setInternalName("quick_strike");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->RagingBlow, 1);
            PA->setInternalName("raging_blow");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(nullptr, 1);
            PA->setNameOverride("Wait for 80 rage");
            PA->setInternalName("wait_for_rage");
            PA->setSkipToNextActionIfUseConditionFails(false);
            SET_PREDICATE_WITH_TEXT(PA, [](PlayerCharacter *PC, float timestamp){if (PC->getResource() >= 80) return true; return false;});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->QuickStrike, 1);
            PA->setNameOverride("Quick Strike (Follow-Up Only)");
            PA->setInternalName("qs_follow_up");
            SET_PREDICATE_WITH_TEXT(PA, [](PlayerCharacter *PC, float timestamp){if (PC->getLastUsedAction() == PC->getPriorityActionList()->getActionFromInternalName("quick_strike")) return true; return false;});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->FreeWildStrikes, 1);
            PA->setNameOverride("Start: Free Wild Strikes");
            PA->setInternalName("free_wild_strikes");
            PA->setIgnoreGcd(true);
            PA->setIgnoreResourceCost(true);
            SET_PREDICATE_WITH_TEXT(PA, [&](PlayerCharacter *PC, float timestamp) {if (!didPreWildStrikes){didPreWildStrikes=true;return true;} return false;});
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->Overpower, 1);
            PA->setInternalName("overpower");
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->FreeBlessingOfMight, 3);
            PA->setInternalName("blessing_of_might");
            PA->setIgnoreGcd(true);
            PA->setIgnoreResourceCost(true);
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->FreeMarkOfTheWild, 3);
            PA->setInternalName("mark_of_the_wild");
            PA->setIgnoreGcd(true);
            PA->setIgnoreResourceCost(true);
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->FreeBlessingOfKings, 3);
            PA->setInternalName("blessing_of_kings");
            PA->setIgnoreGcd(true);
            PA->setIgnoreResourceCost(true);
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
        {
            PriorityAction *PA = new PriorityAction(gal->FreeStrengthBonus, 1);
            PA->setInternalName("strength_bonus");
            PA->setIgnoreGcd(true);
            PA->setIgnoreResourceCost(true);
            availableActionsForClass->getPriorityActions().push_back(PA);
        }
    }
    if (PAL == nullptr) {
        PAL = new PriorityActionList();
        
        availableActionsForClass->removeExistingAction(PAL->addNewAction(availableActionsForClass->getActionFromInternalName("free_battle_shout")));
        availableActionsForClass->removeExistingAction(PAL->addNewAction(availableActionsForClass->getActionFromInternalName("mainhand_auto")));
        availableActionsForClass->removeExistingAction(PAL->addNewAction(availableActionsForClass->getActionFromInternalName("offhand_auto")));
        
        if (this->PC != nullptr) {
            this->PC->setPriorityActionList(PAL);
        }
    }
}

void MainWindow::simCleanup()
{
    delete sim;
    sim = nullptr;
    delete PC;
    PC = nullptr;
    delete enemy;
    enemy = nullptr;
    delete PAL;
    PAL = nullptr;
    delete availableActionsForClass;
    availableActionsForClass = nullptr;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    this->simSetup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_simButton_clicked()
{
    this->simSetup();
    
    PC->getCombatLog()->setEnabled(true);
    this->sim->reset();
    sim->resetIterationsData();
    sim->simulate(PAL);
    this->ui->simOutput->clear();
    for (auto& text : PC->getCombatLog()->getLogText()) {
        //this->ui->simOutput->appendPlainText(QString::fromStdString(text));
        this->ui->simOutput->appendHtml(QString::fromStdString(text));
    }
    this->ui->simOutput->appendHtml("<br>");
    std::stringstream ss;
    this->sim->printIterationSummary(ss);
    this->ui->simOutput->appendHtml(QString::fromStdString(ss.str()));
}

void MainWindow::on_actionsButton_clicked()
{
    this->simSetup();
    
    actionsDialog->setPC(this->PC);
    actionsDialog->clearAllPriorityActions();
    actionsDialog->addPriorityActionList(this->PAL);
    actionsDialog->addClassPriorityActionList(this->availableActionsForClass);
    //actionsDialog->setModal(true);
    actionsDialog->setWindowTitle("Player Priority Actions");
    actionsDialog->show();
}

void MainWindow::on_characterButton_clicked()
{
    this->simSetup();
    
    characterSheet->setPC(this->PC);
    characterSheet->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //characterSheet->setModal(true);
    characterSheet->show();
}

void MainWindow::on_talentsButton_clicked()
{
    this->simSetup();
    
    //talentsDialog->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    talentsDialog->show();
}

void MainWindow::on_iterateButton_clicked()
{
    QInputDialog qin;
    qin.exec();
    int value = qin.textValue().toInt();
    if (value > 0) {
        this->simSetup();
        this->ui->simOutput->clear();
        
        sim->resetIterationsData();
        PC->getCombatLog()->setEnabled(false);
        
        for (int i=0; i<value; ++i) {
            this->simSetup();
            this->sim->reset();
            sim->simulate(PAL);
        }
        std::stringstream ss;
//        for (auto& text : PC->getCombatLog()->getLogText()) {
//            this->ui->simOutput->appendHtml(QString::fromStdString(text));
//        }
        this->sim->printIterationSummary(ss);
        this->ui->simOutput->appendHtml(QString::fromStdString(ss.str()));
    }
}
