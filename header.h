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
#define MAX_LIFE 3 //starting number of a robot's life
#define MAX_VISION 10 //range of a robot's field of view
#define MAX_ENERGY 6 //starting number of a robot's energy units

#define MAX_TURN 50 //number of turn max in a game
#define MAX_TIMER 2000 //interval of read word in a player code before the robot lose 1 energy unit
#define DELAY_ACTION 150 //pause delay between each player's action
#define DELAY_TURN 100 //pause delay between each game turn


/* coords */
struct coord{
	unsigned short i; //y
	unsigned short j; //x
	short dir;
};
typedef struct coord coord;
//Direction -> north=1, east=2, south=3, west=4

/* variable list */
struct cellvar{
	char* name;
	int val;
	struct cellvar* next;
};
typedef struct cellvar cellvar;
typedef cellvar * listvar;

/* players */
struct player{
	char *color; //sûrement un code en hexa, il faudra voir les couleurs prédéfinies de sdl sinon
	char *name;
	unsigned short treasure;
	unsigned int score;
	unsigned short life;
	unsigned short energy;
	listvar varlist;
	coord loc;
	char onbase;
	char **code;
	char *text;
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


/* int list */
struct cellint
{
	int data;
	struct cellint *next;
};
typedef struct cellint cellint;
typedef struct cellint * listint;


/* globals */
extern listplay playerslist; //list of players
extern listaction actionslist; //list of players' actions for this turn
extern listbutin extrabutinslist; //extra treasures after a player is killed
extern listbutin butinslist; //all treasure on the map
extern char **level; //the level and all it contains
extern unsigned short nlvl; //size of the level
extern char * currentaction; //string used to display an action being done
extern char reading; //indicate if a player's code is being read (for managment of signals)
extern char error; //indicate if a player's code provoqued an error (to be able to stop reading it)


/* prototypes */

/* gamemotor.c */
//the game loop and handler of the actions list, and the ending displayer
void start();
void end(unsigned short);

/* getplayer.c */
//construct a player per valid code in ROBOTSDIR and add it to the game
char getplayers();
void freeplayer(player *);

/* verifcode.c */
//syntax analyzer of a code, display eventual first error found
player * verif_code(player *);
listvar findvar(char *,listvar);

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

/* interpreter.c */
//the interpreter of a player's code
unsigned short readcode(player *);

/* vision.c */
//return what a robot can see a a specific case
coord ligne(coord, int, int);

/* eval.c */
//to calculate a value with a string
int eval(player *, int);
int eval_math(player *, char *);

/* onquit.c */
//free every allocated memory of the game
void cleangame();
void ctrlC(int);
void div0(int);

#endif
