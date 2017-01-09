%{
# include "header.h"

# define YYDEBUG 1

void yyerror(player*,hashtable, const char *);
extern int yylineno;
int yylex(player*,hashtable);
void yyfinish();
static unsigned int timer = 0;
extern char delay_nop;
    
# define YCHECKTIMER(N) timer+=N;\
                        if(!update_energy(&timer, joueur)){\
                            yyfinish();\
                            YYACCEPT;\
                        }//in players' file, check the timer and exit the function if too much time was taken

%}

%param {player* joueur}
%param {hashtable keywords}

%union {
  int i;
  llist c;
  char *s;
  struct{
    unsigned int index;
    int line;
  } w;
};

%error-verbose

%token <c> YNOM
%token <s> YTEST YCOND
%token <i> YNUM YDIR YSPRINT YBACK YENDWHILE
%token <w> YWHILE
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YLIFE YSCORE YNRJ YNOP
%token UNRECOGNISED

%precedence ','
%left '+' '-'
%left '*' '/' '%'
%precedence UNARY

%type <i> value condlist cond

%%
program : instrlist
                { yyfinish(); }

instrlist : .instr
    | instrlist .instr
    
.instr : YVAR YNOM
                { YCHECKTIMER(1); }
    | YNOM '=' value
                { $1->val = $3; YCHECKTIMER(1); }
    | whilexpr
    | ifexpr
    | action
    | nope
    
nope : YNOP
    | nope YNOP
    
value : YNUM
                { $$ = $1; }
    | YNOM
                { $$ = $1->val; }
    | '(' value ')'
                { $$ = $2; }
    | YLOOK value ',' value
                { $$ = ligne(joueur->loc, $2, $4).dir; YCHECKTIMER(3); }
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
                
whilexpr : YWHILE condlist
                {   yy_rewind();
                    yyclearin;
                    delay_nop=1;
                    if (!$2){
                        yy_change_start_condition(3); //jump_while
                    }
                    else
                        yy_new_while();
                }
            instrlist YENDWHILE
                {   delay_nop=1;
                    if($2)
                        yy_clean(1); //delete one while;
                    if (!$5){ //mid rule actions are taken into account in $n count
                        yyclearin;
                        yy_loop(joueur, $1.index, $1.line); //loop
                    }
                    else
                        yy_change_start_condition(0); //initial
                }
    
ifexpr : YIF condlist
                {   if (!$2){
                        yy_rewind();
                        yyclearin;
                        yy_change_start_condition(1); //jump_if
                    }
                }
        instrlist YENDIF
                { if(!$2) yy_change_start_condition(0); } //initial

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

                
condlist : cond
                { $$ = $1; YCHECKTIMER(1); }
    | condlist YCOND cond
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

void yyerror(player *joueur, hashtable __attribute__ ((unused))keywords, const char *message){
  extern char * yytext;

  fprintf(stderr, "%s : %d: %s at %s\n", joueur->name, yylineno, message, yytext);
  yyfinish();
}

void yyfinish(){
  yy_clean(-1); // free the whole pilewhile
  timer = 0;
}