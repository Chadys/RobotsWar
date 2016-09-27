#include "header.h"

unsigned short verif_name(char *,listvar);
unsigned short verif_2(char *, listvar, short);
unsigned short verif_cond(char **, int, listvar, short);
unsigned short verif_var(char *, listvar, short);
unsigned short verif_val(char *, listvar, short);
unsigned short verif_num(char *, listvar, short);

//Verify that the code of robot is valid, display the detected error if it isn't
player * verif_code(player *joueur){
	int i, j, pendingif, pendingwhile, pendingelse;
	listvar temp;

	pendingif=0;
	pendingwhile=0;
	pendingelse=0;
	for(i=0;joueur->code[i]!=NULL;i++){
		if(!strcmp(joueur->code[i],"var")){
			if(!verif_name(joueur->code[i+1],joueur->varlist)){
				fprintf(stderr,"Error: Word %d, Invalid variable name\n",i);
				break;
			}
			temp=malloc(sizeof(cellvar));
			if(!temp)
				fprintf(stderr,"Error: Word %d, Could not obtain memory to stock variable\n",i);
			else{
				temp->name=strdup(joueur->code[i+1]);
				if(!temp->name){
					fprintf(stderr,"Error: Word %d, Could not obtain memory to stock variable name\n",i);
					break;
				}
				temp->next=joueur->varlist;
				joueur->varlist=temp;
			}
			joueur->code[i]="\0";
			joueur->code[i+1]="\0";
			i++;
			continue;
		}
		if(!strcmp(joueur->code[i],"Look") || !strcmp(joueur->code[i],"Shoot")){
			j=verif_2(joueur->code[i+1], joueur->varlist,1);
			if(!j){
				fprintf(stderr,"Error: Word %d, Invalid use of function '%s'\n",i,joueur->code[i]);
				break;
			}
			i+=j;
			continue;
		}
		if(!strcmp(joueur->code[i],"TurnAround") || !strcmp(joueur->code[i],"Go")){
			if (!joueur->code[i+1]){
				fprintf(stderr,"Error: Word %d, '%s' needs one argument\n",i,joueur->code[i]);
				break;
			}
			if(!strcmp(joueur->code[i+1],"LEFT") || !strcmp(joueur->code[i+1],"RIGHT") || (!strcmp(joueur->code[i+1],"BACK") && *joueur->code[i] == 'T') || ((!strcmp(joueur->code[i+1],"FORWARD") || !strcmp(joueur->code[i+1],"SPRINT")) && *joueur->code[i] == 'G')){
				i++;
				continue;
			}
			fprintf(stderr,"Error: Word %d, '%s' is an invalid argument for '%s'\n",i,joueur->code[i+1],joueur->code[i]);
			break;
		}
		if(!strcmp(joueur->code[i],"Snooze"))
			continue;
		if(!strcmp(joueur->code[i],"endif")){
			if(pendingif==0){
				fprintf(stderr,"Error: Word %d, 'endingif' without starting 'if'\n",i);
				break;
			}
			pendingif--;
			continue;
		}
		if(!strcmp(joueur->code[i],"endwhile")){
			if(pendingwhile==0){
				fprintf(stderr,"Error: Word %d, '%s' without starting 'while'\n",i,joueur->code[i]);
				break;
			}
			pendingwhile--;
			continue;
		}
		if(!strcmp(joueur->code[i],"endelse")){
			if(!pendingelse){
				fprintf(stderr,"Error: Word %d, '%s' without starting 'else'\n",i,joueur->code[i]);
				break;
			}
			pendingelse--;
			continue;
		} 
		if(!strcmp(joueur->code[i],"break")){
			if(pendingwhile==0){
				fprintf(stderr,"Error: Word %d, '%s' without starting 'while'\n",i,joueur->code[i]);
				break;
			}
			continue;
		} 
		if(!strcmp(joueur->code[i],"continue")){
			if(pendingwhile==0){
				fprintf(stderr,"Error: Word %d, '%s' without starting 'while'\n",i,joueur->code[i]);
				break;
			}
			continue;
		} 
		if(!strcmp(joueur->code[i],"if") || !strcmp(joueur->code[i],"while")){
			j=verif_cond(joueur->code, i+1, joueur->varlist,1);
			if(!j){
				fprintf(stderr,"Error: Word %d, invalid condition for '%s'\n",i,joueur->code[i]);
				break;
			}
			if(*joueur->code[i]=='i')
				pendingif++;
			else if(*joueur->code[i]=='w')
				pendingwhile++;
			i+=j;
			continue;
		}
		if(!strcmp(joueur->code[i],"else")){
			if(!i || strcmp(joueur->code[i-1],"endif")){
				fprintf(stderr,"Error: Word %d, 'else' without 'if'\n",i);
				break;
			}
			pendingelse++;
		}
		if(!verif_var(joueur->code[i], joueur->varlist,1)){
			fprintf(stderr,"Error: Word %d, Invalid variable\n",i);
			break;
		}
		i++;
		if(!joueur->code[i] || strcmp(joueur->code[i],"=")){
			fprintf(stderr,"Error: Word %d, Missing '='\n",i);
			i--;
			break;
		}
		j=verif_val(joueur->code[i+1], joueur->varlist,1);
		if(!j){
			fprintf(stderr,"Error: Word %d, Could not assign value to '%s', invalid value\n",i,joueur->code[i-1]);
			break;
		}
		i+=j;
	}
	if(!joueur->code[i]){
		if(i && !pendingif && !pendingwhile)
			return joueur;
		if(pendingif>0)
			fprintf(stderr,"Error: 'if' never closed\n");
		if(pendingwhile>0)
			fprintf(stderr,"Error: 'while' never closed\n");
		if(pendingelse>0)
			fprintf(stderr,"Error: 'else' never closed\n");
		if(!i)
			fprintf(stderr, "Error: This file is empty\n");
	}
	freeplayer(joueur);
	return NULL;
}


