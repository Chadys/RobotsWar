#include "header.h"

listplay playerslist=NULL;
listaction actionslist=NULL;
jmp_buf ebuf;



player *current_p;

int main()
{
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

	i = setjmp(ebuf);
	if(i)
		cleangame();

	printf("==============================\n\n");
	int size;
	int nb_need_compile = 0;
	int nb_no_need_compile = 0;
	char **robots_user_files = get_rebot_names_remove_double(&size);
	char **robots_so_files;
	char **robots_c_files;

	get_so_c_files(robots_user_files, size, &robots_so_files, &robots_c_files);

	char *need_compile = get_robots_need_compile(robots_user_files, robots_so_files, size);

	printf("==============================\n\n");
	printf("Following robots need to be compiled : \n\n");
	for(i = 0; i < size; i++)
	{
		if(need_compile[i])
		{
			nb_need_compile++;
			printf(KRED "%s\n" KNRM, robots_user_files[i]);
		}
	}
	printf("\nFollowing robots DON'T need to be compiled : \n\n");
	for(i = 0; i < size; i++)
	{
		if(!need_compile[i])
		{
			nb_no_need_compile++;
			printf(KGRN "%s\n" KNRM, robots_user_files[i]);
		}
	}
	printf("==============================\n\n");
	int recount = 0;
	if(nb_need_compile > 0)
	{
		printf("Some robots can be compiled. Do you want to compile them ? y/n ");
		for(i=fgetc(stdin);i!='y' && i!='n';i=fgetc(stdin))
		{
			if(i!=EOF && i!=' ' && i!='\n')
			{
            	printf("Invalid entry, please try again.\n");
            	continue;
			}
        }
        switch(i)
        {
        	case 'y' : 
        	{
        		recount = 1;
        		compile_needed(&need_compile, size, robots_user_files, robots_so_files, robots_c_files);
        		break;
        	}
        	case 'n':
        	{
        		break;
        	}
        }
    }

    if(recount)
    {
    	nb_no_need_compile = 0;
    	for(i = 0; i < size; i++)
		{
			if(!need_compile[i])
				nb_no_need_compile++;
		}
    }

    // update nb_no_need_compile
    printf("\nCompilations dones ! \n\n");
    int nb_loaded = 0;
    void **handlers = NULL;
    robotfct *robots_functions = NULL;
    char **players_names;
    if(nb_no_need_compile > 2)
    {
    	printf("Loading external libs...\n\n");
    	nb_loaded = load_so_functions(robots_so_files, need_compile, size, &handlers, &robots_functions, &players_names);
    	printf("\nLoaded %d robots !\n", nb_loaded);
    }
    else
    {
    	printf("You don't have enought compiled robots to run the game !\n");
    }
    printf("==============================\n\n");

    int startgame = 0;
    if(nb_loaded >= 2)
    {
    	build_game(players_names, robots_functions, handlers, nb_loaded);

    	for(temp=playerslist,i=1;temp;temp=temp->next,i++)
			printf("%sPlayer %d : %s\e[39m\n\n", temp->play->color, i, temp->play->name);
		displaylevel(0);
    	
		printf("If you want to recharge the game (after correcting your code\nor because this random level doesn't satisfy you)\nPlease type 'n' and press ENTER\nIf you simply want to start the game, please type 'o'.\nFinally to quit type 'q' : ");
		for(i=fgetc(stdin);i!='n' && i!='o' && i!='q';i=fgetc(stdin))
		{
			if(i!=EOF && i!=' ' && i!='\n')
				printf("Invalid entry, please try again.\n");;
		}
		if(i=='n')
			longjmp(ebuf,1);
		if(i=='o')
			startgame = 1;
    	// Le jeu peut commencer !
    	// on a nos fonctions robots_functions
    }

    if(startgame)
    	start();

    free(handlers); // free players already close handlers
    free(robots_functions);

	for(i = 0; i < size; i++)
	{
		free(robots_user_files[i]);
		free(robots_so_files[i]);
		free(robots_c_files[i]);
	}
	free(players_names); // free players already free name strings
	free(robots_user_files);
	free(robots_so_files);
	free(robots_c_files);
	free(need_compile);
}


