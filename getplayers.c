#include "header.h"
player *current_p = NULL;
hashtable keywords;

//return all the names of the .robot files
char **getplayersfiles()
{
	int nplayers;

	nplayers=0;
	system("(ls -1 " ROBOTSDIR " | grep '.robot$') > " TMP_FILE);
	
	FILE *RSTREAM = fopen(TMP_FILE, "r");
	
	if(!RSTREAM)
	{
		fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
		perror("fopen");
		return NULL;
	}
	
	int i;
	char **ret = malloc(sizeof(char *) * MAX_ROBOTS);
	if (!ret){
		fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
		perror("malloc");
		return NULL;
	}
	char *buff = malloc(sizeof(char) * MAX_FILE_NAME);
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


	nlvl=set_level(nplayers);
	if(!nlvl){
		fprintf(stderr, "The level couldn't be created.\n");
		for(i=0;i<MAX_ROBOTS && *(ret + i);i++)
			free(*(ret + i));
		free(ret);
		return NULL;
	}
	return ret;
}

int create_player(FILE *, char *, char *, coord);
//add each valid player to the game
char getplayers(){
	char **players;
	int i,j;
	FILE * fd;
	char *temp;
	char *COLORS[11]={"\e[31m","\e[94m","\e[93m","\e[32m","\e[35m","\e[36m","\e[91m","\e[92m","\e[95m","\e[96m","\e[97m"};
	coord c;

	j=0;
    keywords = init_hash(10);
    if(!keywords.n){
        fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
        perror("init_hash");
        return 0;
    }
	players = getplayersfiles();
	if (!players){
        free_hash(keywords);
		return 0;
    }
	for(i = 0; i < MAX_ROBOTS; i++){
		if(!*(players + i))
			break;
		c=addrandombase();
		if(strlen(*(players + i))==6){
			fprintf(stderr, "A file has no player name before the .robot extension\n");
			free(*(players+i));
			continue;
		}
		temp=malloc((strlen(*(players + i))+strlen(ROBOTSDIR)+2)*sizeof(char)); //to concatenate the path to playerfile and the player file name separated by "/", and the terminating NUL
		if (!temp){
			fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
			perror("malloc");
			free(*(players+i));
			continue;
		}
		strcpy(temp,ROBOTSDIR);
		strcat(temp,"/");
		strcat(temp,*(players + i));
		fd = fopen(temp, "r+");
		free(temp);
		if(!fd){
			fprintf(stderr, "Error in file getplayers.c, line %d, while opening file %s\n", __LINE__,*(players + i));
			perror("fopen");
			free(*(players+i));
			continue;
		}
		if(create_player(fd, *(players + i),COLORS[j],c)){
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
		fclose(fd);
		free(*(players+i));
	}
	free(players);
	if(i<2){
		fprintf(stderr, "\nThis game needs a least two files\n");
		return 0;
	}
	if(!playerslist || !playerslist->next){
		fprintf(stderr, "\nNot enought valid players have been found. Please correct your codes and try again.\n");
		return 0;
	}
	for(;i < nlvl; i++){
		c=addrandombase();
	}
	return 1;
}


 // create a player if the code in the file is valid and add it to the global list
int create_player(FILE * fd, char * nomjoueur, char * color, coord c){
	char *ligne,*buffer, *test;
	player *joueur;
	listplay add;
	unsigned int cap;
	size_t sizecode,sizebuf;

	sizecode=0;
	cap=MAX_LETTERS_READ;
	printf("\nAnalysing file %s :\n",nomjoueur);
	ligne=calloc(cap+1,sizeof(char));
	if (!ligne){
		fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
		perror("calloc");
		return 0;
	}
	buffer=calloc(cap+1,sizeof(char));
	if (!buffer){
		fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
		perror("calloc");
		free(ligne);
		return 0;
	}
	while(fgets(buffer, MAX_LETTERS_READ, fd)){
		sizebuf=strlen(buffer);
		if(sizecode+2>UINT_MAX-sizebuf){
			fprintf(stderr,"Error: code is too long\n");
			free(ligne);
			free(buffer);
			return 0;
		}
		if(sizebuf+sizecode+2>cap){
			cap*=2;
			test=realloc(ligne,cap*sizeof(char)+1);
			ligne=test;
			if (!test){
				fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
				perror("realloc");
				free(ligne);
				free(buffer);
				return 0;
			}
			memset(ligne+cap/2,'\0',cap/2);
		}
		strcat(ligne,buffer);
		sizecode+=sizebuf;
	}
	free(buffer);

	joueur=malloc(sizeof(player));
	if(!joueur){
		fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
		perror("malloc");
		free(ligne);
		return 0;
	}
	joueur->code=ligne;
    joueur->sizecode = sizecode+2;
	joueur->color=color;
	joueur->name=strndup(nomjoueur,strlen(nomjoueur)-6); //copie the filename without .robot
	if (!joueur->name){
		fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
		perror("strndup");
		free(joueur);
		free(ligne);
		free(joueur->code);
		return 0;
	}
	joueur->treasure=0;
	joueur->score=0;
	joueur->life=MAX_LIFE;
	joueur->energy=MAX_ENERGY;
	joueur->onbase=1;
	joueur->variables=init_hash(20);
    if(!joueur->variables.n){
        fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
        perror("init_hash");
        free(joueur);
        free(ligne);
        free(joueur->code);
        free(joueur->name);
        return 0;
    }
	joueur->loc=c;
    check_init_parser(joueur->code, joueur->sizecode);
	if(!checkparse()){
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
		printf("Ok!\n");
		return 1;
	}
	printf("Player not added, errors have been encountered\n");
    freeplayer(joueur);
	return 0;
}

//free all memory used by a player
void freeplayer(player *joueur){
    
	free(joueur->name);
	free(joueur->code);
    free_hash(joueur->variables);
	free(joueur);
}