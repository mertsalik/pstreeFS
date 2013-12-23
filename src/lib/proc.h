#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void removeChar(char *str, char garbage) {

    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

void get_name_of_proc(char *pid, char *result_buf){
		if(strcmp(pid,"0")!=0){
			char cmd[200];
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
        }else{
			strncpy(result_buf, "0", 2);
		}
}

void get_proc_name(char *pid, char *buf){
	
	char stat_info[1024];
	char cmd[1000];
	sprintf(cmd, "/proc/%s/stat",pid);
	FILE *status = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, status) != -1){
		//puts(arg)
	}
	strncpy(stat_info,arg,size+1);
	free(arg);
	fclose(status);
	
	int i = 0;
	char *pch;
	char *saveptr;
	pch = strtok_r(stat_info, " ", &saveptr);
	while(pch!=NULL){
		if(i==1){
			break;
		}
		pch = strtok_r(NULL, " ", &saveptr);
		i++;
	}
	removeChar(pch,'(');
	removeChar(pch,')');
	strncpy(buf,pch,strlen(pch)+1);
}

void pid_to_name(char *pid,char *name){
	char result[200];
	get_proc_name(pid,result);
	if(strcmp(result,"")==0){
		strncpy(name, "@", 2);
	}else{
		strncpy(name,result,strlen(result)+1);
	}
	strcat(name, "#");
	strcat(name, pid);
}

void name_to_pid(char *name,char *pid){
	printf("name_to_pid : %s\n",name);
	if(strcmp(name,"0")!=0){
		char n[200];
		strncpy(n,name,strlen(name)+1);
		char *pch;
		char *saveptr;
		pch = strtok_r(n, "#", &saveptr); // name is on the pch now
		if(pch!=NULL){
			pch = strtok_r(NULL, "#", &saveptr);
			if(pch!=NULL){
				strncpy(pid,pch,strlen(pch)+1);
			}else{
				strncpy(pid,"-1",3);
			}
		}else{
			strncpy(pid,"-1",3);
		}
	}else{
		strncpy(pid,"0",2);
	}
}

void read_sched_stat(char *pid, char *buf){
	char cmd[1000];
	sprintf(cmd,"/proc/%s/sched",pid);
	FILE *sched = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, sched) != -1){
		//puts(arg)
	}
	strncpy(buf, arg, size+1);
	free(arg);
	fclose(sched);
}

void read_status_file(char *pid, char *buf){
	char cmd[1000];
	sprintf(cmd, "/proc/%s/sched",pid);
	FILE *status = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, status) != -1){
		//puts(arg)
	}
	strncpy(buf,arg,size+1);
	free(arg);
	fclose(status);
}


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

void get_proc_info(char *pid, char *buf){
	//char cmdline[100];
	//char sched[2048];
	char status[2048];
	//get_name_of_proc(pid,cmdline);
	//read_sched_stat(pid,sched);
	read_status_file(pid,status);
	//strcat(result,"\n_________");
	//strcat(result,cmdline);
	//strcat(result,"__________\n");
	//strcat(result,status);
	//strcat(result,"\n_________\n");
	//strcat(result,sched);
	//strncpy(buf,result,strlen(result)+1);
	strncpy(buf,status,strlen(status)+1);
}

/**
 * Returns 4. item on STAT FILE
 */
void get_ppid_from_stat(char *stat_info, char *buf){
	int i = 0;
	char *pch;
	char *saveptr;
	pch = strtok_r(stat_info, " ", &saveptr);
	while(pch!=NULL){
		if(i==3){
			break;
		}
		pch = strtok_r(NULL, " ", &saveptr);
		i++;
	}
	strncpy(buf,pch,strlen(pch)+1);
}

typedef struct node{
	char name[2048];
	struct node *next;
}node;

int is_proc_folder(char *dname){
	int i ;
	for(i=0;i<strlen(dname);i++){
		if(!isdigit(dname[i])){
			return 0;
		}
	}
	return 1;
}

void read_proc_list(node* head){
	node* temp;
	temp = head;
	struct dirent *dp;
	DIR *dfd = opendir("/proc");
	if(dfd != NULL) {
		while((dp = readdir(dfd)) != NULL){
			if(is_proc_folder(dp->d_name)){
				strncpy(temp->name,dp->d_name,strlen(dp->d_name)+1);
				node* nnode = (struct node*) malloc(sizeof(struct node));
				nnode->next = NULL;
				temp->next = nnode;
				temp = nnode;
			}
		}
		closedir(dfd);
	}
}

/**
 *	If path is not correct
 *	It returns -1
 */
void parse_path(char *path, char *result){
	char *pch;
	char tmp[20];
	char *saveptr;
	pch = strtok_r(path, "/", &saveptr);
	while(pch!=NULL){
		char pid[20];
		name_to_pid(pch,pid);
		if(!is_proc_folder(pid)){
			strncpy(tmp,"-1",3);
			break;
		}
		if(strcmp(pid,"0")==0){
			strncpy(tmp,pid,strlen(pid)+1);
			pch = strtok_r(NULL, "/", &saveptr);
			continue;
		}
		char stat[2048];
		char pid2[20];
		strncpy(pid2,pid,strlen(pid)+1);
		read_proc_stat(pid2,stat);
		char ppid[20];
		get_ppid_from_stat(stat,ppid);
		if(strcmp(ppid,tmp)==0){
			strncpy(tmp,pid,strlen(pid)+1);
			pch = strtok_r(NULL, "/", &saveptr);
		}else{
			strncpy(tmp,"-1",3);
			break;
		}
	}
	strncpy(result,tmp,strlen(tmp)+1);
}

int is_info_query(char *path){
        char *pch;
        if(strstr(path,"info.txt") != NULL){
                return 1;
        }else{
                return 0;
        }
}

void removeSubstring(char *s, const char *toremove){
        while(s=strstr(s,toremove))
                memmove(s, s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

void clear_info_query(char *path){
        removeSubstring(path,"info.txt");
}



