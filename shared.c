#include <stdio.h>
extern int global_main;
int foo(int);
void fct()
{
	global_main++;
	printf("shared works : %d\n", foo(global_main));
}