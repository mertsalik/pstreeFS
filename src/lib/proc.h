#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
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
