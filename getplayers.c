#include "header.h"

FILE * include_player_fct;
FILE * get_players_fonc;
player *current_p;
FILE * current_p_file;
listfunction functionlist;
extern FILE *yyin;

//return all the names of the .robot files
char **getplayersfiles(char mode)
{
	int i, nplayers;
    char **ret, *buff;

    nplayers = 0;
    if(!mode)
        system("(ls -1 " ROBOTSDIR " | grep '.robot$') > " TMP_FILE);
    else
        system("(ls -1 | grep '_robot.c$') > " TMP_FILE);
	
	FILE *RSTREAM = fopen(TMP_FILE, "r");
	
	if(!RSTREAM)
	{
		fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
		perror("fopen");
		return NULL;
	}
	
	ret = malloc(sizeof(char *) * MAX_ROBOTS);
	if (!ret){
		fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
		perror("malloc");
		return NULL;
	}
	buff = malloc(sizeof(char) * MAX_FILE_NAME);
	if (!buff){
		fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
		perror("malloc");
		free(ret);
		return NULL;
	}
	for(i = 0; i <  MAX_ROBOTS; i++)
	{
		if(!fgets(buff, MAX_FILE_NAME, RSTREAM))
			break;
		*(ret + i) = strndup(buff,strlen(buff)-1); //delete the \n at the end
        nplayers++;
	}
	if(i<MAX_ROBOTS)
		*(ret + i)=NULL;
	if(fgets(buff, MAX_FILE_NAME, RSTREAM))
		fprintf(stderr, "Max number of files reached (%d), some files have been ignored.\n", MAX_ROBOTS);
	free(buff);
	fclose(RSTREAM);
	remove(TMP_FILE);
    
    if(mode){
        nlvl=set_level(nplayers);
        if(!nlvl){
            fprintf(stderr, "The level couldn't be created.\n");
            return 0;
        }
    }
	return ret;
}

char compile(char *);
char compile_all(){
    char **players;
    int i,j;

    j=0;
    players = getplayersfiles(0);
    if (!players)
        return 0;
    
    include_player_fct = fopen("include_player_fct.h", "w");
    if(!include_player_fct){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("fopen");
        for(i = 0; i < MAX_ROBOTS && *(players + i); i++)
            free(*(players+i));
        free(players);
        return 0;
    }
    get_players_fonc = fopen("get_players_fonc.c", "w");
    if(!get_players_fonc){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("fopen");
        fclose(include_player_fct);
        for(i = 0; i < MAX_ROBOTS && *(players + i); i++)
            free(*(players+i));
        free(players);
        return 0;
    }
    fprintf(get_players_fonc, "#include \"header.h\"\n#include \"include_player_fct.h\"\n\n\nvoid init_functionlist(){\n\tfunctionlist.n = 0;\n");
    
    for(i = 0; i < MAX_ROBOTS; i++){
        if(!*(players + i))
            break;
        if(strlen(*(players + i))==6){
            fprintf(stderr, "A file has no player name before the .robot extension\n");
            free(*(players+i));
            continue;
        }
        if(compile(*(players+i)))
            j++;
            
        free(*(players+i));
    }
    /*TODO nettoyer hash table */
    free(players);
    fprintf(get_players_fonc, "}");
    fclose(include_player_fct);
    fclose(get_players_fonc);
    if(i<2){
        fprintf(stderr, "\nThis game needs a least two files\n");
        return 0;
    }
    if(j<2){
        fprintf(stderr, "\nNot enought valid players have been found. Please correct your codes and try compiling again.\n");
        return 0;
    }
    return 1;
}

