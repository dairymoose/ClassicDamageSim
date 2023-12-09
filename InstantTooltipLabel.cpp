#include "InstantTooltipLabel.h"
#include <QEnterEvent>
#include <QTextEdit>
#include <sstream>
#include "DamageSimulation.h"
#include "GlobalAbilityList.h"

Ability *InstantTooltipLabel::getAbility() const
{
    return ability;
}

void InstantTooltipLabel::setAbility(Ability *value)
{
    ability = value;
}

PlayerCharacter *InstantTooltipLabel::getPC() const
{
    return PC;
}

void InstantTooltipLabel::setPC(PlayerCharacter *value)
{
    PC = value;
}

std::vector<Enemy *> *InstantTooltipLabel::getEnemyListPtr() const
{
    return enemyListPtr;
}

void InstantTooltipLabel::setEnemyListPtr(std::vector<Enemy *> *value)
{
    enemyListPtr = value;
}

void InstantTooltipLabel::resetPcBuffsAndResources()
{
    PC->initResourceValue();
    PC->setCurrentHp(PC->getMaxHp());
    PC->setDamageDone(0);
    PC->clearAllBuffsAndDebuffsAndFreeMemory();
    if (PC->getTarget() != nullptr) {
        PC->getTarget()->clearAllBuffsAndDebuffsAndFreeMemory();
        PC->getTarget()->setCurrentHp(PC->getTarget()->getMaxHp());
    }
}

