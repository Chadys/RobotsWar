#include "header.h"

char **level=NULL;
unsigned short nlvl=0;
listbutin butinslist=NULL;
listbutin extrabutinslist=NULL;
//Contents -> nothing=' ', wall='=', treasure='$', base='@', player='^' || '<' || '>' || 'v'

//create the level with one of the level file, choosen depending on the player number
int set_level(int nb_players)
{	
	char *test;
	char *buff = NULL;

	if(nb_players >= 1 && nb_players <= 10)
		buff = strdup("levels/1-10.txt");
	else if(nb_players >= 11 && nb_players <= 25)
		buff = strdup("levels/11-25.txt");
	else if(nb_players >= 26)
		buff = strdup("levels/26-50.txt");
	else{
		fprintf(stderr, "Invalid players number (%d)\n", nb_players);
		return 0;
	}

	if(!buff){
		perror("strdup");
		return 0;
	}

	test = realloc(buff, sizeof(char) * MAX_LETTERS_READ);
	if(!test){
		perror("realloc");
		free(buff);
		return 0;
	}
	buff=test;
	FILE *data = fopen(buff, "r");

	if(!data)
	{
		perror("fopen");
		free(buff);
		return 0;

	}

	fgets(buff, MAX_LETTERS_READ, data);
	int size = strtol(buff, NULL, 10);
	if(size < 1 && size > MAX_LETTERS_READ)
	{
		free(buff);
		fclose(data);
		return 0;
	}

	level = malloc(sizeof(char*) * size);
	if(!level)
	{
		free(buff);
		fclose(data);
		return 0;
	}
	int i, j, c;
	for(i = 0; i < size; i++)
	{
		*(level + i) = malloc(sizeof(char) * size + 1);
		if(!*(level + i))
		{
			free(buff);
			int j;
			for(j = 0; j < i; j++)
				free(*(level + j));
			free(level);
			fclose(data);
			return 0;
		}
		for(j = 0; j < size + 1; j++)
		{
			c = fgetc(data);
			if(c == '\0' || c == '\n')
				break;
			switch(c)
			{
				case '.' : *(*(level + i) + j) = ' ';break;
				default : *(*(level + i) + j) = c;
			}
		}
		*(*(level + i) + size) = '\0';
	}

	free(buff);
	fclose(data);
	return size;
}

//add a base at random on a empty case of the level
coord addrandombase(){
	coord c;

	c.i=rand()%nlvl;
	c.j=rand()%nlvl;
	while(level[c.i][c.j]!=' '){
		c.i=rand()%nlvl;
		c.j=rand()%nlvl;
	}
	level[c.i][c.j]='@';
	c.dir=rand()%4+1;
	return c;
}

//add a treasure at random on a empty case of the level
void addrandombutin(){
	int i,j;
	coord c;
	listbutin temp;

	i=rand()%nlvl;
	j=rand()%nlvl;
	while(level[i][j]!=' '){
		i=rand()%nlvl;
		j=rand()%nlvl;
	}
	level[i][j]='$';
	c.i=i;
	c.j=j;
	c.dir=(rand()%10)+1;
	temp=malloc(sizeof(butin));
	if(!temp){
		fprintf(stderr, "Error in file level.c, line %d\n", __LINE__);
		perror("malloc");
		return;
	}
	temp->data=c;
	temp->onbase=0;
	temp->next=butinslist;
	butinslist=temp;
}

//return the player at the i,j location
player *findplayer(unsigned short i, unsigned short j){
	listplay temp;

	for(temp=playerslist;temp;temp=temp->next){
		if(i==temp->play->loc.i && j==temp->play->loc.j)
			return temp->play;
	}
	return NULL;
}

void displayplayer(player *);
//display the level with all of its content, the players' list with the stats of each and the current action
void displaylevel(int nround){
	unsigned short i,j;
	char c;
	char *etc;
	listplay tmp;
	player *joueur;

	tmp=playerslist;
	for(i=0;i<nlvl/2-2;i++)
		printf("\e[37m _");
	printf(" ROUND : %02hd",nround);
	for(i+=5;i<nlvl;i++)
		printf(" _");
	printf("  Player    Treasures Total Life Energy\n|");
	for(i=0;i<nlvl;i++)
	{
		for(j=0;j<nlvl;j++)
		{
			c=level[i][j];
			if (c == ' ')
				printf("%2c",c);
			else {
				if(c == '=')
					printf("\e[37m%2c",c);
				else {
					if(c == '$')
						printf("\e[33m%2c",c);
					else {
						if(c == '@')
							printf("\e[34m%2c",c);
						else{
							joueur=findplayer(i,j);
							if(joueur==NULL)
								etc="\e[30m";
							else
								etc=joueur->color;
							printf("%s%2c",etc,c);
						}
					}
				}
			}
		}
		printf("\e[37m|");
		if(tmp){
			displayplayer(tmp->play);
			tmp=tmp->next;
		}
		printf("\n|");
	}
	for(i=0;i<nlvl;i++)
		printf(" —");
	if(tmp){
		printf(" ");
		displayplayer(tmp->play);
		tmp=tmp->next;
		printf("\n");
	}
	if(currentaction)
		printf("\n\e[37m%s",currentaction);
	while(tmp){
		for(i=0;i<nlvl+1;i++)
			printf("  ");
		displayplayer(tmp->play);
		tmp=tmp->next;
		printf("\n");
	}
	printf("\e[39m\n");
}
//display a robot name and all its stats
void displayplayer(player * joueur){
	unsigned short i;
	char *etc;

	if(joueur->life==0)
		printf(" \e[90m%-10s DEAD",joueur->name);
	else{
		i=joueur->energy;
		etc=malloc(sizeof(char)*i+1);
		if(!etc){
			fprintf(stderr, "Error in file addplayer.c, line %d\n", __LINE__);
			perror("malloc");
			etc="?";
		}
		else{
			*etc='\0';
			for(;i>0;i--)
				strcat(etc,"+");
		}
		printf(" %s%-11.11s%6hd%9d%5hd%7s\e[37m",joueur->color,joueur->name,joueur->treasure,joueur->score,joueur->life,etc);
		free(etc);
	}
}