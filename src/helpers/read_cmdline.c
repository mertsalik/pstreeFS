#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_name_of_proc(char *pid, char *result_buf){
	char cmd[1000];
	sprintf(cmd,"/proc/%s/cmdline",pid);
	//printf("%s",cmd);
	FILE *cmdline = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, cmdline) != -1){
		//puts(arg);
	}
	strncpy(result_buf, arg, size+1);
	free(arg);
	fclose(cmdline);
}


int main(int argc, char **argv){
	char result[200];
	get_name_of_proc("1", result);
	printf("%s\n",result);
	return 0;
}
