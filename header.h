#ifndef _ROBOTS_
#define _ROBOTS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>

#define TMP_FILE "tmplist.txt" //name given to a temporary file
#define MAX_ROBOTS 50 //max number of players
#define ROBOTSDIR "robots" //name of the directory containing players' files
#define MAX_FILE_NAME 256 //max number of letters of a file
#define MAX_LETTERS_READ 100 //max number of letter read at once
#define MAX_TAB 15 //max number of tabulation that can be printed to a player's compiled file
#define MAX_LIFE 3 //starting number of a robot's life
#define MAX_VISION 10 //range of a robot's field of view
#define MAX_ENERGY 6 //starting number of a robot's energy units
#define MAX_TURN 50 //number of turn max in a game
#define MAX_TIMER 500 //interval of read word in a player code before the robot lose 1 energy unit
#define DELAY_ACTION 150000000 //pause delay between each player's action
#define DELAY_TURN 100000000 //pause delay between each game turn

# define YCHECKTIMER(N) timer+=N; if(!update_energy(&timer)) return; //in players' file, check the timer and exit the function if too much time was taken

/* coords */
struct coord{
	unsigned short i; //y
	unsigned short j; //x
	short dir;
};
typedef struct coord coord;
//Direction -> north=1, east=2, south=3, west=4

/* Hash table and hash cell */
typedef struct cell
{
    char *name;
    int val;
    struct cell *next;
}cell;
typedef cell* llist;

typedef struct hashtable
{
    int n;
    int p;
    llist *alveole;
}hashtable;

/* players */
struct player{
	char *color; //sûrement un code en hexa, il faudra voir les couleurs prédéfinies de sdl sinon
	char *name;
	unsigned short treasure;
	unsigned short score;
	unsigned short life;
	unsigned short energy;
	coord loc;
	char onbase;
    char *code;
    size_t sizecode;
    hashtable variables;
};
typedef struct player player;

/* player list */
struct cellplay{
	player *play;
	struct cellplay* next;
};
typedef struct cellplay cellplay;
typedef cellplay* listplay;


/* action list */
typedef void (*fct)(player *,int,int,short);
struct action{
	player *p;
	short priority; //MAX_ENERGY-(p->energy-cout energetique de l'action)
	int arg_0;
	int arg_1;
	fct fct;
	struct action *next;
};
typedef struct action action;
typedef action * listaction;


/* loot list */
struct butin{
	coord data;
	char onbase;
	struct butin * next;
};
typedef struct butin butin;
typedef struct butin * listbutin;


/* player's actions enum */
typedef enum ACTION{
        SNOOZE,
        TURNAROUND,
        GO,
        SHOOT
} ACTION;


/* globals */
extern listplay playerslist; //list of players
extern listaction actionslist; //list of players' actions for this turn
extern listbutin extrabutinslist; //extra treasures after a player is killed
extern listbutin butinslist; //all treasure on the map
extern char **level; //the level and all it contains
extern unsigned short nlvl; //size of the level
extern char * currentaction; //string used to display an action being done
extern char reading; //indicate if a player's code is being read (for management of signals)
extern jmp_buf ebuf; //to make jump
extern sigjmp_buf sigebuf; //to jump from signals
extern hashtable keywords; //to stock our parser's keywords


/* prototypes */

/* gamemotor.c */
//the game loop and handler of the actions list, and the ending displayer
void start();
void end(unsigned short);

/* getplayer.c */
//construct a player per valid code in ROBOTSDIR and add it to the game
char getplayers();
void freeplayer(player *);
char compile_all();

/* level.c */
//construct a level (depending on player's number) with files in the levels/ directory
int set_level(int);
void displaylevel(int);
player *findplayer(unsigned short, unsigned short);
coord addrandombase();
void addrandombutin();

/* actions.c */
//possible actions of a player and the function to add one to the global list for this turn
void add_action(action *);
void turnaround(player *, int,int,short);
void go(player *, int,int,short);
void shoot(player *, int,int,short);

/* vision.c */
//return what a robot can see a a specific case
coord ligne(coord, int, int);

/* onquit.c */
//free every allocated memory of the game
void cleangame();
void ctrlC(int);
void div0(int);

/* players_code.c */
//interact with the player's compiled code
void activate(player *);
void create_action(char, int, int, player*);
char update_energy(unsigned int *, player*);

/* lex.yy.c */
//lexical analyser
void yy_init_parser(char*,size_t);
void yy_flush();

/* compiler.tab.c */
// compiler's parsing
int yyparse();

/* compiler.tab.c */
// compiler's parsing but only to check codes' correctness
int checkparse();

/* get_players_fonc.c */
// inititalisation of function list, only do something if the compiler has been launched
void init_functionlist();

/* hash_table.c */
hashtable init_hash(int);
void free_hash(hashtable);
llist get_or_insert(hashtable, char *);

#endif
