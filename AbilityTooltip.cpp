#include "AbilityTooltip.h"
#include "ui_AbilityTooltip.h"
#include <QEnterEvent>

AbilityTooltip::AbilityTooltip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AbilityTooltip)
{
    ui->setupUi(this);
    
    setWindowFlags(Qt::ToolTip);
    //setAttribute(Qt::WA_TransparentForMouseEvents);
    //setFocusPolicy(Qt::NoFocus);
    this->setMouseTracking(true);
}

void AbilityTooltip::leaveEvent(QEvent *event)
{
    this->hide();
}

AbilityTooltip::~AbilityTooltip()
{
    delete ui;
}
