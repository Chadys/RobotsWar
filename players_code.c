#include "header.h"

char reading=0;
sigjmp_buf sigebuf;

//call a player's code
//Do nothing if the code produce an error (like a division by 0)
void activate(player *joueur){
    int i;
    
    reading=1;
    current_p = joueur;
    i = sigsetjmp(sigebuf, 1);
    if(i)
        fprintf(stderr, "Arithmetic error encountered while reading code of player %s\n", joueur->name);
    else
        functionlist.p_fct[joueur->number]();
    reading=0;
    joueur->energy+=2;
    if(joueur->energy>MAX_ENERGY || joueur->onbase)
        joueur->energy=MAX_ENERGY;
}

// add to the actionslist the action called by the robot
void create_action(char choice, int arg1, int arg2){
    short priority;
    listaction act;
    
    switch(choice){
        case SNOOZE:
            current_p->energy++;
            return;
        case TURNAROUND:
            priority=current_p->energy-1;
            act=malloc(sizeof(action));
            if(!act){
                fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                perror("malloc");
                return;
            }
            act->p=current_p;
            act->priority=current_p->energy-1;
            act->arg_0=arg1;
            act->arg_1=0;
            act->fct=turnaround;
            act->next=NULL;
            add_action(act);
            return;
        case SHOOT:
            priority=current_p->energy-3;
            act=malloc(sizeof(action));
            if(!act){
                fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                perror("malloc");
                return;
            }
            act->p=current_p;
            act->priority=current_p->energy-3;
            act->arg_0=arg1;
            act->arg_1=arg2;
            act->fct=shoot;
            act->next=NULL;
            add_action(act);
            return;
        case GO:
            priority=current_p->energy-3;
            if(arg1 == 1) //FORWARD
                priority++;
            act=malloc(sizeof(action));
            if(!act){
                fprintf(stderr, "Error in file getplayers.c, line %d\n", __LINE__);
                perror("malloc");
                return;
            }
            act->p=current_p;
            act->priority=priority;
            act->arg_0=arg1;
            act->arg_1=0;
            act->fct=go;
            act->next=NULL;
            add_action(act);
    }
}

char update_energy(unsigned int *action_read){
    if (*action_read >= MAX_TIMER){
        current_p->energy--;
        *action_read=0;
        if (!current_p->energy){
            fprintf(stderr, "An infinite loop was found or too much time was taken while parsing code of player %s\n", current_p->name);
            return 0;
        }
    }
    return 1;
}