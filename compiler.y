%{
# include "header.h"

# define YYDEBUG 1

void yyerror(player*,hashtable, const char *);
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
%token <s> YTEST YCOND
%token <i> YNUM YDIR YSPRINT YBACK YWHILE
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YENDWHILE YLIFE YSCORE YNRJ YPASS
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
    | YNOM '=' value
                { $1->val = $3; }
    | whilexpr
    | ifexpr
    | action
    | YPASS
    
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
                
whilexpr : YWHILE condlist
                {   if (!$2){
                        yy_change_start_condition(4); //jump_while
                        yy_delete_while($1);
                        yy_rewind();
                        yychar = YPASS;
                    }
                    else{
                        yy_new_while($1); //do_while
                        yy_rewind();
                        yyclearin;
                    }
                }
            instrlist YENDWHILE
                {   if ($2){
                        yy_loop(joueur);
                        yy_rewind();
                        yyclearin;
                    }
                }
    
ifexpr : YIF condlist
                {   if (!$2){
                        yy_change_start_condition(1); //jump_if
                        yy_rewind();
                        yychar = YPASS;
                    }
                }
        instrlist YENDIF

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
                { $$ = $1; }
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

void yyerror(player __attribute__ ((unused))*joueur, hashtable __attribute__ ((unused))keywords, const char *message){
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", yylineno, message, yytext);
  yyfinish();
}

void yyfinish(){
  yylineno = 1;
  yy_flush();
  yy_clean();
}