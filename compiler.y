%{
# include "header.h"

# define YYDEBUG 1
int yylex();
void yyerror(const char *);
char tab[MAX_TAB] = {'\t','\0'};
void update_tab(char);
%}

%union {
  int i;
  char * c;
};

%error-verbose

%token <c> YNOM YTEST YCOND YLOOP
%token <i> YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YENDELSE YWHILE YENDWHILE YLIFE YSCORE YNRJ

%left '+' '-'
%left '*' '/' '%'
%precedence UNARY

%%
program : instrlist
                { fprintf(current_p_file, "}"); }

instrlist : instr
    | instrlist instr
    
instr : YVAR YNOM
                { fprintf(current_p_file, "%sint %s;\n", tab, $2); }
    | YNOM '='
                { fprintf(current_p_file, "%s%s = ", tab, $1); }
       value
                { fprintf(current_p_file, ";\n"); }
    | whilexpr
    | ifexpr
    | action
    
value : YNUM
                { fprintf(current_p_file, "%d", $1); }
    | YNOM
                { fprintf(current_p_file, "%s", $1); }
    | '('
                { fprintf(current_p_file, "( "); }
        value ')'
                { fprintf(current_p_file, " )"); }
    | YLOOK
                { fprintf(current_p_file, "look("); }
      value
                { fprintf(current_p_file, ", "); }
      value
                { fprintf(current_p_file, ")"); }
    | '-'
                { fprintf(current_p_file, "-"); }
        value %prec UNARY
    | value '+'
                { fprintf(current_p_file, " + "); }
      value
    | value '-'
                { fprintf(current_p_file, " - "); }
      value
    | value '*'
                { fprintf(current_p_file, " * "); }
      value
    | value '%'
                { fprintf(current_p_file, " %% "); }
      value
    | value '/'
                { fprintf(current_p_file, " / "); }
      value
    | YLIFE
                { fprintf(current_p_file, "current_p->life"); }
    | YSCORE
                { fprintf(current_p_file, "current_p->treasure"); }
    | YNRJ
                { fprintf(current_p_file, "current_p->energy"); }
                
whilexpr : YWHILE
                { fprintf(current_p_file, "%swhile (", tab); update_tab(1); }
           condlist whileinstr YENDWHILE
                { update_tab(0); fprintf(current_p_file, "%s}\n", tab); }

whileinstr : instr
    | YLOOP
                { fprintf(current_p_file, "%s%s;\n", tab, $1); }
    | instr whileinstr
    | YLOOP
                { fprintf(current_p_file, "%s%s;\n", tab, $1); }
      whileinstr
    
ifexpr : YIF
                { fprintf(current_p_file, "%sif (", tab); update_tab(1); }
         condlist instrlist YENDIF
                { update_tab(0); fprintf(current_p_file, "%s}\n", tab); }
         .elsexpr
         
.elsexpr : %empty
    | YELSE
                { fprintf(current_p_file, "%selse {", tab); update_tab(1); }
      instrlist YENDELSE
                { update_tab(0); fprintf(current_p_file, "%s}\n", tab); }

action : YSNOOZE
                { fprintf(current_p_file, "%sreturn create_action(SNOOZE, 0, 0);\n", tab); }
    | YTURN YDIR
                { fprintf(current_p_file, "%sreturn create_action(TURNAROUND, %d, 0);\n", tab, $2); }
    | YTURN YBACK
                { fprintf(current_p_file, "%sreturn create_action(TURNAROUND, %d, 0);\n", tab, $2); }
    | YGO YDIR
                { fprintf(current_p_file, "%sreturn create_action(GO, %d, 0);\n", tab, $2); }
    | YGO YSPRINT
                { fprintf(current_p_file, "%sreturn create_action(GO, %d, 0);\n", tab, $2); }
    | YSHOOT
                { fprintf(current_p_file, "%sreturn create_action(SHOOT, ", tab); }
      value
                { fprintf(current_p_file, ", "); }
      value
                { fprintf(current_p_file, ");\n"); }

condlist : conds
                { fprintf(current_p_file, "){\n"); }
                
conds : cond
    | conds YTEST cond
                
cond : value YTEST
                { fprintf(current_p_file, " %s ", $2); }
        value
    | value '<'
                { fprintf(current_p_file, " < "); }
        value
    | value '>'
                { fprintf(current_p_file, " > "); }
        value
%%

# include "lex.yy.c"

void yyerror(const char * message){
  extern int lineno;
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", lineno, message, yytext);
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