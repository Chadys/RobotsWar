CSRC = actions.c gamemotor.c getplayers.c level.c main.c onquit.c vision.c players_code.c hash_table.c
SRC = $(CSRC) lex_analyze.l compiler.y lexical_checker.y lex_check.l
OBJ = $(CSRC:.c=.o) compiler.tab.o lexical_checker.tab.o lex.yy.o lex.check.o
CC = gcc
CFLAGS = -Wall -W -g -Wno-sign-compare
BFLAGS = --warnings=all -v -d
FFLAGS = -s

robotswar:	$(OBJ) header.h compiler.tab.h
			$(CC) $(OBJ) -o robotswar



lex.yy.c : lex_analyze.l compiler.tab.h
	flex $(FFLAGS) lex_analyze.l

lex.check.c : lex_check.l lexical_checker.tab.h
	flex $(FFLAGS) lex_check.l



compiler.tab.c : compiler.y
	bison $(BFLAGS) compiler.y

lexical_checker.tab.c : lexical_checker.y
	bison $(BFLAGS) lexical_checker.y


lexical_checker.tab.o lexical_checker.tab.h :  lexical_checker.tab.c

compiler.tab.o compiler.tab.h : compiler.tab.c

lex.yy.o : lex.yy.c

lex.check.o : lex.check.c

%.o:		%.c
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.tab.* lex.*.c robotswar core *~ *.output ]