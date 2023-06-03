#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "libs.h"
#include "settings.h"

// DO NOT CHANGE
#define WIDTH 25				// playing field
#define HEIGHT 24				
#define TOPLISTMAXLINELENGTH 34 
#define MAX_NUM_DIGITS 12		// maximum number of digits for the encoder position value

FILE *fileDev;

// init variables
char piece; 					
char *name; 					
char arr[MAX_NUM_DIGITS];		

// set flags to default values
int level,				 		
	score,				  		
	showtext = 1,		  		// test with keys 
	next,				  		// next tetromino
	shownext = 1,		  		
	end,				  		// gameover flag
	clrlines = 0,		  		// the number of lines cleared by the player
	startlevel,			  		
	dropped = 0,		  		
	prevEncoderPos = 0,	  		
    currentEncoderPos = 0;		
int fixedpoint[2] = {0}; 		

// screen is divided into three sections - left, right and center
char left[HEIGHT][WIDTH] = {0};
char center[HEIGHT][WIDTH] = {0};
char right[HEIGHT][WIDTH] = {0};

struct timeval t1, t2;

#endif