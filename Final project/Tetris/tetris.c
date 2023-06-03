#include "libs.h"
#include "globals.h"
#include "tetrominos.h"
#include "keys.h"	  
#include "settings.h"
#include "controlfunc.h"
#include "controlfunc.c"
#include "game.h"
#include "game.c"

int main(void)
{
	name = malloc(sizeof *name * 14);		// gui is designed for a maximum length of 14
	srand(time(NULL));						
	initscr();								// initializes the curses library, returns a window object representing the whole screen.
	start_color();							
	cbreak();								// enter cbreak mode. the function changes the keyboard mode to partial keyboard control (interrupts like ctrl+c will be handled). raw() mode - full control without interrupts.
	init_pair(1, COLOR_GREEN, COLOR_BLACK); 
	init_pair(2, COLOR_RED, COLOR_BLACK);	
	attron(COLOR_PAIR(1));					
	do
	{
		clear();														
		printw("\n\n\n       Enter starting level (1-%d): ", MAXLEVEL); 
		refresh();														
		scanw("%d ", &startlevel);										
	} while (startlevel < 1 || startlevel > MAXLEVEL);					
	clear();															
	printw("\n\n\n       Enter your name: ");							
	refresh();															
	scanw("%13s ", name);												
	noecho();															// exit echo mode. echo of input characters is disabled.
	curs_set(0);														// set the cursor as invisible
	next = rand() % 7;													
	fileDev = fopen("/dev/rotary1", "r");								// open the file dev/rotaty1 to read the encoder position
	while (!game())
		;			 
	free(name);		 
	endwin();		 // deinitialize the library and return the terminal to its normal state
	fclose(fileDev); 
	return 0;
}
