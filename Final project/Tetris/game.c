
#ifndef __GAME_C__
#define __GAME_C__

#include "game.h"
#include "globals.h"

// function shows next tetromino
void show_next()
{
	switch (next)
	{
	case 0:
		memcpy(left[11] + WIDTH - 10, TetrominoI[1] + 8, 8);
		memcpy(left[10] + WIDTH - 8, TetrominoJ[0] + 8, 2);
		memcpy(left[11] + WIDTH - 8, TetrominoJ[1] + 8, 6);
		return;
	case 2:
		memcpy(left[10] + WIDTH - 4, TetrominoL[0] + 12, 2);
		memcpy(left[11] + WIDTH - 8, TetrominoL[1] + 8, 6);
		return;
	case 3:
		memcpy(left[10] + WIDTH - 6, TetrominoO[0] + 10, 4);
		memcpy(left[11] + WIDTH - 6, TetrominoO[1] + 10, 4);
		return;
	case 4:
		memcpy(left[10] + WIDTH - 6, TetrominoS[0] + 10, 4);
		memcpy(left[11] + WIDTH - 8, TetrominoS[1] + 8, 4);
		return;
	case 5:
		memcpy(left[10] + WIDTH - 6, TetrominoT[0] + 10, 2);
		memcpy(left[11] + WIDTH - 8, TetrominoT[1] + 8, 6);
		return;
	case 6:
		memcpy(left[10] + WIDTH - 8, TetrominoZ[0] + 8, 4);
		memcpy(left[11] + WIDTH - 6, TetrominoZ[1] + 10, 4);
		return;
	}
}

// self explanatory, refreshes screen
void updatescrn()
{
	clear();
	printw("\n");
	for (int i = 2; i < HEIGHT; ++i)
	{
		if (shownext)
		{
			show_next();
		}
		printw("%s", left[i]);	
		printw("%s", center[i]); 
		if (showtext)			 
		{
			printw("%s", right[i]); 
		}
		printw("\n");
	}
	refresh();	 
	if (shownext)
	{
		memcpy(left[10] + WIDTH - 10, "        ", 8);
		memcpy(left[11] + WIDTH - 10, "        ", 8);
	}
}

// updates score number on the gamescreen (on the left part)
void updatescore()
{
	char *tmp = malloc(sizeof *tmp * 15); 
	sprintf(tmp, "%-14d", score);		  
	memcpy(left[7] + 9, tmp, 14);		  
	free(tmp);							  
}

// show toplist
void toplist()
{
	char *buffer = malloc(sizeof *buffer * TOPLISTMAXLINELENGTH); 
	FILE *f;
	clear();																		 
	if (!(f = fopen("toplist", "r")))
	{												 
		printw("\n\n\n    Toplist doesn't exist! Your score has to be higher than 0" 
			   " to be added ;)\n");
	}
	else
	{
		printw("\n");
		while (fgets(buffer, TOPLISTMAXLINELENGTH, f) != NULL) 
		{ 
			printw("                        %s", buffer);	   
		}
		fclose(f);											   
	}
	refresh();	  
	free(buffer); 
	getch();	  
}

// add score to toplist (top-20) if needed
void addscore()
{
	if (!score) 
	{
		return; 
	}
	FILE *f;
	if (!(f = fopen("toplist", "r"))) 
	{
		if (!(f = fopen("toplist", "w"))) 
		{
			exit(1); 
		}
		fprintf(f, "NAME          LVL SCORE        \n%-13s %2d  %-14d\n", name, level, score); // print only current playes score 
		fclose(f);																			   
		return;
	}
	f = fopen("toplist", "r");
	int num, added = 0;
	char *buffer = malloc(sizeof *buffer * TOPLISTMAXLINELENGTH); 
	FILE *tmp;
	if (!(tmp = fopen("tmp", "a+"))) // mode read and add data to the end of file
		exit(1);					 
	int cntr = 21;// max value of rows for top-20 file
	
	while (fgets(buffer, TOPLISTMAXLINELENGTH, f) != NULL && --cntr) 
	{
		num = strtol(buffer + 18, NULL, 10);  
		if (!added && score > num && num != 0) 
		{
			fprintf(tmp, "%-13s %2d  %-14d\n", name, level, score); 
			score = 0;												
		}
		fputs(buffer, tmp); 
	}
	if (cntr && score) // if current player score > 0 and score on top-20 
	{
		fprintf(tmp, "%-13s %2d  %-14d\n", name, level, score);
	}
	fclose(f);				  
	fclose(tmp);			  
	remove("toplist");		  
	rename("tmp", "toplist"); 
	free(buffer);			  
}

