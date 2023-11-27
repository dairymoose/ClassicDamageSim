#include "CharacterSheet.h"
#include "ui_CharacterSheet.h"

CharacterSheet::CharacterSheet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CharacterSheet)
{
    ui->setupUi(this);
}

CharacterSheet::~CharacterSheet()
{
    delete ui;
}

void CharacterSheet::on_CharacterSheet_rejected()
{
    this->hide();
}