char compile(char *source_filename){
    char *name, *dest_filename, *temp;
    int r;
    static unsigned short num = 0;
    FILE *sourcefile;
    size_t length;
    
    length = strlen(source_filename);
    name=strndup(source_filename,length-6); //copy the filename without .robot
    if(!name){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("strndup");
        return 0;
    }
    printf("\nAnalysing %s's file :\n",name);
    dest_filename = malloc((length+3)*sizeof(char)); //add size for ".c" and NUL
    if(!dest_filename){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        free(name);
        return 0;
    }
    strcpy(dest_filename, name);
    strcpy(dest_filename+length-6, "_robot.c");
    
    temp=malloc((length+strlen(ROBOTSDIR)+2)*sizeof(char)); //to concatenate the path to playerfile and the player file name separated by "/", and the terminating NUL
    if (!temp){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        free(name);
        free(dest_filename);
        return 0;
    }
    strcpy(temp,ROBOTSDIR);
    strcat(temp,"/");
    strcat(temp,source_filename);
    
    sourcefile = fopen(temp, "r");
    free(temp);
    if(!sourcefile){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("fopen");
        free(name);
        free(dest_filename);
        return 0;
    }
    
    current_p_file = fopen(dest_filename, "w");
    if(!current_p_file){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("fopen");
        free(name);
        free(dest_filename);
        fclose(sourcefile);
        return 0;
    }
    fprintf(current_p_file, "#include \"header.h\"\n\nvoid %s(){\n", name);
    yyin = sourcefile;
    r =  yyparse();
    fclose(current_p_file);
    fclose(sourcefile);
    if(r){
        remove(dest_filename);
        printf("Player's file not added, errors have been encountered\n");
        free(dest_filename);
        free(name);
        return 0;
    }
    fprintf(get_players_fonc, "\tfunctionlist.p_fct[%d] = %s;\n\tfunctionlist.n++;\n", num, name);
    fprintf(include_player_fct, "void %s();\n", name);
    num++;
    printf("Ok!\n");
    free(dest_filename);
    free(name);
    return 1;
}

int create_player(char *, char *, int, coord);
//add each valid player to the game
char getplayers(){
	char **players;
	int i,j, nplayers;
	char *COLORS[11]={"\e[31m","\e[94m","\e[93m","\e[32m","\e[35m","\e[36m","\e[91m","\e[92m","\e[95m","\e[96m","\e[97m"};
	coord c;

	j=0;
    nplayers = 0;
	players = getplayersfiles(1);
	if (!players)
		return 0;
    
	for(i = 0; i < MAX_ROBOTS; i++){
		if(!*(players + i))
			break;
		c=addrandombase();
        
		if(create_player(*(players + i), COLORS[j], i, c)){
			j++;
			if(j==11)
				j=0;
			switch(c.dir){
				case 1 :
					level[c.i][c.j]='^';
					break;
				case 2 :
					level[c.i][c.j]='>';
					break;
				case 3 :
					level[c.i][c.j]='v';
					break;
				case 4 :
					level[c.i][c.j]='<';
			}
			addrandombutin();
		}
		free(*(players+i));
        nplayers++;
	}
	free(players);
	if(i<2){
		fprintf(stderr, "\nThis game needs a least two files\n");
		return 0;
	}
	if(!playerslist || !playerslist->next){
		fprintf(stderr, "\nNot enought valid players have been found. Too many error were encountered.\n");
		return 0;
	}
	for(;i < nlvl; i++){
		addrandombase();
	}
	return 1;
}


 // create a player if the code in the file is valid and add it to the global list
int create_player(char * nomjoueur, char * color, int num, coord c){
	player *joueur;
	listplay add;

    printf("\nLoading %s's file :\n",nomjoueur);
	joueur=malloc(sizeof(player));
	if(!joueur){
		fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
		perror("malloc");
		return 0;
	}

	joueur->color=color;
	joueur->name=strndup(nomjoueur,strlen(nomjoueur)-8); //copy the filename without _robot.c
	if (!joueur->name){
		fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
		perror("strndup");
		free(joueur);
		return 0;
	}
	joueur->treasure=0;
	joueur->score=0;
	joueur->life=MAX_LIFE;
	joueur->energy=MAX_ENERGY;
	joueur->onbase=1;
	joueur->varlist=NULL;
	joueur->loc=c;
    joueur->number = (unsigned short) num;
    add=malloc(sizeof(cellplay));
    if (!add){
        fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
        perror("malloc");
        freeplayer(joueur);
        return 0;
    }
    add->play=joueur;
    add->next=playerslist;
    playerslist=add;
    num++;
    return 1;
}

//free all memory used by a player
void freeplayer(player *joueur){
	listvar temp;

	while(joueur->varlist){
		temp=joueur->varlist->next;
		free(joueur->varlist->name);
		free(joueur->varlist);
		joueur->varlist=temp;
	}
	free(joueur->name);
	free(joueur);
}