//Each of those function "verif_*" return the number of element it has examined (for example a valid value can be '15' (one element) but it can also be 'Look 15 18' (three elements))

//Validity of a variable name
unsigned short verif_name(char *code,listvar varlist){
	int i;
	listvar temp;

	if(code==NULL){
		fprintf(stderr,"Variable has no name\n");
	}
	if(!isalpha(*code)){
		fprintf(stderr,"Variable '%s' : please start the name of your variable with a letter\n",code);
		return 0;
	}
	for(i=1;i<(int)strlen(code);i++){
		if(!isalnum(code[i]) || code[i]=='_'){
			fprintf(stderr,"Variable '%s' : please use only letter, number or underscore for the name of your variable\n",code);
			return 0;
		}
	}
	for(temp=varlist;temp!=NULL;temp=temp->next){
		if(!strcmp(code,temp->name)){
			fprintf(stderr,"Variable '%s' : this name is already used, please choose another\n",code);
			return 0;
		}
	}
	if (!strcmp(code,"GAMELIMIT") || !strcmp(code,"NORESULT") || !strcmp(code,"NOTHING") || !strcmp(code,"WALL") || !strcmp(code,"TREASURE") || !strcmp(code,"BASE") || !strcmp(code,"ROBOT") || !strcmp(code,"RIGHT") || !strcmp(code,"LEFT") || !strcmp(code,"BACK") || !strcmp(code,"FORWARD") || !strcmp(code,"SPRINT") || !strcmp(code,"VISIONMAX") || !strcmp(code,"Life") || !strcmp(code,"Score")|| !strcmp(code,"Energy")){
		fprintf(stderr,"Variable '%s' : this name is already used for a predefined value, please choose another\n",code);
		return 0;
	}
	if (!strcmp(code,"if") || !strcmp(code,"endif") || !strcmp(code,"else") || !strcmp(code,"endelse") || !strcmp(code,"while") || !strcmp(code,"endwhile") || !strcmp(code,"break") || !strcmp(code,"Snooze") || !strcmp(code,"Look") || !strcmp(code,"Go") || !strcmp(code,"Shoot") || !strcmp(code,"TurnAround")|| !strcmp(code,"var")){
		fprintf(stderr,"Variable '%s' : this name is already used for a language keyword, please choose another\n",code);
		return 0;
	}
	return 1;
}

//Validity of two int arguments
unsigned short verif_2(char *code, listvar varlist, short print){
	unsigned short i,j,k;

	i=verif_num(code, varlist,0);
	if(!i){
		if(print)
			fprintf(stderr,"Invalid first argument\n");
		return 0;
	}
	for(k=i;k>0;k--)
		code=code+strlen(code)+1;
	j=verif_num(code, varlist,0);
	if(!j){
		if(print)
			fprintf(stderr,"Invalid second argument\n");
		return 0;
	}
	return i+j;
}

//Validity of a condition
unsigned short verif_cond(char **code, int mot, listvar varlist, short print){
	unsigned short i,j,k;

	i=verif_val(code[mot], varlist,0);
	if(!i){
		if(print)
			fprintf(stderr,"Invalid first value\n");
		return 0;
	}
	mot+=i;
	if(!code[mot] || (strcmp(code[mot],"<") && strcmp(code[mot],">") && strcmp(code[mot],"==") && strcmp(code[mot],"!=") && strcmp(code[mot],"<=") && strcmp(code[mot],">="))){
		if(print)
			fprintf(stderr,"Invalid comparator, please use '=', '!', '<', '<=', '>' or '>='\n");
		return 0;
	}
	mot++;
	j=verif_val(code[mot], varlist,0);
	if(!j){
		if(print)
			fprintf(stderr,"Invalid second value\n");
		return 0;
	}
	mot+=j;
	k=0;
	if(!strcmp(code[mot],"||") || !strcmp(code[mot],"&&")){
		k=verif_cond(code,mot+1,varlist,print);
		if(!k){
			if(print)
				fprintf(stderr,"Invalid additional condition\n");
			return 0;
		}
		k++;
	}
	k+=i+1+j;
	return k;
}

