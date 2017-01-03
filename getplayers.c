#include "header.h"

player *current_p;
extern FILE *yyin;

char valid_name(const char *name){
    size_t i, len;
    
    len = strlen(name);
    if(!isalpha(name[0]) && name[0] != '_')
        return 0;
    for(i=1; i<len; i++)
        if(name[i] != '_' && !isalnum(name[i]))
            return 0;
    return 1;
}

char **get_rebot_names_remove_double(int *nb_robots)
{
    DIR* rep = NULL;
    rep = opendir(ROBOTSDIR);
    if(!rep)
    {
        fprintf(stderr, ROBOTSDIR " : %s\n", strerror(errno));
        return NULL;
    }
    char **ret = NULL;
    llist sofiles_head = NULL;
    llist robotsfiles_head = NULL;
    llist add = NULL;
    char *lastdot;
    char *robotname;
    struct dirent* fichierLu = NULL;
    *nb_robots = 0;
    int i = 0;
    int found = 0;
    while((fichierLu = readdir(rep)) != NULL)
    {
        if(*nb_robots == MAX_ROBOTS)
            break;
        lastdot = strrchr(fichierLu->d_name, '.');
        if(lastdot)
        {
            if(!strcmp(lastdot + 1, "so"))
            {
                add = malloc(sizeof(cell));
                if(!add){
                    fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                    perror("malloc");
                    return NULL;
                }
                add->val = strndup(fichierLu->d_name, strlen(fichierLu->d_name) - 9);
                if(!add->val){
                    fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                    perror("strndup");
                    return NULL;
                }     
                add->next = sofiles_head;
                sofiles_head = add;
            }
            else if(!strcmp(lastdot + 1, "robot"))
            {
                i = 0;
                robotname = strndup(fichierLu->d_name, strlen(fichierLu->d_name) - 6);
                if(!robotname){
                    fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                    perror("strndup");
                    return NULL;
                }     
                if(valid_name(robotname))
                {
                    add = malloc(sizeof(cell));
                    if(!add){
                        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                        perror("malloc");
                        return NULL;
                    }
                    add->val = strdup(robotname);
                    if(!add->val){
                        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                        perror("strdup");
                        return NULL;          
                    }
                    add->next = robotsfiles_head;
                    robotsfiles_head = add;
                    (*nb_robots)++;
                }
                else
                    printf(KGRY "Ignored robot : %s.robot (Unvalid name)\n" KNRM, robotname);
                free(robotname);
            }
        }
    }
    ret = malloc(sizeof(char *) * *nb_robots);
    if(!ret){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        return NULL;
    }
    llist ptr;
    llist tmp;
    llist ptr2;
    char buff[300];
    ptr = sofiles_head;
    while(ptr)
    {
        tmp = ptr;
        
        ptr2 = robotsfiles_head;
        found = 0;
        while(ptr2 && !found)
        {
            if(!strcmp(ptr2->val, ptr->val))
                found = 1;
            ptr2 = ptr2->next;
        }
        if(!found)
        {
            sprintf(buff, ROBOTSDIR "/%s.robot.so", ptr->val);
            if(remove(buff) != -1)
                printf("Removed file : '%s'\n", buff);
        }
        
        ptr = ptr->next;

        free(tmp->val);
        free(tmp);
    }
    ptr = robotsfiles_head;
    i = 0;
    while(ptr)
    {
        tmp = ptr;
        
        sprintf(buff, ROBOTSDIR "/%s.robot", ptr->val);
        ret[i] = strdup(buff);
        if(!ret[i]){
            fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
            perror("strdup");
            free(tmp->val);
            free(tmp);
            return NULL;          
        }
        
        ptr = ptr->next;
        free(tmp->val);
        free(tmp);
        i++;
    }
    free(rep);
    return ret;
}

char *get_robots_need_compile(char **robots_user_files, char **robots_so_files, int size)
{
    char *ret = NULL;
    ret = malloc(sizeof(char) * size);
    if(!ret){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        return NULL;
    }
    int i = 0;
    struct stat attr;
    long time_user_file;
    long time_so_file;
    for(i = 0; i < size; i++)
    {
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
            ret[i] = 1;
        else
            ret[i] = 0;
    }
    return ret;
}

void get_so_c_files(char **robots_user_files, int size, char ***r_so, char ***r_c)
{
    int i,j;
    *r_so = malloc(sizeof(char*) * size);
    if(!*r_so){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        return;
    }
    *r_c = malloc(sizeof(char*) * size);
    if(!*r_c){
        free(*r_so);
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        return;
    }
    i = 0;
    char *buff;
    int len_robotname = 0;
    for(i = 0; i < size; i++)
    {
        len_robotname = strlen(robots_user_files[i]) - 6;
        (*r_so)[i] = malloc(sizeof(char) * (len_robotname + 4));
        if(!(*r_so)[i]){
            fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
            for(j=i-1 ; j>=0 ; j--){
                free((*r_so)[j]);
                free((*r_c)[j]);
            }
            free(*r_so);
            free(*r_c);
            perror("malloc");
            return;
        }
        (*r_c)[i] = malloc(sizeof(char) * (len_robotname + 3));
        if(!(*r_c)[i]){
            fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
            for(j=i-1 ; j>=0 ; j--){
                free((*r_so)[j]);
                free((*r_c)[j]);
            }
            free((*r_so)[i]);
            free(*r_so);
            free(*r_c);
            perror("malloc");
            return;
        }
        buff = strndup(robots_user_files[i], len_robotname);
        if(!buff){
            fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
            perror("strndup");
            return;
        }        
        sprintf((*r_so)[i], "%s.so", buff);
        sprintf((*r_c)[i], "%s.c", buff);
        free(buff);
    }
}

