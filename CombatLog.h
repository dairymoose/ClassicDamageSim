#ifndef COMBATLOG_H
#define COMBATLOG_H

#include <vector>
#include <string>

#define USE_HTML

#ifdef USE_HTML
    #define FONT_BLUE "<font color='blue'>"
#else
    #define FONT_BLUE ""
#endif

#ifdef USE_HTML
    #define FONT_RED "<font color='red'>"
#else
    #define FONT_RED ""
#endif

#ifdef USE_HTML
    #define FONT_MAGENTA "<font color='magenta'>"
#else
    #define FONT_MAGENTA ""
#endif

#ifdef USE_HTML
    #define FONT_MAROON "<font color='maroon'>"
#else
    #define FONT_MAROON ""
#endif

#ifdef USE_HTML
    #define FONT_GREEN "<font color='green'>"
#else
    #define FONT_GREEN ""
#endif

#ifdef USE_HTML
    #define FONT_PURPLE "<font color='purple'>"
#else
    #define FONT_PURPLE ""
#endif

#ifdef USE_HTML
    #define FONT_BROWN "<font color='brown'>"
#else
    #define FONT_BROWN ""
#endif

#ifdef USE_HTML
    #define END_FONT "</font>"
#else
    #define END_FONT ""
#endif

#define DAMAGE_FONT_COLOR FONT_MAROON
#define ATTACKER_FONT_COLOR FONT_PURPLE
#define RECEIVER_FONT_COLOR FONT_PURPLE
#define ABILITY_FONT_COLOR FONT_BROWN

class CombatLog
{
    std::vector<std::string> logText;
public:
    CombatLog();
    std::vector<std::string>& getLogText() {
        return logText;
    }
    void clear() {
        this->logText.clear();
    }
};

#endif // COMBATLOG_H
