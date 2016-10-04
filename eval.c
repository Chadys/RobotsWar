#include "header.h"

//replace a string naming a variable to its corresponding int value
int remplacer(player *P, char *varname)
{
    //printf("Remplacer : %s\n", varname);
    if(!strcmp(varname, "VISIONMAX"))
        return MAX_VISION;
    else if(!strcmp(varname, "Life"))
        return P->life;
    else if(!strcmp(varname, "Score"))
        return P->treasure;
    else if(!strcmp(varname, "Energy"))
        return P->energy;
    else if(!strcmp(varname, "GAMELIMIT"))
        return -2;
    else if(!strcmp(varname, "NORESULT"))
        return -1;
    else if(!strcmp(varname, "NOTHING"))
        return 0;
    else if(!strcmp(varname, "WALL") || !strcmp(varname, "FORWARD"))
        return 1;
    else if(!strcmp(varname, "TREASURE") || !strcmp(varname, "RIGHT"))
        return 2;
    else if(!strcmp(varname, "BASE") || !strcmp(varname, "BACK"))
        return 3;
    else if(!strcmp(varname, "ROBOT") || !strcmp(varname, "LEFT"))
        return 4;
    else if(!strcmp(varname, "SPRINT"))
        return 5;

    listvar tmp = P->varlist;
    while(tmp)
    {
        if(!strcmp(tmp->name, varname))
            return tmp->val;
        tmp = tmp->next;
    }
    return INT_MIN;
}

//do the math linked to an operator
int applyOp(char op, int b, int a)
{
    switch (op)
    {
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '*':
        return a * b;
    case '/':
        if(b==0){
            raise(SIGFPE);
            return 0;
        }
        return a / b;
    }
    return 0;
}

//test wether the first operator has precedence over the second one
int hasPrecedence(char op1, char op2)
{
    if (((op2 == '(' || op2 == ')')) || ((op1 == '*' || op1 == '/') && (op2 == '+' || op2 == '-')))
        return 0;

    return 1;
}

//add to a stack
listint listint_add(listint linkedlist, int val)
{
    listint to_add = malloc(sizeof(cellint));
    to_add->data = val;
    to_add->next = linkedlist;
    return to_add;
}

//remove from a stack
listint listint_pop(listint linkedlist)
{
    listint temp;

    if(!linkedlist)
        return NULL;

    temp=linkedlist;
    linkedlist = linkedlist->next;
    free(temp);
    return linkedlist;
}

//gives the value of an expression 
int eval(player *P, int i)
{
    if(!strcmp(P->code[i], "Look"))
    {
        return ligne(P->loc, eval_math(P, P->code[i + 1]), eval_math(P, P->code[i + 2])).dir;
    }
    else
        return eval_math(P, P->code[i]);
}

//gives the value of an arithmetic expression
int eval_math(player *P, char *str)
{
    //printf("Eval : %s\n", str);
    int last = -1;

    listint values = NULL;
    listint ops = NULL;

    unsigned int i;
    for(i = 0; i < strlen(str); i++)
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            int bufflen = 0;
            int buffcap = 3;
            char *buffnb = malloc(sizeof(char) * buffcap + 1);

            while(i < strlen(str) && (str[i] >= '0' && str[i] <= '9'))
            {
                if(bufflen == buffcap)
                {
                    buffcap *= 2;
                    buffnb = realloc(buffnb, sizeof(char) * buffcap + 1);
                }
                buffnb[bufflen] = str[i];
                i++;
                bufflen++;
            }
            buffnb[bufflen] = '\0';
            i--;
            
            values = listint_add(values, strtol(buffnb, NULL, 10));
            last = 1;
            free(buffnb);
        }
        else if((str[i] >= 'a' && str[i]<= 'z') || (str[i] >= 'A' && str[i]<= 'Z'))
        {
            int bufflen = 0;
            int buffcap = 5;
            char *buffvar = malloc(sizeof(char) * buffcap + 1);

            while((i < strlen(str)) && ((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i]<= 'z') || (str[i] >= 'A' && str[i]<= 'Z') || (str[i] == '_' )))
            {
                if(bufflen == buffcap)
                {
                    buffcap *= 2;
                    buffvar = realloc(buffvar, sizeof(char) * buffcap + 1);
                }
                buffvar[bufflen] = str[i];
                i++;
                bufflen++;
            }
            buffvar[bufflen] = '\0';
            i--;

            // we got the buffer with the variable
            values = listint_add(values, remplacer(P, buffvar));
            last = 1;
            free(buffvar);
        }

        else if (str[i] == '(')
        {
            ops = listint_add(ops, '(');
            last = 0;
        }

        else if(str[i] == ')')
        {
            while(ops->data != '(')
            {
                int op = ops->data;
                ops = listint_pop(ops);
                int val1 = values->data;
                values = listint_pop(values);
                int val2 = values->data;
                values = listint_pop(values);


                values = listint_add(values, applyOp(op, val1, val2));
                last = 1;
            }
            ops = listint_pop(ops);
            last = 0;
        }

        else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/')
        {
            if((last != 1 && i + 1 < strlen(str) && str[i] == '-') && (isdigit(str[i + 1]) || isalpha(str[i + 1]) || str[i + 1] == '('))
            {
                ops = listint_add(ops, '*');
                values = listint_add(values, -1);
            }
            else
            {
                while(ops && hasPrecedence(str[i], ops->data))
                {
                    int op = ops->data;
                    ops = listint_pop(ops);
                    int val1 = values->data;
                    values = listint_pop(values);
                    int val2 = values->data;
                    values = listint_pop(values);
    
                    values = listint_add(values, applyOp(op, val1, val2));
                }
                ops = listint_add(ops, str[i]);
                last = 0;
            }
        }
    }

    /*
    printf("Operations : ");
    listint_print(ops);
    printf("Valeurs : ");
    listint_print(values);
    */
    
    while(ops)
    {
        int op = ops->data;
        ops = listint_pop(ops);
        int val1 = values->data;
        values = listint_pop(values);
        int val2 = values->data;
        values = listint_pop(values);

        values = listint_add(values, applyOp(op, val1, val2));
        last = 1;
    }
    int ret = values->data;
    values = listint_pop(values);
    return ret;
}
