%option noyywrap yylineno

    char *get_or_insert(hashtable, char *);
    int getvalue(char *);
    extern hashtable htable;
    
%%

"auto"|"case"|"char"|"const"|"default"|"do"|"double"|"enum"|"extern"|"float"|"for"|"goto"|"int"|"long"|"register"|"return"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"void"|"volatile"  { return RESERVED_KEYWORD; }

"<="|">="|"!="|"==" { yylval.c = get_or_insert(htable, yytext); return YTEST; }
"&&"|"||"       { yylval.c = get_or_insert(htable, yytext); return YCOND; }

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
"break"|"continue"  { yylval.c = get_or_insert(htable, yytext); return YLOOP; }

(?i:[a-z][a-z0-9_]*)        { yylval.c = get_or_insert(htable, yytext); return YNOM; }
[0-9]+          { yylval.i = atoi(yytext); return YNUM;    }
[-+*/%=()<>,]   { return yytext[0]; }

[ \t\f\n]       ;
[^ \t\f\n]      { fprintf(stderr, "yylex : Unrecognised symbol (%c)\n", yytext[0]); return UNRECOGNISED; }

%%
#include "header.h"

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