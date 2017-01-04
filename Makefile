CSRC = actions.c gamemotor.c getplayers.c level.c main.c onquit.c vision.c players_code.c hash_table.c
SRC = $(CSRC) lex_analyze.l compiler.y
OBJ = $(CSRC:.c=.o) compiler.tab.o lex.yy.o
CC = gcc
CFLAGS = -Wall -W -g -Wno-sign-compare
BFLAGS = --warnings=all -v -d
LFLAGs = -s

robotswar:	$(OBJ) header.h
			$(CC) $(OBJ) -g -o robotswar -ldl -Wl,--export-dynamic

lex.yy.c : lex_analyze.l compiler.tab.h
	flex $(LFLAGS) lex_analyze.l

compiler.tab.c : compiler.y
	bison $(BFLAGS) compiler.y

compiler.tab.o compiler.tab.h : compiler.tab.c

lex.yy.o : lex.yy.c

%.o:		%.c
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.tab.c lex.yy.c robotswar core *~ compiler.output