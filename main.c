#include "header.h"

listplay playerslist=NULL;
listaction actionslist=NULL;

int main(){
	listplay temp;
	int i;
	jmp_buf ebuf;

	srand(time(NULL));
	i=atexit(cleangame);
	if(i)
		fprintf(stderr, "Could not register function cleangame\n");
	
	if (signal(SIGINT,ctrlC) == SIG_ERR)
		perror("signal");
	if (signal(SIGFPE,div0) == SIG_ERR)
		perror("signal");

	i = setjmp(ebuf);
	if(i)
		cleangame();

	if (!getplayers()){
		fprintf(stderr, "Could not start game\n");
		exit(EXIT_FAILURE);
	}
	printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	
	for(temp=playerslist,i=1;temp;temp=temp->next,i++)
		printf("%sPlayer %d : %s\e[39m\n\n", temp->play->color, i, temp->play->name);
	displaylevel(0);

	printf("If you want to recharge the game (after correcting your code\nor because this random level doesn't satisfy you)\nPlease type 'n' and press ENTER\nIf you simply want to start the game, please type 'o'.\nFinally to quit type 'q' : ");
	for(i=fgetc(stdin);i!='n' && i!='o' && i!='q';i=fgetc(stdin))
		if(i!=EOF && i!=' ' && i!='\n')
			printf("Invalid entry, please try again.\n");;
	if(i=='n')
		longjmp(ebuf,1);
	if(i=='q')
		exit(EXIT_SUCCESS);

    start();

	exit(EXIT_SUCCESS);
}