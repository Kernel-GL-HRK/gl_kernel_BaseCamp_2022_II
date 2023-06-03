#ifndef __TETROMINOS_H__
#define __TETROMINOS_H__

#include "globals.h"

// all possible tetrominos
char TetrominoI[2][WIDTH] = {"<! . . . . . . . . . .!>",
							 "<! . . .[][][][] . . .!>"};

char TetrominoJ[2][WIDTH] = {"<! . . .[] . . . . . .!>",
							 "<! . . .[][][] . . . .!>"};

char TetrominoL[2][WIDTH] = {"<! . . . . .[] . . . .!>",
							 "<! . . .[][][] . . . .!>"};

char TetrominoO[2][WIDTH] = {"<! . . . .[][] . . . .!>",
							 "<! . . . .[][] . . . .!>"};

char TetrominoS[2][WIDTH] = {"<! . . . .[][] . . . .!>",
							 "<! . . .[][] . . . . .!>"};

char TetrominoT[2][WIDTH] = {"<! . . . .[] . . . . .!>",
							 "<! . . .[][][] . . . .!>"};

char TetrominoZ[2][WIDTH] = {"<! . . .[][] . . . . .!>",
							 "<! . . . .[][] . . . .!>"};

#endif