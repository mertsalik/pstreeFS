#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_proc_stat(char *pid, char *buf){
	char cmd[2048];
	sprintf(cmd,"/proc/%s/stat",pid);
	FILE *stat = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, stat) != -1){
		//puts(arg);
	}
	strncpy(buf, arg, size+1);
	free(arg);
	fclose(stat);
}

/**
 * Returns 4. item on STAT FILE
 */
void get_ppid_from_stat(char *stat_info, char *buf){
	int i = 0;
	char *pch;
	pch = strtok(stat_info, " ");
	while(pch!=NULL){
		if(i==3){
			break;
		}
		pch = strtok(NULL, " ");
		i++;
	}
	strncpy(buf,pch,strlen(pch)+1);
}

int main(){
	char result[2048];
	read_proc_stat("5",result);
	printf("%s\n",result);
	char ppid[10];
	get_ppid_from_stat(result,ppid);
	printf("\nPPID: %s\n",ppid);
	return 0;
}

