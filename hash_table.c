#include "header.h"

hashtable htable;

unsigned long int sdbm(char *str)
{
    unsigned long hash = 0;
    char * c;

    for (c = str; *c; c++)
        hash = *c + (hash << 6) + (hash << 16) - hash;

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

hashtable init_hash(hashtable h)
{
    int i;

    h.alveole = malloc(sizeof(llist) * h.n);
    for(i = 0; i < h.n; i++)
        h.alveole[i] = NULL;
    while(!isprime(i)){
        i++;
    }
    h.p = i;
    return h;
}

char *get_or_insert(hashtable h, char *val)
{
    int a = (sdbm(val) % h.p) % h.n;
    llist ptr = h.alveole[a];
    if(!ptr)
    {
        ptr = h.alveole[a] = malloc(sizeof(llist));
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

void free_hash(hashtable h)
{
    for(int i = 0; i < h.n; i++)
    {
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