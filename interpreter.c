#include "header.h"

char reading=0;
char error=0;

//test wether one condition is true, return words count of condition, negative or positive depending on trueness
int cond_is_ok(player *joueur,int i){
	int x,y;
	int comp;
	char *test;

	x=eval(joueur,i);
    if(!strcmp(joueur->code[i], "Look"))
    	comp=3;
    else
    	comp=1;
    test=joueur->code[i+comp];
    comp++;
    i+=comp;
    y=eval(joueur,i);
    if(!strcmp(joueur->code[i], "Look"))
    	comp+=3;
    else
    	comp+=1;
    switch(*test){
    	case '=' :
    		if(x==y)
    			return comp;
    		return -comp;
    	case '!' :
    		if(x!=y)
    			return comp;
    		return -comp;
    	case '<' :
    		if(*(test+1) == '='){
    			if(x<=y)
    				return comp;
    			return -comp;
    		}
    		if(x<y)
    			return comp;
    		return -comp;
    	case '>' :
    		if(*(test+1) == '='){
    			if(x>=y)
    				return comp;
    			return -comp;
    		}
    		if(x>y)
    			return comp;
    		return -comp;
    }
    return -comp;
}


//test wether all conditions of a test are true, return word count of conditions tested, positive if true, negative if not
int cond_are_ok(player *joueur,int i){
	int taille, total;

	for(taille = cond_is_ok(joueur,i), total=abs(taille), i+=abs(taille) ;; i++, taille = cond_is_ok(joueur,i), total+=1+abs(taille), i+=abs(taille)){
		if(*joueur->code[i]=='|'){
			if(taille>0)
				return total;
			else
				continue;
		}
		if(*joueur->code[i]=='&'){
			if(taille<0)
				return -total;
			else
				continue;
		}
		return taille>0 ? total : -total;
	}
}

//count the number of words of all conditions of a test
unsigned short compte_cond(player *joueur,int i){
	unsigned short taille;

    taille=3;
    if(!strcmp(joueur->code[i], "Look")){
    	taille+=2;
    	i+=2;
    }
    i+=2;
    if(!strcmp(joueur->code[i], "Look")){
    	taille+=2;
    	i+=2;
    }
    i++;
    if(strcmp(joueur->code[i], "||") && strcmp(joueur->code[i], "&&"))
    	return taille;
    taille++;
    return taille+compte_cond(joueur,i+1);
}

