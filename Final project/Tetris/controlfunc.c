#ifndef __CONTROLFUNC_C__
#define __CONTROLFUNC_C__

#include "controlfunc.h"
#include "globals.h"
#include "tetrominos.h"
#include "keys.h"
#include "game.c"


// reads key bindings from config
void setkeybind()
{
	right[3][3] = toupper(MOVL);
	right[3][14] = toupper(MOVR);
	right[4][8] = toupper(ROTA);
	right[5][3] = toupper(DROP);
	right[5][14] = toupper(RSET);
	right[6][3] = toupper(SNXT);
	right[7][3] = toupper(STXT);
	right[8][3] = toupper(EXT);
}

// move current piece to the left
void moveleft()
{
	switch (piece)
	{
	case 'I':
		if (center[fixedpoint[0]][fixedpoint[1] + -6] == '[' || center[fixedpoint[0]][fixedpoint[1] - 6] == '<')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 6, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		fixedpoint[1] -= 2;
		return;
	case 'i':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '<')
		{
			return;
		}
		for (int i = -2; i < 2; ++i)
		{
			if (center[fixedpoint[0] + i][fixedpoint[1] - 2] == '[')
			{
				return;
			}
		}
		for (int i = -2; i < 2; ++i)
		{
			memcpy(center[fixedpoint[0] + i] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		}
		fixedpoint[1] -= 2;
		return;
	case 'J':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 4] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		fixedpoint[1] -= 2;
		return;
	case 'K':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 'j':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] + 1][fixedpoint[1]] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 'k':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 4] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] -= 2;
		return;
	case 'L':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] - 1][fixedpoint[1]] == '[')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		fixedpoint[1] -= 2;
		return;
	case 'M':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 2);
		fixedpoint[1] -= 2;
		return;
	case 'l':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] + 1][fixedpoint[1] - 4] == '[')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 4, TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 'm':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] - 4] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] -= 2;
		return;
	case 'O':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, center[fixedpoint[0] - 1] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		fixedpoint[1] -= 2;
		return;
	case 'S':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 4] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 4] == '<')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 4, center[fixedpoint[0] + 1] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] -= 2;
		return;
	case 's':
		if (center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '<' || center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1]] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoO[0] + 10, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 'T':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] -= 2;
		return;
	case 'U':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoO[0] + 10, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 't':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, TetrominoT[0] + 10, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 'u':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, TetrominoO[0] + 10, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	case 'Z':
		if (center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, center[fixedpoint[0] + 1] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		fixedpoint[1] -= 2;
		return;
	case 'z':
		if (center[fixedpoint[0] - 1][fixedpoint[1]] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}	
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoO[0] + 10, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		fixedpoint[1] -= 2;
		return;
	}
}

// move current piece to the right
void moveright()
{
	switch (piece)
	{
	case 'I':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!')
		{
			return;
		}
			
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, TetrominoI[0] + fixedpoint[1] - 4, 2);
		fixedpoint[1] += 2;
		return;
	case 'i':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '!')
		{
			return;
		}
		for (int i = -2; i < 2; ++i)
		{
			if (center[fixedpoint[0] + i][fixedpoint[1] + 2] == '[')
			{
				return;
			}
		}
		for (int i = -2; i < 2; ++i)
		{
			memcpy(center[fixedpoint[0] + i] + fixedpoint[1],  TetrominoT[0] + 8, 4);
		}
		fixedpoint[1] += 2;
		return;
	case 'J':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] - 1][fixedpoint[1]] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		fixedpoint[1] += 2;
		return;
	case 'K':
		if (center[fixedpoint[0] - 1][fixedpoint[1] + 4] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] + 4] == '!' || center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'j':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] + 1][fixedpoint[1] + 4] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'k':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1] + 2, 2);
		fixedpoint[1] += 2;
		return;
	case 'L':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 4] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1] + 4, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		fixedpoint[1] += 2;
		return;
	case 'M':
		if (center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 4] == '!' || center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 4] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'l':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] + 1][fixedpoint[1]] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'm':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1] + 2, 2);
		fixedpoint[1] += 2;
		return;
	case 'O':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 4] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 4, center[fixedpoint[0] - 1] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] += 2;
		return;
	case 'S':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, center[fixedpoint[0] + 1] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		fixedpoint[1] += 2;
		return;
	case 's':
		if (center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 4] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoO[0] + 8, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'T':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] += 2;
		return;
	case 'U':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoO[0] + 8, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 't':
		if (center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 4, TetrominoT[0] + 10, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'u':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoO[0] + 8, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	case 'Z':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 4] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 4] == '!')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 4, center[fixedpoint[0] + 1] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		fixedpoint[1] += 2;
		return;
	case 'z':
		if (center[fixedpoint[0] - 1][fixedpoint[1] + 4] == '[' || center[fixedpoint[0]][fixedpoint[1] + 4] == '!' || center[fixedpoint[0]][fixedpoint[1] + 4] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoO[0] + 8, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		fixedpoint[1] += 2;
		return;
	}
}

