%{
#include "header.h"
#include "compiler.tab.h"
%}
%option noyywrap yylineno nounput noinput
%x jump_if jump_else do_else jump_while do_while

    int getvalue(char *);
    void yy_change_start_condition(int);
    void yy_leave_start_condition();
    #define YY_DECL int yylex(player *joueur, hashtable keywords)
    static unsigned int letter_index;
    #define YY_USER_ACTION letter_index+=yyleng;
    static unsigned int nif;
    static unsigned int nelse;
    static unsigned int nwhile;
    static whilestack pilewhile = NULL;
    static statestack pilestate = NULL;
    
%%

<*>[ \t\f\n]+       ;

"else"              { yy_change_start_condition(jump_else); }

<do_while>{
    "if"            { pilewhile->unclosed_if++; return YIF; }
    "endif"         { pilewhile->unclosed_if--; return YENDIF; }
    "endwhile"      { return YENDWHILE; }
    "continue"      {   if(pilewhile->unclosed_if){
                            pilewhile->unclosed_if--;
                            yy_rewind();
                            return YENDIF;
                        }
                        return YENDWHILE;
                    }
    "break"         {   if(pilewhile->unclosed_if){
                            pilewhile->unclosed_if--;
                            yy_rewind();
                            return YENDIF;
                        }
                        yy_change_start_condition(jump_while);
                    }
}

<INITIAL,do_else,do_while>{
    "<="|">="|"!="|"==" { yylval.s = get_or_insert(keywords, yytext)->name; return YTEST; }
    "&&"|"||"       { yylval.s = get_or_insert(keywords, yytext)->name; return YCOND; }

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
    "while"         { yylval.i = letter_index-5; return YWHILE; }

    (?i:[a-z][a-z0-9_]*)        { yylval.c = get_or_insert(joueur->variables, yytext); return YNOM; }
    [0-9]+          { yylval.i = atoi(yytext); return YNUM;    }
    [-+*/%=()<>,]   { return yytext[0]; }
    .               { fprintf(stderr, "yylex : Unrecognised symbol (%c)\n", yytext[0]); return UNRECOGNISED; }
}

<jump_if>{
    "endif"[ \t\f\n]*/"else" {  if(!nif){
                                    yy_change_start_condition(do_else);
                                    return YENDIF;
                                }
                                nif--;
                            }
    "endif"     {   if(!nif){
                        yy_leave_start_condition();
                        return YENDIF;
                    }
                    nif--;
                }
    "if"        { nif++; }
    .           ;
}

<do_else,jump_else>{
    "else"      { nelse++; }
    "endelse"   {   if(!nelse)
                        yy_leave_start_condition();
                    else
                        nelse--;
                }
}

<jump_else>.           ;


<jump_while>{
    "while"     { nwhile++; }
    "endwhile"  {   if(!nwhile){
                        yy_leave_start_condition();
                        return YENDWHILE;
                    }
                    nwhile--;
                }
}

%%

/* getvalue -- renvoie la valeur correspondante à un mot-clef */
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

void yy_init_parser(char *code, size_t size){
    
    letter_index = 0;
    nif = 0;
    nelse = 0;
    nwhile = 0;
    BEGIN(INITIAL);
    
    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(!yy_scan_buffer(code, size)){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("yy_scan_buffer");
    }
}
void yy_clean(){
    whilestack tmp;
    statestack tmp2;
    
    while(pilewhile){
        tmp = pilewhile;
        pilewhile = pilewhile->next;
        free(tmp);
    }
    while(pilestate){
        tmp2 = pilestate;
        pilestate = pilestate->next;
        free(tmp2);
    }
}
void yy_flush(){
    YY_FLUSH_BUFFER;
}

void yy_change_start_condition(int cond){
    statestack tmp;
    
    tmp = malloc(sizeof(statecell));
    if(!tmp){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("malloc");
        return;
    }
    tmp->state = YY_START;
    tmp->next = pilestate;
    pilestate = tmp;
    BEGIN(cond);
}

void yy_leave_start_condition(){
    statestack tmp;
    
    if(!pilestate){
        fprintf(stderr, "Trying to leave a state while none has been saved\n");
        return;
    }
    tmp = pilestate;
    pilestate = pilestate->next;
    BEGIN(tmp->state);
    free(tmp);
    
}

void yy_new_while(unsigned int index){
    whilestack tmp;
    
    yy_change_start_condition(do_while);
    if(pilewhile && pilewhile->index == index)
        return;
    tmp = malloc(sizeof(whilecell));
    if(!tmp){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("malloc");
        return;
    }
    tmp->index = index;
    tmp->unclosed_if = 0;
    tmp->next = pilewhile;
    tmp->state = pilestate;
    pilewhile = tmp;
}

void yy_delete_while(unsigned int index){
    whilestack tmp;

    if(pilewhile && pilewhile->index == index){
        tmp = pilewhile;
        pilewhile = pilewhile->next;
        free(tmp);
    }
}

void yy_loop(player *joueur){
    statestack tmp;
    
    if(!pilewhile){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("loop");
        return;
    }
    while(pilestate && pilestate != pilewhile->state){
        tmp = pilestate;
        pilestate = pilestate->next;
        free(tmp);
    }
    yy_leave_start_condition();
    YY_FLUSH_BUFFER;
    
    letter_index = pilewhile->index;
    
    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(!yy_scan_buffer(joueur->code+pilewhile->index, joueur->sizecode-pilewhile->index)){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("yy_scan_buffer");
    }
}

void yy_rewind(){
    letter_index-=yyleng;
    yyless(0);
}

/* TODO add timer */