// prints game over screen
int gameover()
{
	nodelay(stdscr, FALSE); // FALSE - getch() wait for press any key
	if (!end)				
	{
		addscore(); 
	}
	end = 1;									 
	memcpy(left[9], "    __      __      ___ \0" 
					"  /'_ `\\  /'__`\\  /' __`\0"
					" /\\ \\L\\ \\/\\ \\L\\.\\_/\\ \\/\\\0"
					" \\ \\____ \\ \\__/.\\_\\ \\_\\ \0"
					"  \\/___L\\ \\/__/\\/_/\\/_/\\\0"
					"    /\\____/             \0"
					"    \\_/__/              \0",
		   WIDTH * 7);
	memcpy(center[9], "___      __         ___ \0"
					  " __`\\  /'__`\\      / __`\0"
					  " \\/\\ \\/\\  __/     /\\ \\L\\\0"
					  "\\_\\ \\_\\ \\____\\    \\ \\___\0"
					  "/_/\\/_/\\/____/     \\/___\0",
		   WIDTH * 5);
	memcpy(right[9], "  __  __    __  _ __    \0"
					 "\\/\\ \\/\\ \\ /'__`/\\`'__\\  \0"
					 " \\ \\ \\_/ /\\  __\\ \\ \\/   \0"
					 "_/\\ \\___/\\ \\____\\ \\_\\   \0"
					 "/  \\/__/  \\/____/\\/_/   \0",
		   WIDTH * 5);
	memcpy(center[16], "    : QUIT    : RESET   \0"
					   "        : TOPLIST       \0",
		   WIDTH * 2);
	center[16][3] = toupper(EXT);	
	center[16][13] = toupper(RSET); 
	center[17][7] = toupper(TPLS);	
	clear();						
	printw("\n");
	for (int i = 2; i < HEIGHT; ++i)
	{
		if (i == 9 || i == 15 || i == 16)
		{
			attron(COLOR_PAIR(2));
		}
		else if (i == 18)
		{
			attron(COLOR_PAIR(1)); 
		}
		printw("%s", left[i]);	   
		if (i == 14 || i == 15)
		{
			attron(COLOR_PAIR(1)); 
		}
		printw("%s", center[i]);  
		printw("%s\n", right[i]);  
	}
	refresh();
	return 1;
}

// check if a line should be cleared
void checkclr()
{
	int cleared = 0;
	if (fixedpoint[0] > 2)
	{
		for (int i = -2; i < 2; ++i)
		{
			if (!strncmp(center[fixedpoint[0] + i] + 3, "][][][][][][][][][", 18))
			{
				++cleared;
				for (int j = fixedpoint[0] + i; j > 0; --j)
				{
					memcpy(center[j] + 2, center[j - 1] + 2, 20);
				}
			}
		}
		if (cleared)
		{
			score += SCORE;
			score += dropped;
			dropped = 0;
			updatescore();
			updatescrn();
		}
	}
	clrlines += cleared;
}

// initializes a new piece зачем фикседпоинт
void initpiece()
{
	checkclr(); 
	int current;
	current = next;
	next = rand() % 7; 
	switch (current)   
	{
	case 0:
		memcpy(center[0], TetrominoI[0], WIDTH * 2); 
		fixedpoint[0] = 1; 
		fixedpoint[1] = 12;
		piece = 'I';
		return;
	case 1:
		memcpy(center[0], TetrominoJ[0], WIDTH * 2);
		fixedpoint[0] = 1;
		fixedpoint[1] = 10;
		piece = 'J';
		return;
	case 2:
		memcpy(center[0], TetrominoL[0], WIDTH * 2);
		fixedpoint[0] = 1;
		fixedpoint[1] = 10;
		piece = 'L';
		return;
	case 3:
		memcpy(center[0], TetrominoO[0], WIDTH * 2);
		fixedpoint[0] = 1;
		fixedpoint[1] = 10;
		piece = 'O';
		return;
	case 4:
		memcpy(center[0], TetrominoS[0], WIDTH * 2);
		fixedpoint[0] = 0;
		fixedpoint[1] = 10;
		piece = 'S';
		return;
	case 5:
		memcpy(center[0], TetrominoT[0], WIDTH * 2);
		fixedpoint[0] = 1;
		fixedpoint[1] = 10;
		piece = 'T';
		return;
	case 6:
		memcpy(center[0], TetrominoZ[0], WIDTH * 2);
		fixedpoint[0] = 0;
		fixedpoint[1] = 10;
		piece = 'Z';
		return;
	}
}

