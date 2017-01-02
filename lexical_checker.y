%{
# include "header.h"

# define YYDEBUG 1
#define checklex yylex
#define checklval yylval

void yyerror(player*,hashtable, const char *);
static char tab[MAX_TAB] = {'\t','\0'};
static void update_tab(char);
extern int yylineno;
int yylex(player*,hashtable);

%}

%define api.prefix {check}

%param {player* joueur}
%param {hashtable keywords}

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
                { yylineno = 1; }

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

condlist : conds
                
conds : cond
    | conds YCOND cond
                
cond : value YTEST value
    | value '<' value
    | value '>' value
%%

void yyerror(player __attribute__ ((unused))*joueur, hashtable __attribute__ ((unused))keywords, const char * message){
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", yylineno, message, yytext);
  yylineno = 1;
  while(tab[1] != '\0')
    update_tab(0);
  yy_flush();
}

void update_tab(char add){
    int i;
    
    for(i=0; i<MAX_TAB; i++){
        if(tab[i]=='\0'){
            if (!add){
                tab[i-1] = '\0';
            }
            else if(i<MAX_TAB-1){
                tab[i]='\t';
                tab[i+1] = '\0';
            }
            return;
        }
    }
}