#-------------------------------------------------
#
# Project created by QtCreator 2023-11-24T14:50:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WarriorDamageSim
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        Ability.cpp \
        AbilityTooltip.cpp \
        ActionsDialog.cpp \
        AppliedBuff.cpp \
        Buff.cpp \
        CharacterSheet.cpp \
        CombatLog.cpp \
        Combatant.cpp \
        DamageSimulation.cpp \
        Enemy.cpp \
        GlobalAbilityList.cpp \
        InstantTooltipLabel.cpp \
        Item.cpp \
        MeleeHitResult.cpp \
        PlayerCharacter.cpp \
        PlayerClass.cpp \
        PriorityAction.cpp \
        PriorityActionList.cpp \
        Rune.cpp \
        SingularPriorityAction.cpp \
        Talent.cpp \
        TalentsDialog.cpp \
        Weapon.cpp \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        Ability.h \
        AbilityDamageType.h \
        AbilityTooltip.h \
        ActionsDialog.h \
        AppliedBuff.h \
        Buff.h \
        CharacterSheet.h \
        CombatLog.h \
        Combatant.h \
        DamageSimulation.h \
        Enemy.h \
        GlobalAbilityList.h \
        InstantTooltipLabel.h \
        Item.h \
        ItemSlot.h \
        MainWindow.h \
        MeleeHitResult.h \
        PlayerCharacter.h \
        PlayerClass.h \
        PriorityAction.h \
        PriorityActionList.h \
        Rune.h \
        SingularPriorityAction.h \
        Talent.h \
        TalentsDialog.h \
        Weapon.h

FORMS += \
        AbilityTooltip.ui \
        ActionsDialog.ui \
        CharacterSheet.ui \
        MainWindow.ui \
        SingularPriorityAction.ui \
        TalentsDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
