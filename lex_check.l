%{
#include "header.h"
#include "lexical_checker.tab.h"
%}
%option noyywrap yylineno nounput noinput
%option prefix="check"
    
%%

"<="|">="|"!="|"==" { return YTEST; }
"&&"|"||"       { return YCOND; }

"var"           { return YVAR; }
"Look"          { return YLOOK; }
"Shoot"         { return YSHOOT; }
"TurnAround"    { return YTURN; }
"Go"            { return YGO; }
"Snooze"        { return YSNOOZE; }

"LEFT"|"RIGHT"|"FORWARD"    { return YDIR; }
"SPRINT"        { return YSPRINT; }
"BACK"          { return YBACK; }
"GAMELIMIT"|"NORESULT"|"NOTHING"|"WALL"|"TREASURE"|"BASE"|"ROBOT"|"VISIONMAX"   { return YNUM; }
"Life"          { return YLIFE; }
"Score"         { return YSCORE; }
"Energy"        { return YNRJ; }

"if"            { return YIF; }
"endif"         { return YENDIF; }
"while"         { return YWHILE; }
"endwhile"      { return YENDWHILE; }
"else"          { return YELSE; }
"endelse"       { return YENDELSE; }
"break"|"continue"  { return YLOOP; }

(?i:[a-z][a-z0-9_]*)        { return YNOM; }
[0-9]+          { return YNUM;    }
[-+*/%=()<>,]   { return yytext[0]; }

[ \t\f\n]+      ;
.               { fprintf(stderr, "yylex : Unrecognised symbol (%c)\n", yytext[0]); return UNRECOGNISED; }

%%

void check_init_parser(char *code, size_t size){

    yy_delete_buffer( YY_CURRENT_BUFFER );
    if(!yy_scan_buffer(code, size)){
        fprintf(stderr, "Error in file lex.yy.c, line %d\n", __LINE__);
        perror("yy_scan_buffer");
    }
}

void check_flush(){
    YY_FLUSH_BUFFER;
}