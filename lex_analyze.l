%option noyywrap
    #include <stdlib.h>
    #include <string.h>

    typedef struct cell
    {
        char *val;
        struct cell *next;
    }cell;
    typedef cell* llist;
    
    typedef struct hashtable
    {
        int n;
        int p;
        llist *alveole;
    }hashtable;

    int isprime(int);
    unsigned long int sdbm(unsigned char *);
    char *get_or_insert(hashtable *, char *);
    hashtable init_hash(int);
    void free_hash(hashtable *h);
    int getvalue(char *);
    int lineno = 1;
%%

"<="|">="|"!="|"==" { yylval.c = get_simple_chaine(yytext[0]); return YTEST; }
"&&"|"||"       { yylval.c = get_simple_chaine(yytext[0]); return YCOND; }

"var"           { return YVAR; }
"Look"          { return YLOOK; }
"Shoot"         { return YSHOOT; }
"TurnAround"    { return YTURN; }
"Go"            { return YGO; }
"Snooze"        { return YSNOOZE; }

"LEFT"|"RIGHT"|"FORWARD"    { yylval.i = getvalue(yytext); return YDIR; }
"SPRINT"        { yylval.i = 5; return YSPRINT; }
"BACK"          { yylval.i = 3; return YBACK; }
"GAMELIMIT"|"NORESULT"|"NOTHING"|"WALL"|"TREASURE"|"BASE"|"ROBOT"|"VISIONMAX"   { yylval.i = getvalue(yytext); return YNUM; }
"Life"          { return YLIFE; }
"Score"         { return YSCORE; }
"Energy"        { return YNRJ; }

"if"            { return YIF; }
"endif"         { return YENDIF; }
"while"         { return YWHILE; }
"endwhile"      { return YENDWHILE; }
"else"          { return YELSE; }
"endelse"       { return YENDELSE; }
"break"|"continue"  { yylval.c = get_simple_chaine(yytext[0]); return YLOOP; }

(?i:[a-z][a-z0-9_]*)        { yylval.c = get_chaine(yytext, yyleng); return YNOM; }
[0-9]+          { yylval.i = atoi(yytext); return YNUM;    }
[-+*/%=()<>]      { return yytext[0]; }

[ \t\f]         ;
\n              lineno += 1;
.               { fprintf(stderr, "yylex : (%c)\n", yytext[0]); }

%%

hashtable h;

unsigned long int sdbm(unsigned char *str)
{
    unsigned long hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

int isprime(int n)
{
    if(n <= 1)
        return 0;
    else if(n <= 3)
        return 1;
    else if(n % 2 == 0 || n % 3 == 0)
        return 0;
    int i = 5;
    while(i*i <= n)
    {
        if(n % i == 0 || n % (i + 2) == 0)
            return 0;
        i += 6;
    }
    return 1;
}

void init_hash(int n)
{
    int i;
    hashtable ret;

    ret.n = n;
    ret.alveole = malloc(sizeof(llist) * n);
    for(i = 0; i < n; i++)
        ret.alveole[i] = NULL;
    i = n;
    while(!isprime(i)){
        i++;
    }
    ret.p = i;

    strcpy(chaines[0], "==");
    strcpy(chaines[1], "!=");
    strcpy(chaines[2], "<=");
    strcpy(chaines[3], ">=");
    strcpy(chaines[4], "&&");
    strcpy(chaines[5], "||");
    strcpy(chaines[6], "break");
    strcpy(chaines[7], "continue");

    get_or_insert(&ret, "==");
    get_or_insert(&ret, "!=");
    get_or_insert(&ret, "<=");
    get_or_insert(&ret, ">=");
    get_or_insert(&ret, "&&");
    get_or_insert(&ret, "||");
    get_or_insert(&ret, "break");
    get_or_insert(&ret, "continue");

    return ret;
}

char *get_or_insert(hashtable *h, char *val)
{
    int a = (murmur3(val, strlen(val)) % h->p) % h->n;
    llist ptr = h->alveole[a];
    if(!ptr)
    {
        ptr = h->alveole[a] = malloc(sizeof(llist));
        ptr->val = strdup(val);
        ptr->next = NULL;
        return ptr->val;
    }
    else
    {
        if(!strcmp(ptr->val, val))
            return ptr->val;
        while(ptr->next)
        {
            ptr = ptr->next;
            if(!strcmp(ptr->val, val))
                return ptr->val;
        }
        llist add = malloc(sizeof(llist));
        add->val = strdup(val);
        add->next = NULL;
        ptr->next = add;
        return add->val;
    }
}

void free_hash(hashtable *h)
{
    for(int i = 0; i < h.n; i++)
    {
        llist ptr = h.alveole[i];
        while(ptr)
        {
            printf(" %s", ptr->val);
            ptr = ptr->next;
        }

        llist ptr = h.alveole[i];
        while(ptr)
        {
            llist tmp = ptr;
            ptr = ptr->next;
            free(tmp);
        }
        h.alveole[i] = NULL;
    }
    free(h.alveole);
}

/* ggetvalue -- renvoie la valeur correspondante à un mot-clef */
int getvalue(char *keyword){

    if(!strcmp(keyword, "VISIONMAX"))
        return MAX_VISION;
    else if(!strcmp(keyword, "GAMELIMIT"))
        return -2;
    else if(!strcmp(keyword, "NORESULT"))
        return -1;
    else if(!strcmp(keyword, "NOTHING"))
        return 0;
    else if(!strcmp(keyword, "WALL") || !strcmp(keyword, "FORWARD"))
        return 1;
    else if(!strcmp(keyword, "TREASURE") || !strcmp(keyword, "RIGHT"))
        return 2;
    else if(!strcmp(keyword, "BASE"))
        return 3;
    else if(!strcmp(keyword, "ROBOT") || !strcmp(keyword, "LEFT"))
        return 4;
    return INT_MIN;
}