SRC= actions.c eval.c gamemotor.c getplayers.c interpreter.c level.c main.c onquit.c verifcode.c vision.c
OBJ = $(SRC:.c=.o)
CC=gcc
CFLAGS=-Wall -W -g

robotswar:	$(OBJ) header.h
			$(CC) $(OBJ) -o robotswar

%.o:		%.c
			$(CC) $(CFLAGS) -c $<

clean:
	rm *.o *~ core