//Validity of a variable
unsigned short verif_var(char *code, listvar varlist, short print){
	listvar temp;

	temp=findvar(code,varlist);
	if(temp)
			return 1;
	if(print)
		fprintf(stderr, "Could not find a variable called '%s'\n",code);
	return 0;
}

//Validity of an int value
unsigned short verif_val(char *code, listvar varlist, short print){
	unsigned short i;

	if (code==NULL){
		if(print)
			fprintf(stderr, "Missing value\n");
		return 0;
	}
	if(!strcmp(code,"Look")){
		i=verif_2(code+strlen(code)+1,varlist,print);
		if(!i){
			if(print)
				fprintf(stderr, "Misuse of function Look\n");
			return 0;
		}
		return i+1;
	}
	if(verif_var(code,varlist,0))
		return 1;
	if (!strcmp(code,"GAMELIMIT") || !strcmp(code,"NORESULT") || !strcmp(code,"NOTHING") || !strcmp(code,"WALL") || !strcmp(code,"TREASURE") || !strcmp(code,"BASE") || !strcmp(code,"ROBOT") || !strcmp(code,"VISIONMAX") || !strcmp(code,"Life") || !strcmp(code,"Score")|| !strcmp(code,"Energy"))
		return 1;
	if(verif_num(code,varlist,print))
		return 1;
	if(print)
		fprintf(stderr, "Could not convert '%s' to a value\n",code);
	return 0;
}

//Validity of a number or arithmetic operation
unsigned short verif_num(char *code,listvar varlist, short print){
	short par;
	int i,j;
	char *temp;

	par=0;
	if(!isalnum(*code) && *code != '(' && *code != '-'){
		if(print)
			fprintf(stderr, "'%s' isn't a value\n",code);
		return 0;
	}
	for(i=0;i<(int)strlen(code);i++){
		if(isalpha(*(code+i))){ //if there is a variable
			j=strcspn(code+i,"()+*-/");
			temp=strndup(code+i,j);
			if(!temp){
				if(print){
					fprintf(stderr, "Error in file addplayer, line %d\n",__LINE__);
					perror("strndup");
				}
				return 0;
			}
			if(strcmp(temp,"VISIONMAX") && strcmp(temp,"Life") && strcmp(temp,"Score") && strcmp(temp,"Energy") && !verif_var(temp,varlist,1)){
				if(print)
					fprintf(stderr, "'%s' isn't a valid value\n",code);
				free(temp);
				return 0;
			}
			free(temp);
			i+=j-1;
			continue;
		}
		if(isdigit(*(code+i))){
			if(i>0 && *(code+i-1)==')'){
				if(print)
					fprintf(stderr, "Don't use a number after a close parenthesis\n");
				return 0;
			};
			if(i<(int)strlen(code)-1 && *(code+i+1)=='('){
				if(print)
					fprintf(stderr, "Don't use a number before an open parenthesis\n");
				return 0;
			};
			continue;
		}
		if(*(code+i)=='('){	
			if(i>0 && *(code+i-1)==')'){
				if(print)
					fprintf(stderr, "Invalid character before an open parenthesis\n");
				return 0;
			}
			if(i<(int)strlen(code)-1 && (*(code+i+1)=='/' || *(code+i+1)=='*' || *(code+i+1)=='+')){
				if(print)
					fprintf(stderr, "Invalid character after an open parenthesis\n");
				return 0;
			}
			if(i<(int)strlen(code)-1 && *(code+i+1)==')'){
				if(print)
					fprintf(stderr, "Empty parentheses\n");
				return 0;
			}
			par++;
			continue;
		}
		if(*(code+i)==')'){
			if(i>0 && (*(code+i-1)=='+' || *(code+i-1)=='-' || *(code+i-1)=='*' || *(code+i-1)=='/')){
				if(print)
					fprintf(stderr, "Invalid character before a close parenthesis\n");
				return 0;
			}
			if(par==0){
				if(print)
					fprintf(stderr, "Trying to close parentheses without an open parenthesis\n");
				return 0;
			}
			par--;
			continue;
		}
		if (*(code+i)=='+' || *(code+i)=='-' || *(code+i)=='*' || *(code+i)=='/'){
			if(i>0 && (*(code+i-1)=='+' || *(code+i-1)=='-' || *(code+i-1)=='*' || *(code+i-1)=='/')){
				if(print)
					fprintf(stderr, "Double operators\n");
				return 0;
			}
			continue;
		}
		if(print)
			fprintf(stderr, "%c isn't a valid arithmetic symbol\n",*code+i);
		return 0;
	}
	if (*(code+i-1)=='+' || *(code+i-1)=='-' || *(code+i-1)=='*' || *(code+i-1)=='/'){
		if(print)
			fprintf(stderr, "Missing argument for operand %c\n",*(code+i-1));
		return 0;
	}
	if(par>0){
		if(print)
			fprintf(stderr, "Missing a close parenthesis\n");
		return 0;
	}
	return 1;
}

//find a variable in the variables list of a player
listvar findvar(char * code,listvar varlist){
	listvar temp;

	for(temp=varlist;temp;temp=temp->next)
		if(!strcmp(code,temp->name))
			break;;
	return temp;
}
