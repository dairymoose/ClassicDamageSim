#ifndef CHARACTERSHEET_H
#define CHARACTERSHEET_H

#include <QDialog>
#include "PlayerCharacter.h"

namespace Ui {
class CharacterSheet;
}

class CharacterSheet : public QDialog
{
    Q_OBJECT
    
public:
    explicit CharacterSheet(QWidget *parent = nullptr);
    ~CharacterSheet();
    
    PlayerCharacter *getPC() const;
    void setPC(PlayerCharacter *value);
    
private slots:
    void on_CharacterSheet_rejected();
    void on_importButton_clicked();
    
private:
    Ui::CharacterSheet *ui;
    PlayerCharacter *PC = nullptr;
    void setTokenValue(std::string text, int32_t tokenNo);
    void processItem(std::string text, int32_t slotNo);
};

#endif // CHARACTERSHEET_H
