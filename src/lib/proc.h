#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include "string.h"

/**
 *	helper that retrieves short & usefull name of given pid
 *	@param char* pid, process id
 *	@param char* buf, result buffer
 */
void get_proc_name(char *pid, char *buf){
	char stat_info[1024];
	char cmd[100];
	sprintf(cmd, "/proc/%s/stat",pid);
	FILE *status = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, status) != -1){
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

/**
 *	Convention for getting folder name for given process id
 *	@param char* pid, process id
 *	@param char* name, result buffer
 */
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

/**
 *	Convention for getting pid of queried folder
 *	@param char* name, folder name
 *	@param char* pid, result buffer
 */
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

/**
 *	Reads scheduling information of given process id
 *	@param char* pid, process id
 *	@param char* buf, result buffer
 */
void read_sched_file(char *pid, char *buf){
	char cmd[1000];
	sprintf(cmd,"/proc/%s/sched",pid);
	FILE *sched = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, sched) != -1){
	}
	strncpy(buf, arg, size+1);
	free(arg);
	fclose(sched);
}

/**
 *	Reads /proc/pid/status file into buffer
 *	@param char* pid, process id
 *	@param char* buf, the result buffer
 */
void read_status_file(char *pid, char *buf){
	char cmd[1000];
	sprintf(cmd, "/proc/%s/status",pid);
	FILE *status = fopen(cmd,"rb");
	char *arg = 0;
	size_t size = 0;
	while(getdelim(&arg, &size, 0, status) != -1){
	}
	strncpy(buf,arg,size+1);
	free(arg);
	fclose(status);
}

/**
 *	Reads /proc/pid/stat file
 *	@param char* pid, process id
 *	@param char* buf, the result buffer
 */
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
 * Parses stat file content and returns fourth item (Parent Pid)
 * @param char* stat_info,	content of /proc/pid/stat file
 * @param char* buf, the result buffer
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

/**
 *	Data Struct for retrieving dynamic task list
 */
typedef struct node{
	char name[2048];
	struct node *next;
}node;

/**
 *	Checks for given character array is full of digit.
 *	@param char* dname, as pid
 */
int is_proc_folder(char *dname){
	int i ;
	for(i=0;i<strlen(dname);i++){
		if(!isdigit(dname[i])){
			return 0;
		}
	}
	return 1;
}

/**
 *	Reads tasks to given linked list.
 *	@param node* head, the head of linked list
 */
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
 *	Fills the char* result buffer with destination process pid
 *	If queried path is not valid it returns -1
 *	@param char* path, queried directory
 * 	@param char* result, the result buffer
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
/**
 * @param char* , queried directory
 * @return bool, is querying info.txt
 */
int is_info_query(char *path){
        char *pch;
        if(strstr(path,"info.txt") != NULL){
                return 1;
        }else{
                return 0;
        }
}

/**
 *	Clears the info.txt from path
 *	@param char* , queried directory
 */
void clear_info_query(char *path){
        removeSubstring(path,"info.txt");
}
