%{
# include "header.h"

# define YYDEBUG 1
int yylex();
void yyerror(const char *);
%}

%union {
  int i;
  char * c;
};

%error-verbose

%token <c> YNOM YTEST YCOND YLOOP
%token <i> YNUM YDIR YSPRINT YBACK
%token YVAR YLOOK YSHOOT YTURN YGO YSNOOZE YIF YENDIF YELSE YENDELSE YWHILE YENDWHILE YLIFE YSCORE YNRJ

%right '='
%nonassoc YTEST
%left YCOND
%left '+' '-'
%left '*' '/' '%'
%left UNARY

%%
program : instrlist

instrlist : instr
                { fprintf(current_p_file, "}\n\n"); }
    | instr instrlist
    
instr : YVAR YNOM
                { fprintf(current_p_file, "\tint %s;\n", $2); }
    | YNOM '='
                { fprintf(current_p_file, "\t%s = ", $1); }
       value
                { fprintf(current_p_file, "\n;"); }
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
                { fprintf(current_p_file, "- "); }
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
                { fprintf(current_p_file, "\twhile ("); }
           cond whileinstr YENDWHILE
                { fprintf(current_p_file, "\t}\n"); }

whileinstr : instr
    | YLOOP
                { fprintf(current_p_file, "\t\t%s;\n", $1); }
    | instr whileinstr
    | YLOOP
                { fprintf(current_p_file, "\t\t%s;\n", $1); }
      whileinstr
    
ifexpr : YIF
                { fprintf(current_p_file, "\tif ("); }
         cond instrlist YENDIF
                { fprintf(current_p_file, "\t}\n"); }
         elsexpr
         
elsexpr : /* rien */
    | YELSE
                { fprintf(current_p_file, "\telse {"); }
      instrlist YENDELSE
                { fprintf(current_p_file, "\t}\n"); }

action : YSNOOZE
                { fprintf(current_p_file, "\treturn create_action(0, 0, 0);\n"); }
    | YTURN YDIR
                { fprintf(current_p_file, "\treturn create_action(1, %d, 0);\n", $2); }
    | YTURN YBACK
                { fprintf(current_p_file, "\treturn create_action(1, %d, 0);\n", $2); }
    | YGO YDIR
                { fprintf(current_p_file, "\treturn create_action(2, %d, 0);\n", $2); }
    | YGO YSPRINT
                { fprintf(current_p_file, "\treturn create_action(2, %d, 0);\n", $2); }
    | YSHOOT
                { fprintf(current_p_file, "\treturn create_action(3, "); }
      value
                { fprintf(current_p_file, ", "); }
      value
                { fprintf(current_p_file, ");\n"); }

cond : value YTEST
                { fprintf(current_p_file, " %s ", $2); }
        value
                { fprintf(current_p_file, "){\n"); }
%%

# include "lex.yy.c"

void yyerror(const char * message){
  extern int lineno;
  extern char * yytext;

  fprintf(stderr, "%d: %s at %s\n", lineno, message, yytext);
}