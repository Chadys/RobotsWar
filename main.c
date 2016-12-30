#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#ifdef __APPLE__
	#ifndef st_mtime
		#define st_mtime st_mtimespec.tv_sec
	#endif
#endif

listplay playerslist=NULL;
listaction actionslist=NULL;
jmp_buf ebuf;


extern FILE *yyin;

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



	void **so_handlers = NULL;
	struct stat attr;
	char **robots_names = NULL;
	char **robots_user_files = NULL;
	char **robots_c_files = NULL;
	char **robots_so_files = NULL;
	long time_user_file = 0;
	long time_so_file = 0;
	int *robots_compiled = NULL;
	int *robots_need_compile = NULL;
	int curr_compiled = 0;
	int curr_need_compile = 0;
	int len_robots_dir = strlen(ROBOTSDIR);
	int len_robot_name = 0;
	so_handlers = malloc(sizeof(void *) * MAX_ROBOTS);
	robots_names = getplayersfiles(0);
	robots_user_files = malloc(sizeof(char *) * MAX_ROBOTS);
	robots_c_files = malloc(sizeof(char *) * MAX_ROBOTS);
	robots_so_files = malloc(sizeof(char *) * MAX_ROBOTS);
	robots_compiled = malloc(sizeof(int) * MAX_ROBOTS);
	robots_need_compile = malloc(sizeof(int) * MAX_ROBOTS);
	for(i=0;i<MAX_ROBOTS;i++){robots_compiled[i]=-1;robots_need_compile[i]=-1;}
	
	for(i = 0; i < MAX_ROBOTS; i++){
		if(!(robots_names[i]))
            break;
        len_robot_name = strlen((robots_names[i]));
        if(len_robot_name <= 6)
			continue;
		robots_user_files[i] = malloc(sizeof(char) * (len_robots_dir + len_robot_name + 2));
        robots_so_files[i] = malloc(sizeof(char) * (len_robot_name + 8));
        sprintf(robots_user_files[i], ROBOTSDIR "/%s", robots_names[i]);
        sprintf(robots_so_files[i], "./%s.so", robots_names[i]);
        robots_names[i] = strndup(robots_names[i], len_robot_name - 6);
        
        
        if(stat(robots_user_files[i], &attr) != -1)
        {
        	time_user_file = attr.st_mtime;
        	if(stat(robots_so_files[i], &attr) != -1)
        		time_so_file = attr.st_mtime;
        	else
        		time_so_file = time_user_file - 1; // to compile it
        }
        else
        	continue;

        if(time_user_file > time_so_file)
        {
        	robots_c_files[curr_need_compile] = malloc(sizeof(char) * (len_robot_name + 3));
        	sprintf(robots_c_files[i], "%s.c", robots_user_files[i]);
        	robots_need_compile[curr_need_compile++] = i; 
        }
        else
			robots_compiled[curr_compiled++] = i;

			
        /*
        printf("ROBOT %3d : %s -> %s\n", i, robots_user_files[i], robots_so_files[i]);
			valid_robots[valid++] = i;
			*/
    }
    /*
    for(i = 0; i < MAX_ROBOTS; i++){
		if(!(robots_names[i]))
            break;
        printf("%s : %s : %s : %s\n", robots_names[i], robots_user_files[i], robots_c_files[i], robots_so_files[i]);
    }
    */

    
    printf("Les robots suivants n'ont pas besoin d'être compilés : \n");
    for(i = 0; i < curr_compiled; i++)
    	printf("%s\n", robots_names[robots_compiled[i]]);
    printf("==========\n");
    printf("Les robots suivants vont être compilés : \n");
    for(i = 0; i < curr_need_compile; i++)
    	printf("%s\n", robots_names[robots_need_compile[i]]);// -> %s -> %s\n", robots_names[robots_need_compile[i]], robots_c_files[robots_need_compile[i]], robots_so_files[robots_need_compile[i]]);
    printf("==========\n");
	
	FILE *sourcefile, *current_p_file;
	int r = 0;
	htable = init_hash(htable.n);
    for(i = 0; i < curr_need_compile; i++)
    {
    	printf("Compiling %s...\n", robots_names[robots_need_compile[i]]);
    	sourcefile = NULL;
    	current_p_file = NULL;
    	sourcefile = fopen(robots_user_files[robots_need_compile[i]], "r");
    	current_p_file = fopen(robots_c_files[robots_need_compile[i]], "w");
    	if(!sourcefile || !current_p_file)
    	{
    		printf("error while opening %s's files\n", robots_names[robots_need_compile[i]]);
    		if(sourcefile)fclose(sourcefile);
    		if(current_p_file)fclose(current_p_file);
    		continue;
    	}
    	yyin = sourcefile;
    	printf("now\n");
    	r =  yyparse();
    	fclose(sourcefile);
    	fclose(current_p_file);
    }

    /*
	A PARTIR DE LA : 
	-il faut compiler en C les nouveaux robots (index robots_need_compile)
	(les robots_so_files sont déjà de type : "./R2D2.robot.so")

	quand on a les 'name.robot.c' on doit faire :

	-le compiler en .so :
	system("gcc name.robot.c --shared -fPIC -o name.robot.so")
	on vérifie que le fichier a bien été créer :
	if(stat(robots_so_files[i], &attr) != -1)) ça peut arriver si notre compilateur pour C a écrit nimp
	
	-on charge les différents .so
	handlers[i] = dlopen(robots_so_file[i], RTLD_LAZY);
	on vérifie que dlopen a bien marché :
	if(!handle[i])
	{
		fprintf (stderr, "%s\n", dlerror());
        exit(1);
	}

	-on récupère le pointeur sur la fonction du robot:
	 dlerror() // on "fflush" les erreurs/warning qu'il peut y avoir dans dlerror
	fonction[i] = dlsym(handle[i], "proceed"); ça sera plus simple que chaque robot ait le même nom pour sa fonction dans le .c
    on vérifie que ça a marché :
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s\n", error);
        exit(1);
    }

    -a la fin du programme on libère les .so :
    dlclose(handle);
    */




    printf("Would you like to compile new robots (type 'c') or launch the game with compilled robots ?\nFinally to quit type 'q' : ");
    for(i=fgetc(stdin);i!='c' && i!='r' && i!='q';i=fgetc(stdin))
        if(i!=EOF && i!=' ' && i!='\n')
            printf("Invalid entry, please try again.\n");;
    if(i=='c'){
        if(compile_all()){
            printf("\nSuccessful compilation, you can now restart the game to play with your robots !\n\n\nReloading executable :\n\n");
            system("make");
            execl("./robotswar", "robotswar", NULL);
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