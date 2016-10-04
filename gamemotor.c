#include "header.h"

//game loop : read each player's code wich will fill the actionslist, do each action and stop oi MAX_TURN reached or there are less than 2 player left alive
void start()
{
	unsigned short n_living_player, stillturn;

	struct timespec perturn;
	perturn.tv_nsec = DELAY_TURN;
	perturn.tv_sec = 0;
	
	struct timespec perplayer;
	perplayer.tv_nsec = DELAY_ACTION;
	perplayer.tv_sec = 0;

	n_living_player=0;
	stillturn=0;
	int current_turn = 0;
	listplay tmp;
	listaction actiontmp;

	while(current_turn <= MAX_TURN)
	{
		n_living_player=0;
		for(tmp = playerslist; tmp; n_living_player+=readcode(tmp->play), tmp = tmp->next);

		if(n_living_player<2){
			end(n_living_player);
			return;
		}

		if(!actionslist)
			stillturn++;
		if(stillturn==3){
			end(3);
			return;
		}
		while(actionslist)
		{
			if(actionslist->p->life > 0 && actionslist->p->energy > 0)
			{
				actionslist->fct(actionslist->p, actionslist->arg_0, actionslist->arg_1, actionslist->priority);
			}
			
			printf("\033[H\033[2J");
			displaylevel(current_turn);
			if(*currentaction)
				free(currentaction);
			currentaction = "";
			nanosleep(&perplayer, NULL);
			
			actiontmp = actionslist;
			actionslist = actionslist->next;
			free(actiontmp);
		}
		nanosleep(&perturn, NULL);
		current_turn++;
	}
	end(2);
}

//ending displayer
void end(unsigned short n_living_player){
	listplay temp,winner;
	int gold1,gold2;
	unsigned short draw;

	if(n_living_player<2){
		for(temp=playerslist;temp;temp=temp->next)
			if(temp->play->life>0)
				break;;
		if(temp)
			printf("THE WINNER IS %s%s\e[39m\nCONGRATULATION, YOU KILLER MACHINE !\n",temp->play->color,temp->play->name);
		else
			printf("THIS IS A DRAW ! WHAT A BUTCHERY !"); //should never happen
		return;
	}
	if(n_living_player>2){
		printf("THIS IS A DRAW ! \n Not one bot is doing anything anymore, maybe you need to start making better ones eh ?\n");
		return;
	}
	winner=playerslist;
	while(winner && !winner->play->life)
		winner=winner->next;
	draw=0;
	for(temp=playerslist->next;temp;temp=temp->next){
		gold1=temp->play->score+temp->play->treasure;
		gold2=winner->play->score+winner->play->treasure;
		if (temp->play->life){
			if(gold1>gold2){
				winner=temp;
				draw=0;
			}
			else if (gold1 == gold2)
				draw++;
		}
	}
	if(!draw)
		printf("THE WINNER IS %s%s\e[39m\nCONGRATULATION, YOU GOLD DIGGER !\n",winner->play->color,winner->play->name);
	else
		printf("THIS IS A DRAW ! CONGRATULATION YOU %hd\n",draw+1);
}
