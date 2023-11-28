#include "TalentsDialog.h"
#include "ui_TalentsDialog.h"

TalentsDialog::TalentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TalentsDialog)
{
    ui->setupUi(this);
}

TalentsDialog::~TalentsDialog()
{
    delete ui;
}
