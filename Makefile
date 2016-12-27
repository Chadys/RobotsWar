CSRC= actions.c eval.c gamemotor.c getplayers.c interpreter.c level.c main.c onquit.c verifcode.c vision.c
SRC= $(CSRC) robot.l robot.y
OBJ = $(CSRC:.c=.o) robot.tab.o
CC=gcc
CFLAGS=-Wall -W -g

robotswar:	$(OBJ) header.h
			$(CC) $(OBJ) -o robotswar

robot.tab.c : robot.y
	bison robot.y

lex.yy.c : robot.l
	flex robot.l

robot.tab.o : robot.tab.c lex.yy.c

%.o:		%.c
			$(CC) $(CFLAGS) -c $<

clean:
	rm rm -f *.tab.c lex.yy.c robotswar core *~