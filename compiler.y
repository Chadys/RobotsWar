/* ppcm.y
 parseur de ppcm : le code est genere au fur et a mesure du parsing
 */
%{
# include "header.h"
# include "lex.yy.c"

# define YYDEBUG 1
%}

%union {
  int i;            /* constantes, etiquettes et nbre d'arg. */
  char * c;            /* variables et fonctions */=
};

%token <c> YNOM YTEST YCOND YLOOP
%token <i> YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YWHILE YENDWHILE YELSE YENDELSE YLIFE YSCORE YNRJ

%type <i> calcul

%right '='
%nonassoc YTEST
%left YCOND
%left '+' '-'
%left '*' '/' '%'
%left UNARY

%%

%%
main(){
  yyparse();
  return 0;
}

yyerror(char * message){
  extern int lineno;
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", lineno, message, yytext);
}