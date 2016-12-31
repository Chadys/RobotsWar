#include "header.h"

char * currentaction="";
listaction _actionslist = NULL;

//add an action to the global actionslist for this turn
void add_action(listaction A)
{
	if(!actionslist)
	{
		actionslist = A;
		return;
	}

	listaction tmp = actionslist;
	listaction before = NULL;
	while(tmp)
	{
		if(tmp->priority < A->priority)
		{
			if(!before)
			{
				A->next = tmp;
				actionslist = A;
				return;
			}
			A->next = tmp;
			before->next = A;
			return;
		}
		else if(tmp->priority == A->priority)
		{
			listaction tmp2 = tmp->next;
			int count = 1;
			while(tmp2)
			{
				if(tmp2->priority == A->priority)
					count++;
				else
					break;
				tmp2 = tmp2->next;
			}
			//count contient le nombre d'actions qui ont la même priorité que A à partir de tmp
			int random = (rand() % (count + 1)); // +1 car on peut insérer avant tmp
			
			if(random == 0)
			{
				A->next = tmp;
				if(!before)
				{
					actionslist = A;
					return;
				}
				before->next = A;
				return;

			}
			tmp2 = tmp;
			for(count = 1; count < random; count++)
			{
				tmp2 = tmp2->next;
			}
			// on insèrt après tmp2;

			A->next = tmp2->next; // tmp2->next peut être NULL
			tmp2->next = A;
			return;
		}
		before = tmp;
		tmp = tmp->next;
	}
	// tmp = NULL et l'action n'a pas été ajoutée.
	before->next = A;
}

//make a robot changes direction
void turnaround(player * joueur, int newdir,int __attribute__ ((unused))useless, short able){
	char *arg;
	int length;
	void* test;

	if(able>=0){
		switch(newdir){
			case 2 :
				switch(joueur->loc.dir){
					case 1 :
						joueur->loc.dir=2;
						level[joueur->loc.i][joueur->loc.j]='>';
						break;
					case 2 :
						joueur->loc.dir=3;
						level[joueur->loc.i][joueur->loc.j]='v';
						break;
					case 3 :
						joueur->loc.dir=4;
						level[joueur->loc.i][joueur->loc.j]='<';
						break;
					case 4 :
						joueur->loc.dir=1;
						level[joueur->loc.i][joueur->loc.j]='^';
				}
				arg="RIGHT";
				break;
			case 4 :
				switch(joueur->loc.dir){
					case 1 :
						joueur->loc.dir=4;
						level[joueur->loc.i][joueur->loc.j]='<';
							break;
					case 2 :
						joueur->loc.dir=1;
						level[joueur->loc.i][joueur->loc.j]='^';
						break;
					case 3 :
						joueur->loc.dir=2;
						level[joueur->loc.i][joueur->loc.j]='>';
						break;
					case 4 :
						joueur->loc.dir=3;
						level[joueur->loc.i][joueur->loc.j]='v';
				}
				arg="LEFT";
				break;
			case 3 :
				switch(joueur->loc.dir){
					case 1 :
						joueur->loc.dir=3;
						level[joueur->loc.i][joueur->loc.j]='v';
						break;
					case 2 :
						joueur->loc.dir=4;
						level[joueur->loc.i][joueur->loc.j]='<';
						break;
					case 3 :
						joueur->loc.dir=1;
						level[joueur->loc.i][joueur->loc.j]='^';
						break;
					case 4 :
						joueur->loc.dir=2;
						level[joueur->loc.i][joueur->loc.j]='>';
				}
				arg="BACK";
		}
		joueur->energy--;
	}
	else
		switch(newdir){
			case 2 :
				arg="RIGHT";
				break;
			case 4 :
				arg="LEFT";
				break;
			case 3 :
				arg="BACK";
		};

	length=strlen(joueur->name)+21; //color+name+" : TurnAround " and '\0'
	length+=strlen(arg);
	currentaction=malloc(sizeof(char)*length);
	if(!currentaction){
		fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
		perror("malloc");
		currentaction="";
	}
	else{
		strcpy(currentaction,joueur->color);
		strcat(currentaction,joueur->name);
		strcat(currentaction," : TurnAround ");
		strcat(currentaction,arg);
	}
	if(able<0){
		test=realloc(currentaction, sizeof(char)*(length+23));
		if(!test){
			fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
			perror("realloc");
			return;
		}
		currentaction=test;
		strcat(currentaction," !! Not enought ENERGY");
	}
}


