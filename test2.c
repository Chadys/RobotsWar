#include <math.h>
#include <stdio.h>
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

unsigned long int sdbm(unsigned char *str)
{
    unsigned long hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

int murmur3(const char* key, size_t len) {
  int h = 0;
  if (len > 3) {
    const int* key_x4 = (const int*) key;
    size_t i = len >> 2;
    do {
      int k = *key_x4++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h += (h << 2) + 0xe6546b64;
    } while (--i);
    key = (const char*) key_x4;
  }
  if (len & 3) {
    size_t i = len & 3;
    int k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
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

hashtable init(int n)
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

int main(int argc, char **argv)
{
	hashtable h = init(200);
	for(int i = 0; i < 100; i++)
	{
		char *add = malloc(sizeof(char) * 20);
		sprintf(add, "test%d", i);
		printf("%s\n", get_or_insert(&h, add));
	}
	
	for(int i = 0; i < h.n; i++)
	{
		printf("%2d :", i);
		llist ptr = h.alveole[i];
		while(ptr)
		{
			printf(" %s", ptr->val);
			ptr = ptr->next;
		}
		printf("\n");
	}
}