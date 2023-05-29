CC = gcc
CFLAGS = -Wall -pedantic -g
.PHONY: depend clean

all: tetris
	@echo  tetris has been compiled.

tetris: tetris.o
	$(CC) $(CFLAGS) $< -o tetris -lncurses

tetris.o: tetris.c controlfunc.c game.c libs.h globals.h tetrominos.h keys.h settings.h controlfunc.h game.h
	$(CC) $(CFLAGS) -c $< controlfunc.c game.o

game.o: game.c game.h globals.h
	$(CC) $(CFLAGS) -c $<

controlfunc.o: controlfunc.c game.c globals.h keys.h tetrominos.h controlfunc.h 
	$(CC) $(CFLAGS) -c controlfunc.c game.c

clean:
	-rm *.o *~ tetris

#depend: tetris.c game.c controlfunc.c
#	makedepend $(INCLUDES) $^