void compile_needed(char **need_compile, int nb_compile, int size, char **r_robot, char **r_so, char **r_c)
{
    int i;
    FILE *sourcefile, *current_p_file;
    int r = 0;
    hashtable htable = init_hash(nb_compile * 50);
    if(!htable.n){
        fprintf(stderr, "Error in file getplayer.c, line %d\n", __LINE__);
        perror("init_hash");
        return;
    }
    char call[300];
    struct stat attr;
    for(i = 0; i < size; i++)
    {
        if(need_compile[0][i])
        {
            printf("\n" KBLU "Compiling %s..." KNRM " \n", r_robot[i]);
            sourcefile = fopen(r_robot[i], "r");
            if(!sourcefile)
            {
                fprintf(stderr, "%s : %s\n", r_robot[i], strerror(errno));
                continue;
            }
            current_p_file = fopen(r_c[i], "w");
            if(!current_p_file)
            {
                fprintf(stderr, "%s : %s\n", r_c[i], strerror(errno));
                continue;
            }
            yyin = sourcefile;
            fprintf(current_p_file, "#include \"../header.h\"\n\nvoid proceed(){\nunsigned int timer = 0;\n");
            r =  yyparse(current_p_file, htable);
            fclose(sourcefile);
            fclose(current_p_file);
            if(r)
            {
                printf(KRED "fail !\n" KNRM);
                remove(r_c[i]);
                continue;
            }
            sprintf(call, "gcc %s --shared -fPIC -o %s", r_c[i], r_so[i]);
            system(call);
            if(stat(r_so[i], &attr) != -1)
            {
                need_compile[0][i] = 0;
                printf(KGRN "OK !\n" KNRM);
            }
            remove(r_c[i]);
        }
    }
    free_hash(htable);
}

int load_so_functions(char **r_so, char *need_compile, int size, void ***handlers, robotfct **functions, char ***r_names)
{
    *handlers = malloc(sizeof(void*) * size);
    if(!*handlers){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        return 0;
    }
    *functions = malloc(sizeof(robotfct) * size);
    if(!*handlers){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        free(*handlers);
        perror("malloc");
        return 0;
    }
    *r_names = malloc(sizeof(char *) * size);
    if(!*r_names){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        free(*handlers);
        free(*functions);
        perror("malloc");
        return 0;
    }
    int ret = 0;
    int i;
    char *error;
    char *point;
    char *slash;
    for(i = 0; i < size; i++)
    {
        if(!need_compile[i])
        {
            handlers[0][ret] = NULL;
            handlers[0][ret] = dlopen(r_so[i], RTLD_LAZY);
            if(handlers[0][ret])
            {
                dlerror();
                functions[0][ret] = dlsym(handlers[0][ret], "proceed");
                if ((error = dlerror()) != NULL)
                {
                    fprintf (stderr, "%s\n", error);
                    dlclose(functions[0][ret]);
                    free(error);
                }
                else
                {
                    printf(KGRN "Successfully loaded %s !\n" KNRM, r_so[i]);
                    point = strrchr(r_so[i] , '.');
                    slash = strrchr(r_so[i], '/') + 1;
                    r_names[0][ret] = strndup(r_so[i] + (slash - r_so[i]), point - r_so[i] - ((slash - r_so[i])));
                    if(!r_names[0][ret]){
                        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                        free(*handlers);
                        free(*functions);
                        free(*r_names);
                        perror("strndup");
                        return 0;
                    }                    
                    ret++;
                }
            }
            else
            {
                printf(KRED "Could not load %s !\n" KNRM, r_so[i]);
            }
        }
    }
    return ret;
}

char build_game(char **p_names, robotfct *functions, void **handlers, int nb_players)
{
    char *COLORS[11]={"\e[31m","\e[94m","\e[93m","\e[32m","\e[35m","\e[36m","\e[91m","\e[92m","\e[95m","\e[96m","\e[97m"};
    coord c;
    int i, j = 0;
    nlvl = set_level(nb_players);
    if(!nlvl){
        fprintf(stderr, "The level couldn't be created.\n");
        return 0;
    }

    for(i = 0; i < nb_players; i++)
    {
        c = addrandombase();
        
        if(create_player(p_names[i], COLORS[j], i, c, handlers[i], functions[i]))
        {
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
    }
    for(;i < nlvl; i++){
        addrandombase();
    }
    return 1;

}

 // create a player if the code in the file is valid and add it to the global list
char create_player(char * nomjoueur, char * color, unsigned short num, coord c, void *handler, robotfct function){
	player *joueur;
	listplay add;

	joueur=malloc(sizeof(player));
	if(!joueur){
		fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
		perror("malloc");
        printf("Failed.\n");
		return 0;
	}

	joueur->color=color;
	joueur->name=nomjoueur;
	joueur->treasure=0;
	joueur->score=0;
	joueur->life=MAX_LIFE;
	joueur->energy=MAX_ENERGY;
	joueur->onbase=1;
	joueur->loc=c;
    joueur->number = num;
    joueur->handler = handler;
    joueur->fct = function;
    add=malloc(sizeof(cellplay));
    if (!add){
        fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
        perror("malloc");
        freeplayer(joueur);
        printf("Failed.\n");
        return 0;
    }
    add->play=joueur;
    add->next=playerslist;
    playerslist=add;
    return 1;
}

//free all memory used by a player
void freeplayer(player *joueur){

	free(joueur->name);
    dlclose(joueur->handler);
	free(joueur);
}