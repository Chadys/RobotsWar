  /* ppcm.l
   analyseur lexical. representation unique des chaines de caracteres
   */
# include "header.h"
char * chaine();
void init_chaine();
int lineno = 1;
static char ** chaines = NULL;
static int nchaines = 0;
init_chaine();

%%
"<="|">="|"!="|"==" { yylval.c = simple_chaine(yytext[0]); return YTEST; }
"&&"|"||"       { yylval.c = simple_chaine(yytext[0]); return YCOND; }

"var"           { return YVAR; }
"Look"          { return YLOOK; }
"Shoot"         { return YSHOOT; }
"TurnAround"    { return YTURN; }
"Go"            { return YGO; }
"Snooze"        { return YSNOOZE; }

"LEFT"|"RIGHT"|"FORWARD"    { yylval.i = getvalue(yytext); return YDIR; }
"SPRINT"        { yylval.i = 5; return YSPRINT; }
"BACK"          { yylval.i = 3; return YBACK; }
"GAMELIMIT"|"NORESULT"|"NOTHING"|"WALL"|"TREASURE"|"BASE"|"ROBOT"|"VISIONMAX"   { yylval.i = getvalue(yytext); return YNUM; }
"Life"          { return YLIFE; }
"Score"         { return YSCORE; }
"Energy"        { return YNRJ; }

"if"            { return YIF; }
"endif"         { return YENDIF; }
"while"         { return YWHILE; }
"endwhile"      { return YENDWHILE; }
"else"          { return YELSE; }
"endelse"       { return YENDELSE; }
"break"|"continue"  { yylval.c = simple_chaine(yytext[0]); return YLOOP; }

(?i:[a-z][a-z0-9_]*)        { yylval.c = chaine(yytext); return YNOM; }
[0-9]+          { yylval.i = atoi(yytext); return YNUM;    }
[-+*/%=()]      { return yytext[0]; }

[ \t\f]         ;
\n              lineno += 1;
.               { fprintf(stderr, "yylex : (%c)\n", yytext[0]); }
%%

/* init_chaine  --  renvoie une representation unique de la chaine argument */
void init_chaine(){
    int i;

    nchaine = 8;
    if((chaine = malloc(nchaines*sizeof(char*)) == NULL);
        fprintf(stderr,"Error: Line 0, Could not obtain enought memory to stock keywords %s\n", s);
    for(i = 0; i<6; i++)
        if((chaine[i] = malloc(3*sizeof(char)) == NULL);
            fprintf(stderr,"Error: Line 0, Could not obtain enought memory to stock keywords %s\n", s);
    if((chaine[6] = malloc(6*sizeof(char)) == NULL);
        fprintf(stderr,"Error: Line 0, Could not obtain enought memory to stock keywords %s\n", s);
    if((chaine[7] = malloc(9*sizeof(char)) == NULL);
        fprintf(stderr,"Error: Line 0, Could not obtain enought memory to stock keywords %s\n", s);
    strcpy(chaines[0], "==");
    strcpy(chaines[1], "!=");
    strcpy(chaines[2], "<=");
    strcpy(chaines[3], ">=");
    strcpy(chaines[4], "&&");
    strcpy(chaines[5], "||");
    strcpy(chaines[6], "break");
    strcpy(chaines[7], "continue");
}

/* chaine  --  renvoie une representation unique de la chaine argument */
char * chaine(char * s){
    void *test;
    int i;

    for(i = 8; i < nchaines ; ++i)
        if (!strcmp(chaines[i], s))
            return chaines[i];
            
    nchaines++;
    if ((test = realloc(chaines, nchaines*sizeof(char*))) == NULL)
        fprintf(stderr,"Error: Line %d, Could not obtain enought memory to stock variable %s\n",lineno, s);
    chaines = test;
    if((chaines[i] = malloc((strlen(s) + 1)*sizeof(char))) == NULL)
        fprintf(stderr,"Error: Line %d, Could not obtain enought memory to stock variable %s\n",lineno, s);
    strcpy(chaines[i], s);
    return chaines[i];
}

/* simple_chaine  --  renvoie une representation unique d'un mot-clef */
char * simple_chaine(char c){
    switch(c){
        case '=':
            return chaine[0]
        case '!':
            return chaine[1]
        case '<':
            return chaine[2]
        case '>':
            return chaine[3]
        case '&':
            return chaine[4]
        case '|':
            return chaine[5]
        case 'b':
            return chaine[6]
        case 'c':
            return chaine[7]
    }
}

/* ggetvalue -- renvoie la valeur correspondante à un mot-clef */
int getvalue(char *keyword){

    if(!strcmp(keyword, "VISIONMAX"))
        return MAX_VISION;
    else if(!strcmp(keyword, "GAMELIMIT"))
        return -2;
    else if(!strcmp(keyword, "NORESULT"))
        return -1;
    else if(!strcmp(keyword, "NOTHING"))
        return 0;
    else if(!strcmp(keyword, "WALL") || !strcmp(keyword, "FORWARD"))
        return 1;
    else if(!strcmp(keyword, "TREASURE") || !strcmp(keyword, "RIGHT"))
        return 2;
    else if(!strcmp(keyword, "BASE"))
        return 3;
    else if(!strcmp(keyword, "ROBOT") || !strcmp(keyword, "LEFT"))
        return 4;
    return INT_MIN;
}

int yywrap(){
    if(chaines!=NULL)
        free(chaines);
    return 1;
}
