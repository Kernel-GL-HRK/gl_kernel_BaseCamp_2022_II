#ifndef __SETTINGS_H__
#define __SETTINGS_H__

//formula for calculating score
#define SCORE (100*cleared+75*(cleared/2)+125*(cleared/3)+150*(cleared/4))* level
//formula for calculating drop interval
#define DROPINTERVAL (DELAY/1000) * ((300 - level*13)/5)
//delay is the delay between two commands, here to avoid raping processor
#define DELAY 7777
#define MAXLEVEL 30
//number of lines that have to be cleared for lvl up
#define LINESFORLVLUP 10

#endif