void addbutinjoueur(player*);
void findandrenewbutin(player *, unsigned short, unsigned short);

//make a robot changes location
void go(player * joueur, int newcase,int __attribute__ ((unused))useless, short able){
	unsigned short nrj;
	coord dest,destbis;
	int prof,larg;
	char *arg;
	int length;
	void* test;

	nrj=2;
	switch(newcase){
		case 1 :
			prof=1;
			larg=0;
			arg="FORWARD";
			break;
		case 2 :
			prof=1;
			larg=1;
			arg="RIGHT";
			nrj++;
			break;
		case 4 :
			prof=1;
			larg=-1;
			arg="LEFT";
			nrj++;
			break;
		case 5 :
			prof=2;
			larg=0;
			arg="SPRINT";
			nrj++;
	}

	length=strlen(joueur->name)+13; //color+name+" : Go " and '\0'
	length+=strlen(arg);
	currentaction=malloc(sizeof(char)*length);
	if(!currentaction){
		fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
		perror("malloc");
		currentaction="";
	}
	else{
		strcpy(currentaction,joueur->color);
		strcat(currentaction,joueur->name);
		strcat(currentaction," : Go ");
		strcat(currentaction,arg);
	}

	if(able>=0){
		dest=ligne(joueur->loc,prof,larg);
		destbis=ligne(joueur->loc,1,0);
		if(dest.dir==-1 || dest.dir==-2 || dest.dir==1 || dest.dir==4)
			return;
		if(joueur->onbase){
			level[joueur->loc.i][joueur->loc.j]='@';
			joueur->onbase=0;
		}
		else
			level[joueur->loc.i][joueur->loc.j]=' ';
		joueur->loc.i=dest.i;
		joueur->loc.j=dest.j;
		if(destbis.dir==3 && *arg!='F'){
			joueur->energy=MAX_ENERGY;
			joueur->score+=joueur->treasure;
			joueur->treasure=0;
		}
		else if(destbis.dir==2 && *arg!='F'){
			findandrenewbutin(joueur,destbis.i,destbis.j);
			if(joueur->onbase)
				joueur->onbase=0;
		}
		if(dest.dir==3){
			joueur->onbase=1;
			joueur->energy=MAX_ENERGY;
			joueur->score+=joueur->treasure;
			joueur->treasure=0;
		}
		else if(dest.dir==2)
			findandrenewbutin(joueur,dest.i,dest.j);
		switch(joueur->loc.dir){
			case 1 :
				level[dest.i][dest.j]='^';
				break;
			case 2 :
				level[dest.i][dest.j]='>';
				break;
			case 3 :
				level[dest.i][dest.j]='v';
				break;
			case 4 :
				level[dest.i][dest.j]='<';
		}
		joueur->energy-=nrj;
	}
	else{
		test=realloc(currentaction, sizeof(char)*length+23);
		if(!test){
			fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
			perror("realloc");
			return;
		}
		currentaction=test;
		strcat(currentaction," !! Not enought ENERGY");
	}
}

// teleport a player that has been hit to a random empty location
void replaceplayer(player *joueur){
	int i,j;
	
	i=rand()%nlvl;
	j=rand()%nlvl;
	while(level[i][j]!=' '){
		i=rand()%nlvl;
		j=rand()%nlvl;
	}
	joueur->loc.i=i;
	joueur->loc.j=j;
	joueur->loc.dir=(rand()%4)+1;
	switch(joueur->loc.dir){
		case 1 :
			level[i][j]='^';
			break;
		case 2 :
			level[i][j]='>';
			break;
		case 3 :
			level[i][j]='v';
			break;
		case 4 :
			level[i][j]='<';
	}
}

//what happens to a robot that has been hit
void isshooted(coord bot){
	player * joueur;

	joueur=findplayer(bot.i,bot.j);
	if (joueur){
		addbutinjoueur(joueur);
		if(joueur->life)
			replaceplayer(joueur);
		else
			joueur->score=0;
	}
}

