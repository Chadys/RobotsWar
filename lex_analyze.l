%{
#include "header.h"
#include "compiler.tab.h"
%}
%option noyywrap yylineno nounput noinput
%x jump_if jump_else jump_while

    int getvalue(char *);
    void yy_change_start_condition(int);
    enum yytokentype yy_close_if();
    #define YY_DECL int yylex(player *joueur, hashtable keywords)
    static unsigned int letter_index;
    #define YY_USER_ACTION letter_index+=yyleng;
    static unsigned int nif;
    static unsigned int nelse;
    static unsigned int nwhile;
    static whilestack pilewhile = NULL;
    char delay_nop = 1; //used so the lookahead token after a while cond can't call break or continue
    
%%

<*>[ \t\f\n]+       ;

<jump_if>{
    "if"            { nif++; }
    "endif"[ \t\f\n]*("else")?  {    if(!nif){
                                        if(pilewhile)
                                            pilewhile->unclosed_if--;
                                        return YENDIF;
                                    }
                                    nif--;
                                }
    [^ifend]+|.     { return YNOP; }
}

<jump_else>{
    "else"          { nelse++; }
    "endelse"       {   if(!nelse)
                            BEGIN(INITIAL);
                        else
                            nelse--;
                    }
    [^elsnd]+|.     ;
}



<jump_while>{
    "while"         { nwhile++; }
    "endwhile"      {   if(!nwhile){
                            yylval.i = 1;
                            return YENDWHILE;
                        }
                        nwhile--;
                    }
    [^whilend]+|.   { return YNOP; }
}


"<="|">="|"!="|"==" { yylval.s = get_or_insert(keywords, yytext)->name; return YTEST; }
"&&"|"||"           { yylval.s = get_or_insert(keywords, yytext)->name; return YCOND; }

"var"               { return YVAR; }
"Look"              { return YLOOK; }
"Shoot"             { return YSHOOT; }
"TurnAround"        { return YTURN; }
"Go"                { return YGO; }
"Snooze"            { return YSNOOZE; }

"LEFT"|"RIGHT"|"FORWARD"    { yylval.i = getvalue(yytext); return YDIR; }
"SPRINT"            { yylval.i = 5; return YSPRINT; }
"BACK"              { yylval.i = 3; return YBACK; }
"GAMELIMIT"|"NORESULT"|"NOTHING"|"WALL"|"TREASURE"|"BASE"|"ROBOT"|"VISIONMAX"   { yylval.i = getvalue(yytext); return YNUM; }
"Life"              { return YLIFE; }
"Score"             { return YSCORE; }
"Energy"            { return YNRJ; }

"if"                { if(pilewhile) pilewhile->unclosed_if++; return YIF; }
"endif"             { if(pilewhile) pilewhile->unclosed_if--; return YENDIF; }
"else"              { BEGIN(jump_else); }
"endelse"           ;
"while"             { yylval.w.index = letter_index-yyleng; yylval.w.line = yylineno; return YWHILE; }
"endwhile"          { yylval.i = 0; return YENDWHILE; }
"break"             {   if(delay_nop){
                            delay_nop--;
                            yy_rewind();
                            return YNOP;
                        }
                        if(!pilewhile->unclosed_if){
                            BEGIN(jump_while);
                            return YNOP;
                        }
                        return yy_close_if();
                    }
"continue"          {   if(delay_nop){
                            delay_nop--;
                            yy_rewind();
                            return YNOP;
                        }
                        if(!pilewhile->unclosed_if){
                            yylval.i = 0;
                            return YENDWHILE;
                        }
                        return yy_close_if();
                    }

[[:alpha:]][[:alnum:]_]*        { yylval.c = get_or_insert(joueur->variables, yytext); return YNOM; }
[[:digit:]]+        { yylval.i = atoi(yytext); return YNUM;    }
[-+*/%=()<>,]       { return yytext[0]; }
.                   { fprintf(stderr, "yylex : Unrecognised symbol (%c)\n", yytext[0]); return UNRECOGNISED; }

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
    yylineno = 1;
    delay_nop = 1;
    BEGIN(INITIAL);
    
    yy_end();
    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(!yy_scan_buffer(code, size)){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("yy_scan_buffer");
    }
}

void yy_change_start_condition(int cond){
    BEGIN(cond);
}

void yy_loop(player *joueur, unsigned int index, int line){
    
    letter_index = index;
    yylineno = line;
    
    yy_end();
    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(!yy_scan_buffer(joueur->code+letter_index, joueur->sizecode-letter_index)){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("yy_scan_buffer");
    }
}

 // flex with yy_scan_buffer modify joueur->code to put an '\0' to end the current string yytext,
 // this restore the original character
void yy_end(){
    if(yy_c_buf_p)
        *(yy_c_buf_p) = (yy_hold_char);
}

void yy_rewind(){
    if(pilewhile && !strcmp(yytext, "if"))
        pilewhile->unclosed_if--;
    letter_index-=yyleng;
    yyless(0);
}

void yy_clean(int n){
    whilestack tmp;
    
    while(pilewhile && n){
        tmp=pilewhile;
        pilewhile = pilewhile->next;
        free(tmp);
        n--;
    }
}

void yy_new_while(){
    whilestack tmp;
    
    tmp = malloc(sizeof(whilecell));
    if(!tmp){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("malloc");
        return;
    }
    tmp->unclosed_if = 0;
    tmp->next = pilewhile;
    pilewhile = tmp;
}

enum yytokentype yy_close_if(){
    static char add_nop = 1;
    
    yy_rewind();
    if(add_nop){
        add_nop--;
        return YNOP;
    }
    add_nop++;
    pilewhile->unclosed_if--;
    return YENDIF; 
}