void InstantTooltipLabel::enterEvent(QEvent *event)
{
    if (this->ability != nullptr && this->PC != nullptr) {
        QTextEdit *textEdit = this->tooltip->findChild<QTextEdit *>("textEdit");
        if (textEdit && enemyListPtr != nullptr) {
            std::stringstream style;
            style<<"font-family: Verdana,\"Open Sans\",Arial,\"Helvetica Neue\",Helvetica,sans-serif;\n";
            style<<"color: white;\n";
            style<<"padding: 5px;\n";
            int32_t tooltipHeightBoost = 0;
            int rankSpaceCount = 34 - (this->ability->getName().length() * 1.40f);
            int instantSpaceCount = 42 - std::string("Instant").length();
            int castTimeSpaceCount = 39 - std::string("2.5 sec cast").length();
            std::string rankSpaces = "";
            std::string instantSpaces = "";
            std::string castTimeSpaces = "";
            for (int i=0; i<rankSpaceCount; ++i) {
                rankSpaces += "&nbsp;";
            }
            for (int i=0; i<instantSpaceCount; ++i) {
                instantSpaces += "&nbsp;";
            }
            for (int i=0; i<castTimeSpaceCount; ++i) {
                castTimeSpaces += "&nbsp;";
            }
            std::stringstream ss;
            int32_t rank = 0;
            if (this->getAbility() != nullptr) {
                rank = this->getAbility()->getRank();
            }
            ss<<"<font size=\"5\">"<<this->ability->getName()<<rankSpaces<<"<font color=\"grey\">Rank "<<rank<<"</font></font><br>";
            if (this->ability->getResourceCost() > 0) {
                ss<<"<font size=\"3\">"<<this->ability->getResourceCost()<<" Rage"<<"</font><br>";
            }
            std::string cdText = "";
            float cd = this->ability->getCooldown(PC);
            if (cd > 0.0f) {
                std::stringstream out;
                out<<cd<<" sec cooldown";
                cdText = out.str();
            }
            if (this->ability != nullptr) {
                if (this->ability->getCastTime() == 0.0f) {
                    ss<<"<font size=\"3\">"<<"Instant"<<instantSpaces<<cdText<<"</font><br>";
                } else {
                    ss<<"<font size=\"3\">"<<this->getAbility()->getCastTime()<<" sec cast"<<castTimeSpaces<<cdText<<"</font><br>";
                }
            }
            ss<<"<br>";
            std::string tooltipText = "";
            if (this->getAbility() != nullptr && this->getAbility()->getOnGetTooltip() != nullptr) {
                tooltipHeightBoost += 18;
                tooltipText = this->getAbility()->getOnGetTooltip()(this->getAbility()->getTooltipText(), 0.0f, this->getPC(), this->getAbility());
                ss<<"<font size=\"3\" color=\"yellow\">"<<tooltipText<<"</font><br>";
            }
            bool avg = PC->getAlwaysUseAverageDamage();
            bool canCrit = PC->getCanCrit();
            bool bakeCrit = PC->getBakeCritIntoAverageDamage();
            bool wildStrikesBake = PC->getBakeWildStrikesIntoAverageDamage();
            PC->setAlwaysUseAverageDamage(true);
            PC->setCanCrit(false);
            PC->setBakeCritIntoAverageDamage(false);
            PC->setBakeWildStrikesIntoAverageDamage(false);
            this->resetPcBuffsAndResources();
            
            if (this->getAbility() != nullptr) {
                std::vector<Enemy *> enemyList;
                enemyList.push_back(PC->getTarget());
                this->getAbility()->execute(PC, *enemyListPtr, 0, true, true);
                //int32_t directDamage = DamageSimulation::getDamageForAbility(this->getAbility(), PC);
                int32_t directDamage = PC->getDamageDone();
                if (this->getAbility()->getReplacesNextMelee()) {
                    directDamage = this->getAbility()->getDamage(PC);
                    directDamage -= PC->getPriorityActionList()->getMainHandAutoAttackAction()->getAbility()->getDamage(PC);
                    directDamage = directDamage * (1.0f - PC->getTarget()->getPhysicalDamageReduction());
                }
                int32_t dotDamage = DamageSimulation::totalDotDamageForAbility(this->getAbility(), PC);
                
                //int32_t dotDamage = 0;
                bool hasDamage = directDamage > 0 || dotDamage > 0;
                int32_t damageValue = 0;
                damageValue += directDamage;
                damageValue += dotDamage;
                if (hasDamage) {
                    PC->setCanCrit(true);
                    PC->setBakeCritIntoAverageDamage(true);
                    PC->setBakeWildStrikesIntoAverageDamage(true);
                    bool didItCrit;
                    
                    this->resetPcBuffsAndResources();
                    this->getAbility()->execute(PC, *enemyListPtr, 0, true, true);
                    //int32_t directDamage = DamageSimulation::getDamageForAbility(this->getAbility(), PC);
                    //int32_t damageValueWithCrit = PC->maybeApplyCritDamage(this->getAbility(), DamageSimulation::getDamageForAbility(this->getAbility(), PC), didItCrit) + dotDamage;
                    int32_t damageValueWithCrit = PC->getDamageDone();
                    if (this->getAbility()->getReplacesNextMelee()) {
                        damageValueWithCrit = this->getAbility()->getDamage(PC);
                        damageValueWithCrit -= PC->getPriorityActionList()->getMainHandAutoAttackAction()->getAbility()->getDamage(PC);
                        damageValueWithCrit = damageValueWithCrit * (1.0f - PC->getTarget()->getPhysicalDamageReduction());
                    }
                    
                    this->resetPcBuffsAndResources();
                    AppliedBuff *appliedWildStrikes = PC->applyBuff(PC, 0, GlobalAbilityList::activeList->WildStrikes, true);
                    this->getAbility()->execute(PC, *enemyListPtr, 0, true, true);
                    //int32_t directDamage = DamageSimulation::getDamageForAbility(this->getAbility(), PC);
                    //int32_t damageValueWithCritWithWildStrikes = PC->maybeApplyCritDamage(this->getAbility(), DamageSimulation::getDamageForAbility(this->getAbility(), PC), didItCrit) + dotDamage;
                    int32_t damageValueWithCritWithWildStrikes = PC->getDamageDone();
                    if (this->getAbility()->getReplacesNextMelee()) {
                        damageValueWithCritWithWildStrikes = this->getAbility()->getDamage(PC);
                        damageValueWithCritWithWildStrikes -= PC->getPriorityActionList()->getMainHandAutoAttackAction()->getAbility()->getDamage(PC);
                        damageValueWithCritWithWildStrikes = damageValueWithCritWithWildStrikes * (1.0f - PC->getTarget()->getPhysicalDamageReduction());
                    }
                    
                    
                    PC->removeBuff(appliedWildStrikes, 0);
                    
                    tooltipHeightBoost += 15;
                    ss<<"<br>";
                    
                    tooltipHeightBoost += 15;
                    ss<<"<font size=\"3\" color=\"magenta\">Target Direct Damage: "<<directDamage<<"</font><br>";
                    
                    if (this->getAbility()->getResourceCost() > 0) {
                        tooltipHeightBoost += 15;
                        float DPR = (float)damageValue/this->getAbility()->getResourceCost();
                        ss<<"<font size=\"3\" color=\"magenta\">"<<DPR<<" DPR"<<"</font><br>";
                        
                        if (directDamage > 0) {
                            tooltipHeightBoost += 15;
                            float DPRwithCrit = (float)damageValueWithCrit/this->getAbility()->getResourceCost();
                            ss<<"<font size=\"3\" color=\"magenta\">"<<DPRwithCrit<<" Crit-Wtd-Avg DPR"<<"</font><br>";
                            
                            tooltipHeightBoost += 15;
                            float DPRwithCritWithWildStrikes = (float)damageValueWithCritWithWildStrikes/this->getAbility()->getResourceCost();
                            ss<<"<font size=\"3\" color=\"magenta\">"<<DPRwithCritWithWildStrikes<<" Crit-Wtd-Avg-WildStrikes"<<"</font><br>";
                        }
                    }
                    float cd = this->getAbility()->getCooldown(PC);
                    if (cd > 0.0f) {
                        tooltipHeightBoost += 15;
                        float DPS = (float)damageValue/cd;
                        ss<<"<font size=\"3\" color=\"magenta\">"<<DPS<<" DPS"<<"</font><br>";
                    } else {
                        tooltipHeightBoost += 15;
                        float DPS = (float)directDamage/PC->getGcdDuration();
                        ss<<"<font size=\"3\" color=\"magenta\">Every-GCD DPS: "<<DPS<<"</font><br>";
                    }
                    if (PC->isWhiteAttack(this->getAbility())) {
                        
                    }
                }
            }

            PC->setAlwaysUseAverageDamage(avg);
            PC->setCanCrit(canCrit);
            PC->setBakeCritIntoAverageDamage(bakeCrit);
            PC->setBakeWildStrikesIntoAverageDamage(wildStrikesBake);
            
            this->tooltip->resize(300, 70 + (tooltipHeightBoost + tooltipText.length()*0.28f));
            textEdit->setHtml(QString::fromStdString(ss.str()));
            textEdit->setStyleSheet(QString::fromStdString(style.str()));
        }
    } else {
        this->tooltip->resize(400, 200);
    }
    
    QEnterEvent *ee = (QEnterEvent *) event;
    this->tooltip->move(ee->globalPos());
    this->tooltip->show();
}

void InstantTooltipLabel::leaveEvent(QEvent *event)
{
    QEnterEvent *ee = (QEnterEvent *) event;
    if (!this->tooltip->rect().contains(this->tooltip->mapFromGlobal(QCursor::pos()))) {
        this->tooltip->hide();
    }
}
