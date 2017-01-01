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
#include <dlfcn.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#ifdef __APPLE__
	#ifndef st_mtime
		#define st_mtime st_mtimespec.tv_sec
	#endif
#endif


#define TMP_FILE "tmplist.txt" //name given to a temporary file
#define MAX_ROBOTS 50 //max number of players
#define ROBOTSDIR "robots" //name of the directory containing players' files
#define MAX_FILE_NAME 256 //max number of letters of a file
#define MAX_ROBOT_NAME 100
#define MAX_LETTERS_READ 100 //max number of letter read at once
#define MAX_TAB 15 //max number of tabulation that can be printed to a player's compiled file
#define MAX_LIFE 3 //starting number of a robot's life
#define MAX_VISION 10 //range of a robot's field of view
#define MAX_ENERGY 6 //starting number of a robot's energy units
#define MAX_TURN 50 //number of turn max in a game
#define MAX_TIMER 500 //interval of read word in a player code before the robot lose 1 energy unit
#define DELAY_ACTION 150000000 //pause delay between each player's action
#define DELAY_TURN 100000000 //pause delay between each game turn


#define KNRM  "\x1B[0m"
#define KGRY  "\x1B[30;1m"
#define KRED  "\x1B[31;1m"
#define KGRN  "\x1B[32;1m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34;1m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

# define YCHECKTIMER(N) timer+=N; if(!update_energy(&timer)) return; //in players' file, check the timer and exit the function if too much time was taken

typedef void (*robotfct)(void);

/* coords */
struct coord{
	unsigned short i; //y
	unsigned short j; //x
	short dir;
};
typedef struct coord coord;
//Direction -> north=1, east=2, south=3, west=4

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
    unsigned short number;
    robotfct fct;
    void *handler;
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


/* Hash table and hash cell */
typedef struct cell
{
    char *val;
    struct cell *next;
}cell;
typedef cell* llist;

typedef struct hashtable
{
    int n;
    int p;
    llist *alveole;
}hashtable;

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
extern char reading; //indicate if a player's code is being read (for managment of signals)
extern player *current_p; //current player whose code is being compiled
extern jmp_buf ebuf; //to make jump
extern sigjmp_buf sigebuf; //to jump from signals
extern hashtable htable; //hash table for the lexical analyser


/* prototypes */





/* gamemotor.c */
//the game loop and handler of the actions list, and the ending displayer
void start();
void end(unsigned short);

/* getplayer.c */
//construct a player per valid code in ROBOTSDIR and add it to the game
char create_player(char * nomjoueur, char * color, unsigned short num, coord c, void *handler, robotfct function);
char build_game(char **p_names, robotfct *functions, void **handlers, int nb_players);
void freeplayer(player *);
char **get_rebot_names_remove_double(int *nb_robots); //return all valid .robot path and remove .so that don't have .robot associated
char *get_robots_need_compile(char **robots_user_files, char **robots_so_files, int size); // return a boolean array that indicates all .so files that needs to be compiled
void get_so_c_files(char **robots_user_files, int size, char ***r_so, char ***r_c); // get .c path and .so path from all .robot path
void compile_needed(char **need_compile, int nb_compile, int size, char **r_robot, char **r_so, char **r_c); // compile all files that are marked as compillable
int load_so_functions(char **r_so, char *need_compile, int size, void ***handlers, robotfct **functions, char ***names); // load .so libraries files in memory and grab all 'proceed' functions to make them available. also fill indexes tab to all loaded robots indexes

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
void create_action(char, int, int);
char update_energy(unsigned int *);

/* compiler.tab.c */
// compiler's parsing
int yyparse();

/* get_players_fonc.c */
// inititalisation of function list, only do something if the compiler has been launched
void init_functionlist();

/* hash_table.c */
hashtable init_hash(int);
void free_hash(hashtable);

#endif
