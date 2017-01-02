%{
# include "header.h"

# define YYDEBUG 1

void yyerror(player*,hashtable, const char *);
static char tab[MAX_TAB] = {'\t','\0'};
static void update_tab(char);
extern int yylineno;
int yylex(player*,hashtable);

%}

%param {player* joueur}
%param {hashtable keywords}

%union {
  int i;
  char * c;
};

%error-verbose

%token <c> YNOM YTEST YCOND YLOOP
%token <i> YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YENDELSE YWHILE YENDWHILE YLIFE YSCORE YNRJ
%token UNRECOGNISED

%left ','
%left '+' '-'
%left '*' '/' '%'
%precedence UNARY


%%
program : instrlist
                { fprintf(stdout, "}"); yylineno = 1; }

instrlist : instr
    | instrlist instr
    
instr : YVAR YNOM
                { fprintf(stdout, "%sint %s;\nYCHECKTIMER(1);\n", tab, $2); }
    | YNOM '='
                { fprintf(stdout, "%s%s = ", tab, $1); }
       value
                { fprintf(stdout, ";\nYCHECKTIMER(3);\n"); }
    | whilexpr
    | ifexpr
    | action
    
value : YNUM
                { fprintf(stdout, "%d", $1); }
    | YNOM
                { fprintf(stdout, "%s", $1); }
    | '('
                { fprintf(stdout, "( "); }
        value ')'
                { fprintf(stdout, " )"); }
    | YLOOK
                { fprintf(stdout, "ligne(current_p->loc, "); }
      value ','
                { fprintf(stdout, ", "); }
      value
                { fprintf(stdout, ").dir"); }
    | '-' %prec UNARY
                { fprintf(stdout, " -"); }
      value
    | value '+'
                { fprintf(stdout, " + "); }
      value
    | value '-'
                { fprintf(stdout, " - "); }
      value
    | value '*'
                { fprintf(stdout, " * "); }
      value
    | value '%'
                { fprintf(stdout, " %% "); }
      value
    | value '/'
                { fprintf(stdout, " / "); }
      value
    | YLIFE
                { fprintf(stdout, "current_p->life"); }
    | YSCORE
                { fprintf(stdout, "current_p->treasure"); }
    | YNRJ
                { fprintf(stdout, "current_p->energy"); }
                
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
                { fprintf(stdout, "%sint %s;\nYCHECKTIMER(1);\n", tab, $2); }
    | YNOM '='
                { fprintf(stdout, "%s%s = ", tab, $1); }
       value
                { fprintf(stdout, ";\nYCHECKTIMER(1);\n"); }
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
                { fprintf(stdout, "%sreturn create_action(SNOOZE, 0, 0);\n", tab); }
    | YTURN YDIR
                { fprintf(stdout, "%sreturn create_action(TURNAROUND, %d, 0);\n", tab, $2); }
    | YTURN YBACK
                { fprintf(stdout, "%sreturn create_action(TURNAROUND, %d, 0);\n", tab, $2); }
    | YGO YDIR
                { fprintf(stdout, "%sreturn create_action(GO, %d, 0);\n", tab, $2); }
    | YGO YSPRINT
                { fprintf(stdout, "%sreturn create_action(GO, %d, 0);\n", tab, $2); }
    | YSHOOT
                { fprintf(stdout, "%sreturn create_action(SHOOT, ", tab); }
      value ','
                { fprintf(stdout, ", "); }
      value
                { fprintf(stdout, ");\n"); }

condlist : conds
                { fprintf(stdout, "){\nYCHECKTIMER(1);\n"); }
                
conds : cond
    | conds YCOND
                { fprintf(stdout, " %s ", $2); }
        cond
                
cond : value YTEST
                { fprintf(stdout, " %s ", $2); }
        value
    | value '<'
                { fprintf(stdout, " < "); }
        value
    | value '>'
                { fprintf(stdout, " > "); }
        value
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