//make a robot shoots and call isshooted if a robot is hit, display a little animation
void shoot(player * joueur, int prof,int larg, short able)
{
	coord res;
	char arg_0[32], arg_1[32];
	int length;
	char* str;
	struct timespec perplayer;
	void* test;
	
	length=strlen(joueur->name)+16; //color+name+" : Shoot " and '\0'
	length+=sprintf(arg_0, "%d", prof)+1;
	length+=sprintf(arg_1, "%d", larg);
	currentaction=malloc(sizeof(char)*length);
	if(!currentaction){
		fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
		perror("malloc");
		currentaction="";
	}
	else{
		strcpy(currentaction,joueur->color);
		strcat(currentaction,joueur->name);
		strcat(currentaction," : Shoot ");
		strcat(currentaction,arg_0);
		strcat(currentaction," ");
		strcat(currentaction,arg_1);
	}

	if(able>=0){
		res=ligne(joueur->loc,prof,larg);
		if(res.dir>-1)
		{
			joueur->energy-=3;	

			str = malloc(sizeof(char) * 50);
			if(str){
				system("tput sc");
				sprintf(str, "tput cup %d %d", res.i + 1, (res.j * 2) + 1);
				system(str);
				sprintf(str, "echo %s[%c]", joueur->color,level[res.i][res.j]);
				system(str);
				free(str);
				perplayer.tv_nsec = DELAY_ACTION;
				perplayer.tv_sec = 0;
				nanosleep(&perplayer, NULL);
			}
			else{
                fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
                perror("malloc");
            }
		
			if(res.dir==4)
				isshooted(res);
		}
	}
	else{
		test=realloc(currentaction, sizeof(char)*length+23);
		if(!test){
			fprintf(stderr, "Error in file actions.c, line %d\n", __LINE__);
			perror("realloc");
			return;
		}
		currentaction=test;
		strcat(currentaction," !! Not enought ENERGY");
	}
}

//add on the map the treasure left by a hit robot
void addbutinjoueur(player * joueur){
	listbutin temp;

	if(joueur->treasure>0){
		temp=malloc(sizeof(butin));
		if(!temp){
			fprintf(stderr, "Error in file level.c, line %d\n", __LINE__);
			perror("malloc");
			return;
		}
		temp->data.i=joueur->loc.i;
		temp->data.j=joueur->loc.j;
		temp->data.dir=joueur->treasure;
		temp->onbase=joueur->onbase;
		temp->next=extrabutinslist;
		extrabutinslist=temp;
		level[joueur->loc.i][joueur->loc.j]='$';
	}
	else{
		if (joueur->onbase)
			level[joueur->loc.i][joueur->loc.j]='@';
		else
			level[joueur->loc.i][joueur->loc.j]=' ';
	}
	joueur->treasure=0;
	joueur->onbase=0;
	if(joueur->life)
		joueur->life--;
	joueur->energy=0;
}

//add the coresponding value of a treasure to the robot that found it, replace elsewhere if it wasn't a robot's treasure
void findandrenewbutin(player *joueur, unsigned short i, unsigned short j){
	listbutin temp, prev;

	for(temp=extrabutinslist,prev=temp;temp;prev=temp,temp=temp->next)
		if(temp->data.i == i && temp->data.j == j){
			joueur->treasure+=temp->data.dir;
			joueur->onbase=temp->onbase;
			if(temp->onbase)
				level[i][j]='@';
			else
				level[i][j]=' ';
			if(temp==extrabutinslist)
				extrabutinslist=temp->next;
			else
				prev->next=temp->next;
			free(temp);
			return;
		};
	for(temp=butinslist,prev=temp;temp;prev=temp,temp=temp->next)
		if(temp->data.i == i && temp->data.j == j){
			joueur->treasure+=temp->data.dir;
			level[i][j]=' ';
			if(temp==butinslist)
				butinslist=temp->next;
			else
				prev->next=temp->next;
			free(temp);
			addrandombutin();
			return;
		};
	joueur->treasure+=10;
}
