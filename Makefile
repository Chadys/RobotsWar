CSRC = actions.c gamemotor.c getplayers.c level.c main.c onquit.c vision.c players_code.c hash_table.c
SRC = $(CSRC) lex_analyze.l compiler.y lexical_checker.y
OBJ = $(CSRC:.c=.o) compiler.tab.o lexical_checker.tab.o lex.yy.o
CC = gcc
CFLAGS = -Wall -W -g
BFLAGS = --warnings=all -v

robotswar:	$(OBJ) header.h compiler.tab.h
			$(CC) $(OBJ) -o robotswar



lex.yy.c : lex_analyze.l compiler.tab.h
	flex -s lex_analyze.l



compiler.tab.c : compiler.y
	bison $(BFLAGS) -d compiler.y

lexical_checker.tab.c : lexical_checker.y
	bison $(BFLAGS) lexical_checker.y


lexical_checker.tab.o :  lexical_checker.tab.c

compiler.tab.o compiler.tab.h : compiler.tab.c

lex.yy.o : lex.yy.c

%.o:		%.c
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.tab.* lex.yy.c robotswar core *~ compiler.output ]