// initializes screen
void init()
{
  memcpy(left[0], "                        \0"
			"                        \0"
			"                        \0"
			"  PLAYER:               \0"
			"                        \0"
			"  LEVEL:                \0"
			"                        \0"
			"  SCORE:                \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"                        \0"
			"     |\\____/|           \0"
			"     | .  . |   _       \0"
			"     |  -- /   | |      \0"
			"     |     \\  / /       \0"
			"     |      \\/ /        \0"
			"     + +__ +__/         \0",
			 (HEIGHT)*(WIDTH));
	memcpy(center[0], "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<! . . . . . . . . . .!>\0"
					  "<!====================!>\0"
					  "  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\0",
		   (HEIGHT) * (WIDTH));
	memcpy(right[0], "                        \0"
					 "                        \0"
					 "                        \0"
					 "    : LEFT     :RIGHT   \0"
					 "         :ROTATE        \0"
					 "    : DROP     :RESET   \0"
					 "    : SHOW/HIDE NEXT    \0"
					 "    : HIDE THIS TEXT    \0"
					 "    : EXIT              \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0"
					 "                        \0",
		   (HEIGHT) * (WIDTH));
}

// updates level number on the gamescreen (on the left part)
void updatelevel()
{
	char *tmp = malloc(sizeof *tmp * 15); 
	sprintf(tmp, "%-14d", level);		  
	memcpy(left[5] + 9, tmp, 14);		  
	free(tmp);							 
}

// simulates a game of tetris
int game()
{
	nodelay(stdscr, FALSE);							 // FALSE - getch() wait for press any key
	init();											 
	setkeybind();									 
	clear();										 
	end = 0;										 
	score = 0;										 
	level = startlevel;								 
	clrlines = 0;									 
	memcpy(left[3] + 10, name, strlen(name));		 
	printw("\n\n\n       Press any key to start\n"); 
	refresh();										 
	getch();										
	updatescore();									 
	updatelevel();									 
	initpiece();									 
	updatescrn();									 
	nodelay(stdscr, TRUE);							 // TRUE - getch() NOT wait for press any key
	gettimeofday(&t1, NULL);						 
	while (!usleep(DELAY))
	{
		readEncoder();
		switch (getch())
		{
		case DROP:
			if (movedown()) 
			{
				continue; 
			}
			++dropped;	  
			updatescrn(); 
			continue;	  
		case EXT:
			return 1; 
		case RSET:
			return 0; // new game
		case STXT:
			if (end) 
			{
				continue; 
			}
			showtext = !showtext; 
			updatescrn();		  
			continue;
		case SNXT:
			if (end) 
			{
				continue; 
			}
			shownext = !shownext; 
			updatescrn();		  
			continue;
		case MOVR:
			if (end) 
			{
				continue; 
			}
			moveright();  
			updatescrn(); 
			continue;
		case MOVL:
			if (end) 
			{
				continue; 
			}
			moveleft();	  
			updatescrn(); 
			continue;
		case ROTA:
			if (end || fixedpoint[0] < 2)
			{
				continue; 
			}
			rotate();	  
			updatescrn(); 
			continue;
		case TPLS:
			if (!end) 
			{
				continue; 
			}
			toplist(); 
		}

		if (end) 
		{
			gameover(); 
			continue;	
		}
		if (clrlines == LINESFORLVLUP) 
		{
			if (level < MAXLEVEL) 
			{
				++level;	   
				updatelevel(); 
				clrlines = 0;  
			}
		}
		gettimeofday(&t2, NULL);/
		if ((((t2.tv_sec - t1.tv_sec) * 1000) + ((t2.tv_usec - t1.tv_usec) / 1000)) > DROPINTERVAL)
		{
			if (movedown())
			{
				continue;
			}
			updatescrn();
			gettimeofday(&t1, NULL);
		}
	}
	return 1;
}
#endif