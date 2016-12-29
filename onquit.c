#include "header.h"

//free every leftover memory allocation
void cleangame(){
	listplay temp;
	listaction tmp;
	listbutin tp;
	int i;

	while(playerslist){
		temp=playerslist->next;
		freeplayer(playerslist->play);
		free(playerslist);
		playerslist=temp;
	}
	while(actionslist){
		tmp=actionslist->next;
		free(actionslist);
		actionslist=tmp;
	}
	while(butinslist){
		tp=butinslist->next;
		free(butinslist);
		butinslist=tp;
	}
	while(extrabutinslist){
		tp=extrabutinslist->next;
		free(extrabutinslist);
		extrabutinslist=tp;
	}
	for(i=0;i<nlvl;i++)
		free(*(level + i));
	if(nlvl)
		free(level);
	if(currentaction && *currentaction)
		free(currentaction);
}

//enable the program to clean itself before a quit with ^⁻C
void ctrlC(int __attribute__ ((unused))sig){
	exit(EXIT_FAILURE);
}

//if a player's code produce a SIGFPE, stop reading it instead of exiting our program
void div0(int __attribute__ ((unused))sig){
	if(reading){
		siglongjmp(sigebuf,1);
	}
	fprintf(stderr, "Arithmetic error\n");
	exit(EXIT_FAILURE);
}