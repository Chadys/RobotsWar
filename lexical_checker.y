%{
# include "header.h"

# define YYDEBUG 1

void yyerror(const char *);
extern int checklineno;
int checklex();

%}

%define api.prefix {check}

%union {
  int i;
  char * c;
};

%error-verbose

%token YNOM YTEST YCOND YLOOP
%token YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YENDELSE YWHILE YENDWHILE YLIFE YSCORE YNRJ
%token UNRECOGNISED

%precedence ','
%left '+' '-'
%left '*' '/' '%'
%precedence UNARY

%%
program : instrlist
                { checklineno = 1; }

instrlist : instr
    | instrlist instr
    
instr : YVAR YNOM
    | YNOM '=' value
    | whilexpr
    | ifexpr
    | action
    
value : YNUM
    | YNOM
    | '(' value ')'
    | YLOOK value ',' value
    | '-' %prec UNARY value
    | value '+' value
    | value '-' value
    | value '*' value
    | value '%' value
    | value '/' value
    | YLIFE
    | YSCORE
    | YNRJ
                
whilexpr : YWHILE
           condlist whileinstrlist YENDWHILE

whileinstrlist : whileinstr
    | whileinstrlist whileinstr
    
whileinstr : YLOOP
    | ifinwhileexpr
    | YVAR YNOM
    | YNOM '=' value
    | whilexpr
    | action
    
ifexpr : YIF condlist instrlist YENDIF .elsexpr
         
ifinwhileexpr : YIF condlist whileinstrlist YENDIF .elsexpr
         
.elsexpr : %empty
    | YELSE instrlist YENDELSE

action : YSNOOZE
    | YTURN YDIR
    | YTURN YBACK
    | YGO YDIR
    | YGO YSPRINT
    | YSHOOT value ',' value
                
condlist : cond
    | condlist YCOND cond
                
cond : value YTEST value
    | value '<' value
    | value '>' value
%%

void yyerror(const char * message){
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", checklineno, message, yytext);
  checklineno = 1;
  check_flush();
}