// move current piece down
int movedown()
{
	switch (piece)
	{
	case 'I':
		if (center[fixedpoint[0] + 1][fixedpoint[1] - 4] == '=') 
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 4; ++i) 
		{
			if (center[fixedpoint[0] + 1][fixedpoint[1] - 4 + i * 2] == '[') 
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2) 
				{
					return gameover(); 
				}
				initpiece(); 
				return 0;
			}
		}
		
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 4, center[fixedpoint[0]] + fixedpoint[1] - 4, 8);
		memcpy(center[fixedpoint[0]++] + fixedpoint[1] - 4, TetrominoI[0] + fixedpoint[1] - 4, 8);
		return 0;
	case 'i':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]++ - 2] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		return 0;
	case 'J':
		if (center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '=')
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 3; ++i)
		{
			if (center[fixedpoint[0] + 1][fixedpoint[1] - 2 + i * 2] == '[')
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
				{
					return gameover();
				}
				initpiece();
				return 0;
			}
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1] - 2, 6);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 4);
		memcpy(center[fixedpoint[0]++ - 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		return 0;
	case 'K':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0]][fixedpoint[1] + 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 4);
		memcpy(center[fixedpoint[0]++] + fixedpoint[1] + 2, TetrominoT[0] + 10, 2);
		return 0;
	case 'j':
		if (center[fixedpoint[0] + 2][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 2][fixedpoint[1] + 2] == '=' || center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 6);
		memcpy(center[fixedpoint[0]++ + 1] + fixedpoint[1] - 2, TetrominoO[0] + 10, 4);
		return 0;
	case 'k':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 2][fixedpoint[1] - 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1] - 2, TetrominoO[0] + 10, 4);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]++ + 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		return 0;
	case 'L':
		if (center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '=')
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 3; ++i)
		{
			if (center[fixedpoint[0] + 1][fixedpoint[1] - 2 + i * 2] == '[')
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
				{
					return gameover();
				}
				initpiece();
				return 0;
			}
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1] - 2, 6);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 4);
		memcpy(center[fixedpoint[0]++ - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		return 0;
	case 'M':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 2][fixedpoint[1] + 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], TetrominoO[0] + 10, 4);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]++ + 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		return 0;
	case 'l':
		if (center[fixedpoint[0] + 2][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 2][fixedpoint[1] - 2] == '=' || center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 6);
		memcpy(center[fixedpoint[0]++ + 1] + fixedpoint[1], TetrominoO[0] + 10, 4);
		return 0;
	case 'm':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0]][fixedpoint[1] - 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], TetrominoT[0] + 10, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 4);
		memcpy(center[fixedpoint[0]++] + fixedpoint[1] - 2, TetrominoT[0] + 10, 2);
		return 0;
	case 'O':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '=')
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 2; ++i)
		{
			if (center[fixedpoint[0] + 1][fixedpoint[1] + i * 2] == '[')
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
				{
					return gameover();
				}
				initpiece();
				return 0;
			}
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 4);
		memcpy(center[fixedpoint[0]++ - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 4);
		return 0;
	case 'S':
		if (center[fixedpoint[0] + 2][fixedpoint[1] - 2] == '=')
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 2; ++i)
		{
			if (center[fixedpoint[0] + 2][fixedpoint[1] - 2 + i * 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
				{
					return gameover();
				}
				initpiece();
				return 0;
			}
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1] - 2, center[fixedpoint[0] + 1] + fixedpoint[1] - 2, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoS[0] + 8, 6);
		memcpy(center[fixedpoint[0]++] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 4);
		return 0;
	case 's':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 2][fixedpoint[1] + 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoO[0] + 10, 4);
		memcpy(center[fixedpoint[0]++ + 2] + fixedpoint[1] + 2, TetrominoT[0] + 10, 2);
		return 0;
	case 'T':
		if (center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '=')
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 3; ++i)
		{
			if (center[fixedpoint[0] + 1][fixedpoint[1] - 2 + i * 2] == '[')
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
				{
					return gameover();
				}
				initpiece();
				return 0;
			}
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1] - 2, 6);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoT[0] + 8, 6);
		memcpy(center[fixedpoint[0]++ - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		return 0;
	case 'U':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0]++ - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		return 0;
	case 't':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]++] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 6);
		return 0;
	case 'u':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0]++ - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		return 0;
	case 'Z':
		if (center[fixedpoint[0] + 2][fixedpoint[1]] == '=')
		{
			initpiece();
			return 0;
		}
		for (int i = 0; i < 2; ++i)
		{
			if (center[fixedpoint[0] + 2][fixedpoint[1] + i * 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
			{
				if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
				{
					return gameover();
				}
				initpiece();
				return 0;
			}
		}
		memcpy(center[fixedpoint[0] + 2] + fixedpoint[1], center[fixedpoint[0] + 1] + fixedpoint[1], 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoZ[0] + 8, 6);
		memcpy(center[fixedpoint[0]++] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 4);
		return 0;
	case 'z':
		if (center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 2][fixedpoint[1]] == '=' || center[fixedpoint[0] + 2][fixedpoint[1]] == '[')
		{
			if (fixedpoint[0] == 1 || fixedpoint[0] == 2)
			{
				return gameover();
			}
			initpiece();
			return 0;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1], TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoO[0] + 10, 4);
		memcpy(center[fixedpoint[0]++ + 2] + fixedpoint[1], TetrominoT[0] + 10, 2);
		return 0;
	}
	return 0;
}

