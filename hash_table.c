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

hashtable init_hash(int n)
{
    int i;
    hashtable h;
    
    h.n = n;
    if(n==0){
        fprintf(stderr, "Hashtable cannot have a size of 0");
        return h;
    }
    h.alveole = malloc(sizeof(llist) * h.n);
    if (!h.alveole){
        fprintf(stderr, "Error in file hash-table.c, line %d\n", __LINE__);
        perror("malloc");
        return h;
    }
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
    int a;
    llist ptr, add;
    
    a = (sdbm(val) % h.p) % h.n;
    ptr = h.alveole[a];
    
    if(!ptr)
    {
        ptr = h.alveole[a] = malloc(sizeof(cell));
        if(!ptr){
            fprintf(stderr, "Error in file hash-table.c, line %d\n", __LINE__);
            perror("malloc");
            return NULL;
        }
        ptr->val = strdup(val);
        if(!ptr->val){
            fprintf(stderr, "Error in file hash-table.c, line %d\n", __LINE__);
            perror("strdup");
            return NULL;        
        }
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
        add = malloc(sizeof(cell));
        if(!add){
            fprintf(stderr, "Error in file hash-table.c, line %d\n", __LINE__);
            perror("malloc");
            return NULL;
        }
        add->val = strdup(val);
        if(!add->val){
            fprintf(stderr, "Error in file hash-table.c, line %d\n", __LINE__);
            perror("strdup");
            return NULL;         
        }
        add->next = NULL;
        ptr->next = add;
        return add->val;
    }
}

void free_hash(hashtable h)
{
    llist ptr, tmp;
    
    for(int i = 0; i < h.n; i++)
    {
        ptr = h.alveole[i];
        while(ptr)
        {
            tmp = ptr;
            ptr = ptr->next;
            free(tmp->val);
            free(tmp);
        }
        h.alveole[i] = NULL;
    }
    free(h.alveole);
}