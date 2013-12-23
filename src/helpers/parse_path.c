#include <stdio.h>
#include "../lib/proc.h"


/**
 *	If path is not correct
 *	It returns -1
 */
void parse_path2(char *path, char *result){
	char *pch;
	char tmp[20];
	char *saveptr;
	pch = strtok_r(path, "/", &saveptr);
	while(pch!=NULL){
		if(!is_proc_folder(pch)){
			strcpy(tmp,"-1");
			break;
		}
		if(strcmp(pch,"0")==0){
			strncpy(tmp,pch,strlen(pch)+1);
			pch = strtok_r(NULL, "/", &saveptr);
			continue;
		}
		char stat[2048];
		char pid[20];
		strncpy(pid,pch,strlen(pch)+1);
		read_proc_stat(pid,stat);
		char ppid[20];
		get_ppid_from_stat(stat,ppid);
		if(strcmp(ppid,tmp)==0){
			strncpy(tmp,pch,strlen(pch)+1);
			pch = strtok_r(NULL, "/", &saveptr);
		}else{
			strcpy(tmp,"-1");
			break;
		}
		
		
	}
	strncpy(result,tmp,strlen(tmp)+1);
}

int main(){
	const char *zaa ="/autorun.inf";
	char d[2048];
	strcpy(d,zaa);
	char path[100] = "/0/12/5";
	char result[30];
	parse_path2(d,result);
	printf("%s",result);
	return 0;
}
