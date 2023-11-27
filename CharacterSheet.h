#ifndef CHARACTERSHEET_H
#define CHARACTERSHEET_H

#include <QDialog>

namespace Ui {
class CharacterSheet;
}

class CharacterSheet : public QDialog
{
    Q_OBJECT
    
public:
    explicit CharacterSheet(QWidget *parent = nullptr);
    ~CharacterSheet();
    
private slots:
    void on_CharacterSheet_rejected();
    
private:
    Ui::CharacterSheet *ui;
};

#endif // CHARACTERSHEET_H