// rotate current piece (clockwise obviously)
void rotate()
{
	switch (piece)
	{
	case 'I':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1]] == '=' || center[fixedpoint[0] - 1][fixedpoint[1]] == '[' || center[fixedpoint[0] - 2][fixedpoint[1]] == '[') //если крутить фигуру места нет, то ничего не делаем
		{
			return;
		}
		 
		memcpy(center[fixedpoint[0] - 2] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, TetrominoI[0] + fixedpoint[1] - 4, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 'i'; 
		return;
	case 'i': 
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!' || center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '[' || center[fixedpoint[0]][fixedpoint[1] - 4] == '<')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 2] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 4, TetrominoI[1] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[1] + 12, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		piece = 'I';
		return;
	case 'J':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1]] == '=' || center[fixedpoint[0] - 1][fixedpoint[1]] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoO[0] + 8, 6);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 'K';
		return;
	case 'K':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoT[0] + 8, 4);
		piece = 'j';
		return;
	case 'j':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] - 1][fixedpoint[1]] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoO[0] + 10, 6);
		piece = 'k';
		return;
	case 'k':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!' || center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 4);
		piece = 'J';
		return;
	case 'L':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1]] == '=' || center[fixedpoint[0] - 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoT[0] + 10, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoO[0] + 10, 4);
		piece = 'M';
		return;
	case 'M':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 10, 6);
		piece = 'l';
		return;
	case 'l':
		if (center[fixedpoint[0] - 1][fixedpoint[1]] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] + 1][fixedpoint[1]] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2,
			   TetrominoT[0] + 8, 4);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2,
			   TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoO[0] + 10, 4);
		piece = 'm';
		return;
	case 'm':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!' || center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] - 2, TetrominoT[0] + 6, 6);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		piece = 'L';
		return;
	case 'S':
		if (center[fixedpoint[0] - 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoT[0] + 6, 6);
		piece = 's';
		return;
	case 's':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '<' || center[fixedpoint[0] + 1][fixedpoint[1] - 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] - 2, TetrominoO[0] + 10, 6);
		piece = 'S';
		return;
	case 'T':
		if (center[fixedpoint[0] + 1][fixedpoint[1]] == '[' || center[fixedpoint[0] + 1][fixedpoint[1]] == '=')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoI[0] + fixedpoint[1] - 2, 2);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 'U';
		return;
	case 'U':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 't';
		return;
	case 't':
		if (center[fixedpoint[0] - 1][fixedpoint[1]] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1], center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 'u';
		return;
	case 'u':
		if (center[fixedpoint[0]][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '!')
		{
			return;
		}
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1], TetrominoI[0] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 'T';
		return;
	case 'Z':
		if (center[fixedpoint[0] - 1][fixedpoint[1] + 2] == '[' || center[fixedpoint[0]][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoO[0] + 8, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		piece = 'z';
		return;
	case 'z':
		if (center[fixedpoint[0]][fixedpoint[1] - 2] == '[' || center[fixedpoint[0]][fixedpoint[1] - 2] == '<' || center[fixedpoint[0] + 1][fixedpoint[1] + 2] == '[')
		{
			return;
		}
		memcpy(center[fixedpoint[0] - 1] + fixedpoint[1] + 2, TetrominoI[0] + fixedpoint[1] + 2, 2);
		memcpy(center[fixedpoint[0]] + fixedpoint[1] - 2, TetrominoO[0] + 10, 6);
		memcpy(center[fixedpoint[0] + 1] + fixedpoint[1] + 2, center[fixedpoint[0]] + fixedpoint[1], 2);
		piece = 'Z';
		return;
	}
}

// move current piece according to encoder position
void readEncoder()
{
	if (fileDev != NULL)
	{
		fgets(arr, MAX_NUM_DIGITS, fileDev); 
		sscanf(arr, "%d", &currentEncoderPos);
	if (currentEncoderPos > prevEncoderPos) // right
	{
		for (uint32_t i = 0; i < (currentEncoderPos - prevEncoderPos); i++)
		{
			moveright();
			updatescrn();
		}
		prevEncoderPos = currentEncoderPos;
	}
	if (currentEncoderPos < prevEncoderPos) //left
	{

		for (uint32_t i = 0; i < (prevEncoderPos - currentEncoderPos); i++)
		{
			moveleft();
			updatescrn();
		}
		prevEncoderPos = currentEncoderPos;
	}
}
#endif