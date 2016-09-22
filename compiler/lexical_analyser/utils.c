#include "utils.h"
void error(enum ERROR_TYPE type, char*string){
	char *error_str[]={
		"FATAL",
		"WARNING",
		"ERROR",
		NULL,
	};
	printf("%s:%s",*error_str[type],string);
	if(type == FATAL)
		exit(1);
}
void *do_malloc(long size){
	void *p;
	if(size==0)
		error(WARNING,"malloc size is zero");
	p=malloc(size);
	memset(p,0,size);
	if(p == NULL)
		error(FATAL,"malloc memory error!");
	return p;
}

void *do_free(void *p){
	free(p);
}