//read through a player's code, add to the actionslist the first action if fall on.
//Do nothing if robot is dead, the code produce an error (like a division by 0) or the code took to long to process (avoid infinite loop)
//return 1 if player is alive, 0 if dead (useful to count the number of robots left alive)
unsigned short readcode(player *joueur){
	int timer,i,j;
	int pendingif,pendingwhile,pendingelse;
	listint pilewhile,temp;
	listvar var;
	listaction act;
	short priority,taillevaleur;

	timer=0;
	pilewhile=NULL;
	if(!joueur->life){
		while(pilewhile){
			temp = pilewhile->next;
			free(pilewhile);
			pilewhile = temp;
		}
		return 0;
	}
	reading=1;
	for(i=0;joueur->code[i];i++,timer++){
		if(error==1) //if player's code produced an error
			break;
		if(timer>=MAX_TIMER){ //energy slowly decrease as time pass
			joueur->energy--;
			timer-=MAX_TIMER;
		}
		if(!joueur->energy){
			fprintf(stderr, "An infinite loop was found or too much time was taken while parsing code of player %s\n", joueur->name);
			break;
		}
		if(*(joueur->code[i]) =='\0'){
			i++;
			timer++;
			continue;
		}
		if(!strcmp(joueur->code[i],"if")){ //if == true, go after condition, else go after corresponding endif
			taillevaleur = cond_are_ok(joueur,i+1);
		 	if (taillevaleur>0){
		 		i+=taillevaleur;
		 		timer+=taillevaleur;
		 		continue;
		 	}
		 	for(pendingif=0,j=i+(-taillevaleur)+1;strcmp(joueur->code[j],"endif") || pendingif>0;j++){
		 		if(!strcmp(joueur->code[j],"if"))
		 			pendingif++;
		 		else if(!strcmp(joueur->code[j],"endif"))
		 			pendingif--;
		 	}
		 	i=j;
		 	timer++;
		 	if(joueur->code[i+1] && !strcmp(joueur->code[i+1],"else")){
		 		i++;
		 		timer++;
		 	}
		 	continue;
		}
		if(!strcmp(joueur->code[i],"endif"))
			continue; 
		if(!strcmp(joueur->code[i],"else")){
			for(pendingelse=0,j=i+1;strcmp(joueur->code[j],"endelse") || pendingelse>0;j++){
		 		if(!strcmp(joueur->code[j],"else"))
		 			pendingelse++;
		 		else if(!strcmp(joueur->code[j],"endelse"))
		 			pendingelse--;
		 	}
		 	i=j;
		 	timer++;
			continue;
		}
		if(!strcmp(joueur->code[i],"endelse"))
			continue; 
		if(!strcmp(joueur->code[i],"while")){
			taillevaleur = cond_are_ok(joueur,i+1);
		 	if (taillevaleur>0){
				if(!pilewhile || pilewhile->data!=i){
					temp=malloc(sizeof(cellint));
					temp->data=i;
					temp->next=pilewhile;
					pilewhile=temp;
				}
				i+=taillevaleur;
				timer+=taillevaleur;
				continue;
		 	}
		 	else{
		 		if(pilewhile && pilewhile->data==i){
		 			temp=pilewhile;
		 			pilewhile=pilewhile->next;
		 			free(temp);
		 		}
		 		for(pendingwhile=0,j=i+(-taillevaleur)+1;strcmp(joueur->code[j],"endwhile") || pendingwhile>0;j++){
		 			if(!strcmp(joueur->code[j],"while"))
		 				pendingwhile++;
		 			else if(!strcmp(joueur->code[j],"endwhile"))
		 				pendingwhile--;
		 		}
		 		i=j;
		 		timer++;
		 		continue;
		 	}
		}
		if(!strcmp(joueur->code[i],"endwhile")){
			if(pilewhile){
				i=pilewhile->data-1;
				timer++;
			}
			continue;
		}
		if(!strcmp(joueur->code[i],"break")){
			if(pilewhile){
		 		temp=pilewhile;
		 		pilewhile=pilewhile->next;
		 		free(temp);
		 	}
		 	for(pendingwhile=0,j=i+1;strcmp(joueur->code[j],"endwhile") || pendingwhile>0;j++){
		 		if(!strcmp(joueur->code[j],"while"))
		 			pendingwhile++;
		 		else if(!strcmp(joueur->code[j],"endwhile"))
		 			pendingwhile--;
		 	}
		 	i=j;
		 	timer++;
		 	continue;
		}
		if(!strcmp(joueur->code[i],"continue")){
			if(pilewhile){
				i=pilewhile->data-1;
				timer++;
			}
			continue;
		}
		if(!strcmp(joueur->code[i],"Look")){
			eval(joueur,i);
			i+=2;
			timer+=2;
			continue;
		}
		if(!strcmp(joueur->code[i],"Snooze")){
			joueur->energy++;
			break;
		}
		if(!strcmp(joueur->code[i],"TurnAround")){
			priority=joueur->energy-1;
			act=malloc(sizeof(action));
			act->p=joueur;
			act->priority=priority;
			act->arg_0=eval(joueur,i+1);
			act->arg_1=0;
			act->fct=turnaround;
			act->next=NULL;
			add_action(act);
			break;
		}
		if(!strcmp(joueur->code[i],"Shoot")){
			priority=joueur->energy-3;
			act=malloc(sizeof(action));
			act->p=joueur;
			act->priority=priority;
			act->arg_0=eval(joueur,i+1);
			act->arg_1=eval(joueur,i+2);
			act->fct=shoot;
			act->next=NULL;
			add_action(act);
			break;
		}
		if(!strcmp(joueur->code[i],"Go")){
			priority=joueur->energy-3;
			if(!strcmp("FORWARD",joueur->code[i+1]))
				priority++;
			act=malloc(sizeof(action));
			act->p=joueur;
			act->priority=priority;
			act->arg_0=eval(joueur,i+1);
			act->arg_1=0;
			act->fct=go;
			act->next=NULL;
			add_action(act);
			break;
		}
		if(!strcmp(joueur->code[i],"||") || !strcmp(joueur->code[i],"&&")){
			i+=compte_cond(joueur, i+1);
			continue;
		}
		var=findvar(joueur->code[i],joueur->varlist);
		var->val=eval(joueur,i+2);
		if(!strcmp(joueur->code[i+2], "Look")){
			i+=4;
			timer+=4;
			continue;
		}
		i+=2;
		timer+=2;
	}
	if(error==1){
		fprintf(stderr, "Arithmetic error encountered while parsing code of player %s\n", joueur->name);
		error=0;
	}
	reading=0;
	joueur->energy+=2;
	if(joueur->energy>MAX_ENERGY || joueur->onbase)
		joueur->energy=MAX_ENERGY;
	while(pilewhile){
		temp = pilewhile->next;
		free(pilewhile);
		pilewhile = temp;
	}
	return 1;
}
