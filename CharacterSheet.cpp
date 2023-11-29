#include "CharacterSheet.h"
#include "ui_CharacterSheet.h"
#include <QInputDialog>
#include <regex>
#include <sstream>
#include "Weapon.h"

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
#include <iostream>
void CharacterSheet::on_importButton_clicked()
{
    QInputDialog qin;
    qin.exec();
    std::string data = qin.textValue().toStdString();
    if (data.rfind("CDS", 0) == 0) {
        int colon = data.find(":");
        if (colon != std::string::npos) {
            std::string ver = data.substr(3, colon-3);
            if (ver == "1" && data.length() > colon + 1) {
                //CDS1:1/60/100/23/20/22/20/21/102/0/5.0799994468689/29/0/0/0/0/0/0/0/0/
                if (PC != nullptr) { 
                    int32_t tokenNo = 0;
                    
                    std::regex r("/");
                    std::sregex_token_iterator tok(data.begin()+colon+1, data.end(), r, -1);
                    std::sregex_token_iterator end;
                    for (tok; tok != end; ++tok) {
                        this->setTokenValue(*tok, tokenNo);
                        ++tokenNo;
                    }
                }
            }
        }
    }
}

void CharacterSheet::setTokenValue(std::string text, int32_t tokenNo)
{
    if (this->PC == nullptr) {
        return;
    }
    
    std::stringstream ss(text);
    int32_t numeric;
    float real;
    int32_t itemStart = 23;
    int32_t itemSlotNo = tokenNo - itemStart;
    switch (tokenNo) {
    case 0:
        ss >> numeric;
        this->PC->setLevel(numeric);
        break;
    case 1:
        this->PC->setPlayerRace(text);
        break;
    case 2:
        this->PC->setName(text);
        break;
    case 3:
        this->PC->setPlayerClass(PlayerClass::getClassByName(text));
        break;
    case 4:
        ss >> numeric;
        this->PC->setMaxHp(numeric);
        break;
    case 5:
        ss >> numeric;
        this->PC->setResourceMax(numeric);
        break;
    case 6:
        ss >> numeric;
        this->PC->setStrength(numeric);
        break;
    case 7:
        ss >> numeric;
        this->PC->setAgility(numeric);
        break;
    case 8:
        ss >> numeric;
        this->PC->setStamina(numeric);
        break;
    case 9:
        ss >> numeric;
        this->PC->setIntellect(numeric);
        break;
    case 10:
        ss >> numeric;
        this->PC->setSpirit(numeric);
        break;
    case 11:
        ss >> numeric;
        this->PC->setArmor(numeric);
        break;
    case 12:
        ss >> numeric;
        //hit chance
        break;
    case 13:
        ss >> real;
        this->PC->setCritChance(real/100.0f);
        break;
    case 14:
        ss >> numeric;
        //AP
        break;
    case 39:
        this->processItem(text, itemSlotNo);
        break;
    }
}

void CharacterSheet::processItem(std::string text, int32_t slotNo)
{
    if (text.length() <= 0) {
        return;
    }
    
    std::unordered_map<std::string, std::string> values;
    
    std::regex r(",");
    std::sregex_token_iterator tok(text.begin(), text.end(), r, -1);
    std::sregex_token_iterator end;
    for (tok; tok != end; ++tok) {
        int eqPos = tok->str().find("=");
        if (eqPos != std::string::npos) {
            std::string before = tok->str().substr(0, eqPos);
            std::string after = tok->str().substr(eqPos + 1, text.length() - eqPos);
            values.insert({before, after});
        }
    }
    
    std::stringstream ss;
    int32_t numeric;
    float real;
    switch (slotNo) {
    case 0:
        this->PC->setAmmoItem(new Item());
        break;
    case 16:{
        Weapon *wep = new Weapon();
        ss = std::stringstream(values["min"]);
        ss >> numeric;
        wep->setMinDamage(numeric);
        ss = std::stringstream(values["max"]);
        ss >> numeric;
        wep->setMaxDamage(numeric);
        ss = std::stringstream(values["speed"]);
        ss >> real;
        wep->setWeaponSpeed(real);
        std::string type = values["type"];
        if (type == "TwoHand")
            wep->setSlot(ItemSlot::TwoHand);
        else if (type == "MainHand")
            wep->setSlot(ItemSlot::MainHand);
        else if (type == "OffHand")
            wep->setSlot(ItemSlot::OffHand);
        this->PC->setMainHandItem(wep);
    }
        break;
    case 17:
        this->PC->setOffHandItem(new Item());
        break;
    }
}

PlayerCharacter *CharacterSheet::getPC() const
{
    return PC;
}

void CharacterSheet::setPC(PlayerCharacter *value)
{
    PC = value;
}
