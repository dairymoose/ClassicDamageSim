#include "InstantTooltipLabel.h"
#include <QEnterEvent>
#include <QTextEdit>
#include <sstream>

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

void InstantTooltipLabel::enterEvent(QEvent *event)
{
    if (this->ability != nullptr && this->PC != nullptr) {
        QTextEdit *textEdit = this->tooltip->findChild<QTextEdit *>("textEdit");
        if (textEdit) {
            std::stringstream style;
            style<<"font-family: Verdana,\"Open Sans\",Arial,\"Helvetica Neue\",Helvetica,sans-serif;\n";
            style<<"color: white;\n";
            style<<"padding: 5px;\n";
            int32_t tooltipHeightBoost = 0;
            int rankSpaceCount = 35 - (this->ability->getName().length() * 1.40f);
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
            ss<<"<font size=\"5\">"<<this->ability->getName()<<rankSpaces<<"<font color=\"grey\">Rank "<<this->getAbility()->getRank()<<"</font></font><br>";
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
            if (this->ability->getCastTime() == 0.0f) {
                ss<<"<font size=\"3\">"<<"Instant"<<instantSpaces<<cdText<<"</font><br>";
            } else {
                ss<<"<font size=\"3\">"<<this->getAbility()->getCastTime()<<" sec cast"<<castTimeSpaces<<cdText<<"</font><br>";
            }
            ss<<"<br>";
            std::string tooltipText = "";
            if (this->getAbility()->getOnGetTooltip() != nullptr) {
                tooltipHeightBoost = 18;
                tooltipText = this->getAbility()->getOnGetTooltip()(this->getAbility()->getTooltipText(), 0.0f, this->getPC(), this->getAbility());
                ss<<"<font size=\"3\" color=\"yellow\">"<<tooltipText<<"</font><br>";
            }
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
