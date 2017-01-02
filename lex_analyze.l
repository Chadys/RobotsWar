%{
#include "header.h"
#include "compiler.tab.h"
%}
%option noyywrap yylineno

    int getvalue(char *);
    char *get_or_insert(hashtable, char *);
    #define YY_DECL int yylex(player *joueur, hashtable keywords)
    
%%

"<="|">="|"!="|"==" { yylval.c = get_or_insert(keywords, yytext); return YTEST; }
"&&"|"||"       { yylval.c = get_or_insert(keywords, yytext); return YCOND; }

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
"break"|"continue"  { yylval.c = get_or_insert(keywords, yytext); return YLOOP; }

(?i:[a-z][a-z0-9_]*)        { yylval.c = get_or_insert(joueur->variables, yytext); return YNOM; }
[0-9]+          { yylval.i = atoi(yytext); return YNUM;    }
[-+*/%=()<>,]   { return yytext[0]; }

[ \t\f\n]       ;
[^ \t\f\n]      { fprintf(stderr, "yylex : Unrecognised symbol (%c)\n", yytext[0]); return UNRECOGNISED; }

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
    yy_delete_buffer( YY_CURRENT_BUFFER );
    yy_scan_buffer(code, size);
}

void yy_flush(){
    YY_FLUSH_BUFFER;
}