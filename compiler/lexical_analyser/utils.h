#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

enum ERROR_TYPE{FATAL,WARNING,ERROR};

void error(enum ERROR_TYPE type,char *string);
void *do_malloc(long size);
void *do_free(void *p);