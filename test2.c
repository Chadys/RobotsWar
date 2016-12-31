#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#ifdef __APPLE__
	#ifndef st_mtime
		#define st_mtime st_mtimespec.tv_sec
	#endif
#endif

listplay playerslist=NULL;
listaction actionslist=NULL;
jmp_buf ebuf;

extern FILE *yyin;

player *current_p;

char **get_rebot_names_remove_double(int *nb_robots)
{
	DIR* rep = NULL;
	rep = opendir(ROBOTSDIR);
	if(!rep)
	{
		fprintf(stderr, "%s\n", strerror(errno));
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
	int len_unallowed_characters = strlen(ALLOWED_CHARACTERS);
	int i = 0, j = 0;
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
				add->val = strndup(fichierLu->d_name, strlen(fichierLu->d_name) - 9);
				add->next = sofiles_head;
				sofiles_head = add;
			}
			else if(!strcmp(lastdot + 1, "robot"))
			{
				//if(isvalid(fichierLu->d_name))
				found = 0;
				i = 0;
				robotname = strndup(fichierLu->d_name, strlen(fichierLu->d_name) - 6);
				if(robotname)
				{
					while(robotname[i])
					{
						found = 0;
						if(i == MAX_ROBOT_NAME)
							break;
						for(j = 0; j < len_unallowed_characters; j++)
						{
							if(ALLOWED_CHARACTERS[j] == robotname[i])
							{
								found = 1;
								break;
							}
						}
						if(!found)
							break;
						i++;
					}
				}
				if(found)
				{
					add = malloc(sizeof(cell));
					add->val = strdup(robotname);
					add->next = robotsfiles_head;
					robotsfiles_head = add;
					(*nb_robots)++;
				}
				else
					printf("Robot name not valid : %s.robot\n", robotname);
				free(robotname);
			}
		}
	}
	ret = malloc(sizeof(char *) * *nb_robots);
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
			if(remove(buff) == -1)
				fprintf(stderr, "%s\n", strerror(errno));
			else
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
	*r_so = malloc(sizeof(char*) * size);
	*r_c = malloc(sizeof(char*) * size);
	int i = 0;
	char *buff;
	int len_robotname = 0;
	for(i = 0; i < size; i++)
	{
		len_robotname = strlen(robots_user_files[i]) - 6;
		(*r_so)[i] = malloc(sizeof(char) * (len_robotname + 4));
		(*r_c)[i] = malloc(sizeof(char) * (len_robotname + 3));
		buff = strndup(robots_user_files[i], len_robotname);
		sprintf((*r_so)[i], "%s.so", buff);
		sprintf((*r_c)[i], "%s.c", buff);
		free(buff);
	}
}

int main()
{
	int size;
	int nb_need_compile = 0;
	int nb_no_need_compile = 0;
	char **robots_user_files = get_rebot_names_remove_double(&size);
	char **robots_so_files;
	char **robots_c_files;
	get_so_c_files(robots_user_files, size, &robots_so_files, &robots_c_files);

	char *need_compile = get_robots_need_compile(robots_user_files, robots_so_files, size);

	int i;
	printf("==============================\n");
	printf("Following robots need to be compiled : \n\n");
	for(i = 0; i < size; i++)
	{
		if(need_compile[i])
		{
			nb_need_compile++;
			printf("%s\n", robots_user_files[i]);
		}
	}
	printf("==============================\n");
	printf("Following robots DON'T need to be compiled : \n\n");
	for(i = 0; i < size; i++)
	{
		if(!need_compile[i])
		{
			nb_no_need_compile++;
			printf("%s\n", robots_user_files[i]);
		}
	}
	printf("==============================\n");

	if(nb_need_compile > 0)
	{
		printf("Would you like to compile robots that need to be compiled ? y/n ");
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
        		printf("Compiling...\n");
        		break;
        	}
        	case 'n':
        	{
        		break;
        	}
        }
    }

    // update nb_no_need_compile
    if(nb_no_need_compile > 2)
    {
    	printf("Entering the game...\n");
    }
    else
    {
    	printf("The game need at least 2 compiled robots\n");
    }

	for(i = 0; i < size; i++)
	{
		free(robots_user_files[i]);
		free(robots_so_files[i]);
		free(robots_c_files[i]);
	}
	free(robots_user_files);
	free(robots_so_files);
	free(robots_c_files);
	free(need_compile);

}
