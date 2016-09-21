#include <stdlib.h>
#include <stdio.h>

enum ERROR_TYPE{FATAL,WARNING,ERROR};

void error(enum ERROR_TYPE type,char *string);
void *do_malloc(int size);
void *do_free(void *p);