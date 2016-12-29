#include "header.h"

listplay playerslist=NULL;
listaction actionslist=NULL;
jmp_buf ebuf;

int main(){
	listplay temp;
	int i;
    struct sigaction actINT, actFPE;

	srand(time(NULL));
	i=atexit(cleangame);
	if(i)
		fprintf(stderr, "Could not register function cleangame\n");
	
    actINT.sa_handler = ctrlC;
    actFPE.sa_handler = div0;
	if (sigaction(SIGINT, &actINT, NULL))
		perror("signal");
	if (sigaction(SIGFPE, &actFPE, NULL))
		perror("signal");

    
    printf("Would you like to compile players' file (type 'c') or launch the game if you already compiled them (type 'r') ?\nFinally to quit type 'q' : ");
    for(i=fgetc(stdin);i!='c' && i!='r' && i!='q';i=fgetc(stdin))
        if(i!=EOF && i!=' ' && i!='\n')
            printf("Invalid entry, please try again.\n");;
    if(i=='c'){
        if(compile_all()){
            printf("\nSuccessful compilation, you can now restart the game to play with your robots !\n\n\nReloading executable :\n\n");
            system("make");
            exit(EXIT_SUCCESS);
        }
        printf("Compilation has failed\n");
        exit(EXIT_FAILURE);
    }
    if(i=='q')
        exit(EXIT_SUCCESS);
    
    
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