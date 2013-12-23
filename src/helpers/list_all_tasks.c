#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

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


int main(){
	node *head = (struct node*) malloc(sizeof(struct node));
	head->next = NULL;
	read_proc_list(head);
	while(head!=NULL){
		printf("%s\n",head->name);
		head = head->next;
	}
	return 0;
}
