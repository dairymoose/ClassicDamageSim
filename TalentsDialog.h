#ifndef TALENTSDIALOG_H
#define TALENTSDIALOG_H

#include <QDialog>

namespace Ui {
class TalentsDialog;
}

class TalentsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TalentsDialog(QWidget *parent = nullptr);
    ~TalentsDialog();
    
private:
    Ui::TalentsDialog *ui;
};

#endif // TALENTSDIALOG_H
