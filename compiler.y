%{
# include "header.h"

# define YYDEBUG 1

void yyerror(player*,hashtable, const char *);
static char tab[MAX_TAB] = {'\t','\0'};
static void update_tab(char);
extern int yylineno;
int yylex(player*,hashtable);
void yyfinish();

%}

%param {player* joueur}
%param {hashtable keywords}

%union {
  int i;
  llist c;
  char *s;
};

%error-verbose

%token <c> YNOM
%token <s> YTEST YCOND YLOOP
%token <i> YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YENDELSE YWHILE YENDWHILE YLIFE YSCORE YNRJ
%token UNRECOGNISED

%precedence ','
%left '+' '-'
%left '*' '/' '%'
%precedence UNARY

%type <i> value condlist conds cond

%%
program : instrlist
                { yylineno = 1; }

instrlist : instr
    | instrlist instr
    
instr : YVAR YNOM
    | YNOM '=' value
                { $1->val = $3; }
    | whilexpr
    | ifexpr
    | action
    
value : YNUM
                { $$ = $1; }
    | YNOM
                { $$ = $1->val; }
    | '(' value ')'
                { $$ = $2; }
    | YLOOK value ',' value
                { $$ = ligne(joueur->loc, $2, $4).dir; }
    | '-' value %prec UNARY
                { $$ = -$2; }
    | value '+'value
                { $$ = $1+$3; }
    | value '-' value
                { $$ = $1-$3; }
    | value '*' value
                { $$ = $1*$3; }
    | value '%' value
                { $$ = $1%$3; }
    | value '/' value
                { $$ = $1/$3; }
    | YLIFE
                { $$ = joueur->life; }
    | YSCORE
                { $$ = joueur->treasure; }
    | YNRJ
                { $$ = joueur->energy; }
                
whilexpr : YWHILE
                { fprintf(stdout, "%swhile (", tab); update_tab(1); }
           condlist whileinstrlist YENDWHILE
                { update_tab(0); fprintf(stdout, "%s}\n", tab); }

whileinstrlist : whileinstr
    | whileinstrlist whileinstr
    
whileinstr : YLOOP
                { fprintf(stdout, "%s%s;\n", tab, $1); }
    | ifinwhileexpr
    | YVAR YNOM
    | YNOM '=' value
                { $1->val = $3; }
    | whilexpr
    | action
    
ifexpr : YIF
                { fprintf(stdout, "%sif (", tab); update_tab(1); }
         condlist instrlist YENDIF
                { update_tab(0); fprintf(stdout, "%s}\n", tab); }
         .elsexpr
         
ifinwhileexpr : YIF
                { fprintf(stdout, "%sif (", tab); update_tab(1); }
         condlist whileinstrlist YENDIF
                { update_tab(0); fprintf(stdout, "%s}\n", tab); }
         .elsexpr
         
.elsexpr : %empty
    | YELSE
                { fprintf(stdout, "%selse {\nYCHECKTIMER(1);\n", tab); update_tab(1); }
      instrlist YENDELSE
                { update_tab(0); fprintf(stdout, "%s}\n", tab); }

action : YSNOOZE
                { create_action(SNOOZE, 0, 0, joueur); yyfinish(); YYACCEPT; }
    | YTURN YDIR
                { create_action(TURNAROUND, $2, 0, joueur); yyfinish(); YYACCEPT; }
    | YTURN YBACK
                { create_action(TURNAROUND, $2, 0, joueur); yyfinish(); YYACCEPT; }
    | YGO YDIR
                { create_action(GO, $2, 0, joueur); yyfinish(); YYACCEPT; }
    | YGO YSPRINT
                { create_action(GO, $2, 0, joueur); yyfinish(); YYACCEPT; }
    | YSHOOT value ',' value
                { create_action(SHOOT, $2, $4, joueur); yyfinish(); YYACCEPT; }

condlist : conds
                { $$ = $1; }
                
conds : cond
                { $$ = $1; }
    | conds YCOND cond
                { switch($2[0]){
                    case('&'):
                        $$ = $1 && $3;
                        break;
                    default:
                        $$ = $1 || $3;
                }}
                
cond : value YTEST value
                { switch($2[0]){
                    case('<'):
                        $$ = $1 <= $3;
                        break;
                    case('>'):
                        $$ = $1 >= $3;
                        break;
                    case('!'):
                        $$ = $1 != $3;
                        break;
                    default:
                        $$ = $1 == $3;
                }}
    | value '<' value
                { $$ = $1 < $3; }
    | value '>' value
                { $$ = $1 > $3; }
%%

void yyerror(player __attribute__ ((unused))*joueur, hashtable __attribute__ ((unused))keywords, const char * message){
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", yylineno, message, yytext);
  yylineno = 1;
  while(tab[1] != '\0')
    update_tab(0);
  yy_flush();
}

void yyfinish(){
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