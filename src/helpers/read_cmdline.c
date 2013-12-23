#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	FILE *cmdline = fopen("/proc/1/cmdline","rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, cmdline) != -1){
		puts(arg);
	}
	free(arg);
	fclose(cmdline);
	return 0;
}
