#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#ifdef __APPLE__
	#ifndef st_mtime
		#define st_mtime st_mtimespec.tv_sec
	#endif
#endif

typedef void(*userfct)(void);
int global_main = 0;
int foo(int a)
{
	printf("foo in main.c, global_main : %d\n", global_main);
	return a + 1;
}

int main()
{
	userfct *userptr = NULL;
	userptr = malloc(sizeof(userfct) * 4);

	void *handle;
	char *error;

	handle = dlopen("./shared.so", RTLD_LAZY);
    if (!handle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();
	
	userptr[0] = dlsym(handle, "fct");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s\n", error);
        exit(1);
    }

    userptr[0]();
    userptr[0]();

	dlclose(handle);
	/*
	struct stat attr;
	stat("./shared.so", &attr);
	time_t t = attr.st_mtime;
	struct tm lt;
	localtime_r(&t, &lt);
	char out[20];
	strftime(out, 20, "%H:%M:%S", &lt);
	//clock = gmtime(&(attr.st_mtime));
	printf("\ntime : %s\n", out);
	*/
	return 0;
}