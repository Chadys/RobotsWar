%{
# include "header.h"

# define YYDEBUG 1

int yylex();
void yyerror(FILE*, hashtable, const char *);
char tab[MAX_TAB] = {'\t','\0'};
void update_tab(char);
extern int yylineno;
void yy_flush();

%}

%parse-param {FILE * current_p_file}
%param {hashtable htable}

%union {
  int i;
  char * c;
};

%error-verbose

%token <c> YNOM YTEST YCOND YLOOP
%token <i> YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YENDELSE YWHILE YENDWHILE YLIFE YSCORE YNRJ
%token RESERVED_KEYWORD UNRECOGNISED

%precedence ','
%left '+' '-'
%left '*' '/' '%'
%precedence UNARY

%%
program : instrlist
                { fprintf(current_p_file, "}"); yylineno = 1; }

instrlist : instr
    | instrlist instr
    
instr : YVAR YNOM
                { fprintf(current_p_file, "%sint %s;\n%sYCHECKTIMER(1);\n", tab, $2, tab); }
    | YNOM '='
                { fprintf(current_p_file, "%s%s = ", tab, $1); }
       value
                { fprintf(current_p_file, ";\n%sYCHECKTIMER(3);\n", tab); }
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
                { fprintf(current_p_file, "ligne(current_p->loc, "); }
      value ','
                { fprintf(current_p_file, ", "); }
      value
                { fprintf(current_p_file, ").dir"); }
    | '-' %prec UNARY
                { fprintf(current_p_file, " -"); }
      value
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
           condlist whileinstrlist YENDWHILE
                { update_tab(0); fprintf(current_p_file, "%s}\n", tab); }

whileinstrlist : whileinstr
    | whileinstrlist whileinstr
    
whileinstr : YLOOP
                { fprintf(current_p_file, "%s%s;\n", tab, $1); }
    | ifinwhileexpr
    | YVAR YNOM
                { fprintf(current_p_file, "%sint %s;\n%sYCHECKTIMER(1);\n", tab, $2, tab); }
    | YNOM '='
                { fprintf(current_p_file, "%s%s = ", tab, $1); }
       value
                { fprintf(current_p_file, ";\n%sYCHECKTIMER(1);\n", tab); }
    | whilexpr
    | action
    
ifexpr : YIF
                { fprintf(current_p_file, "%sif (", tab); update_tab(1); }
         condlist instrlist YENDIF
                { update_tab(0); fprintf(current_p_file, "%s}\n", tab); }
         .elsexpr
         
ifinwhileexpr : YIF
                { fprintf(current_p_file, "%sif (", tab); update_tab(1); }
         condlist whileinstrlist YENDIF
                { update_tab(0); fprintf(current_p_file, "%s}\n", tab); }
         .elsexpr
         
.elsexpr : %empty
    | YELSE
                { fprintf(current_p_file, "%selse {\nYCHECKTIMER(1);\n", tab); update_tab(1); }
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
      value ','
                { fprintf(current_p_file, ", "); }
      value
                { fprintf(current_p_file, ");\n"); }

condlist : conds
                { fprintf(current_p_file, "){\nYCHECKTIMER(1);\n"); }
                
conds : cond
    | conds YCOND
                { fprintf(current_p_file, " %s ", $2); }
        cond
                
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

void yyerror(FILE  __attribute__ ((unused))*current_p_file, hashtable __attribute__ ((unused))htable, const char * message){
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