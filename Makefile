CSRC = actions.c gamemotor.c getplayers.c level.c main.c onquit.c vision.c get_players_fonc.c players_code.c hash_table.c
SRC = $(CSRC) lex_analyze.l compiler.y
OBJ = $(CSRC:.c=.o) compiler.tab.o
CC = gcc
CFLAGS = -Wall -W -g
BFLAGS = --warnings=all -v

robotswar:	$(OBJ) header.h include_player_fct.h
			$(CC) $(OBJ) -o robotswar

lex.yy.c : lex_analyze.l
	flex lex_analyze.l

compiler.tab.c : compiler.y
	bison $(BFLAGS) compiler.y

compiler.tab.o : compiler.tab.c lex.yy.c

%.o:		%.c
			$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *.tab.c lex.yy.c robotswar core *~ include_player_fct.h get_players_fonc.c *robot.c compiler.output ]
	touch include_player_fct.h
	echo "void init_functionlist(){}" >> get_players_fonc.c
	

reload:
	rm -f *.tab.c lex.yy.c robotswar core *~ compiler.output *.o ]