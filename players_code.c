#include "header.h"

char reading=0;
char error=0;


//call a player's code
//Do nothing if the code produce an error (like a division by 0)
void activate(player *joueur){
    
    reading=1;
    current_p = joueur;
    functionlist.p_fct[joueur->number]();
        
    if(error==1){
        fprintf(stderr, "Arithmetic error encountered while reading code of player %s\n", joueur->name);
        error=0;
    }
    reading=0;
    joueur->energy+=2;
    if(joueur->energy>MAX_ENERGY || joueur->onbase)
        joueur->energy=MAX_ENERGY;
}

// add to the actionslist the action called by the robot
void create_action(char action, int arg1, int arg2){
    short priority;
    listaction act;
    
    switch(action){
        case 0: //Snooze
            current_p->energy++;
            return;
        case 1: //TurnAround
            priority=current_p->energy-1;
            act=malloc(sizeof(action));
            act->p=current_p;
            act->priority=current_p->energy-1;
            act->arg_0=arg1;
            act->arg_1=0;
            act->fct=turnaround;
            act->next=NULL;
            add_action(act);
            return;
        case 2: //Shoot
            priority=current_p->energy-3;
            act=malloc(sizeof(action));
            act->p=current_p;
            act->priority=current_p->energy-3;
            act->arg_0=arg1;
            act->arg_1=arg2;
            act->fct=shoot;
            act->next=NULL;
            add_action(act);
            return;
        case 3: //Go
            priority=current_p->energy-3;
            if(arg1 == 1) //FORWARD
                priority++;
            act=malloc(sizeof(action));
            act->p=current_p;
            act->priority=priority;
            act->arg_0=arg1;
            act->arg_1=0;
            act->fct=go;
            act->next=NULL;
            add_action(act);
    }
}
