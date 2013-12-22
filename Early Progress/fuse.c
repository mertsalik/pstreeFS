/*
    FUSE: Filesystem in Userspace
    Copyright (C) 2001-2005  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static struct node{
	int id;
	int pid;
	char *dir;
	struct node *next;
};


static struct node2{
	int id;
	int pid;
	char *dir;
    int childCount;
	struct node2 *next;
};

static struct node *front, *rear;
static struct node2 *root;
static struct node2 *ll;
/////////////////////////////// QUEUE start
static void queueInit(){
    front = NULL;
    rear = NULL;
}

static int isQueueEmpty(){
    
    if(!front)
        return 1;
    
    return 0;
}

static void push(int newID,char *newDir, int newPID){
    struct node *temp;
    
    temp =(struct node*) malloc(sizeof(struct node));
    temp->dir =(char*) malloc(strlen(newDir) * sizeof(char) +1);
    temp->id = newID;
    temp->pid = newPID;
    strcpy(temp->dir,newDir);
    temp->next = NULL;
    
    if(isQueueEmpty() == 1)
        front = temp;
    else
        rear->next = temp;
    
    rear = temp;
   
}

static struct node* pop(){
    struct node *q = front;
    
    if(isQueueEmpty() == 1)
        printf("\nQueue is Empty\n");
    else{
        q = front;
        front = front->next;
        q->next = NULL;
    }
    
    return q;
}
/////////////////////////////////// QUEUE stop

////////////////////////////////// LIST start

static void listInit(){
    root = malloc(sizeof(struct node2));
    root->dir = malloc(sizeof(char) * 6);
    root->id = -1;
    root->pid = -1;
    root->childCount = -1;
    strcpy(root->dir, "dumm1");
    
    root->next = malloc(sizeof(struct node2));
    root->next->dir = malloc(sizeof(char) * 6);
    root->next->id = -2;
    root->next->childCount = -2;
    strcpy(root->next->dir, "dumm2");
    root->next->next = NULL;
}

static int isListEmpty(){
    
    if(root->next->id == -2)
        return 1;
    
    return 0;
}

static void listAdd(int newID,int newPID, char *newDir, int newChildCount){
	
    static struct node2 *temp;
    
    temp =malloc(sizeof(struct node2));
    memset(temp, 0, sizeof(*temp));
    temp->dir = malloc(strlen(newDir) * sizeof(char) +1 );
    memset(temp->dir, 0, sizeof(*temp->dir));
    temp->id = newID;
    temp->pid = newPID;
    temp->childCount = newChildCount;
    strcpy(temp->dir,newDir);
    
    temp->next = root->next;
    root->next = temp;
}
/*End of list operations*/
//FUSE OPERATIONS
static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	
	if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
	}else if(strcmp(path,"/0") == 0){
		stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
		}else{
			struct node2 *temp;
			temp = root->next;
			while(temp->id != -2){
					char * tempPath = malloc(sizeof(char)*strlen(temp->dir) +10000);
					strcpy(tempPath, temp->dir);
					strcat(tempPath, "/");
					char *ito = malloc(sizeof(char)*10);
					strcpy(ito,"");
					sprintf(ito,"%d",temp->id);
					strcat(tempPath,ito);
					if(strcmp(path,tempPath) == 0){			
								stbuf->st_mode = S_IFDIR | 0755;
								stbuf->st_nlink = 2;
							}
							temp = temp->next;
				}
			
		}
	
   /*
	int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
     if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
	}else if(strcmp(path,"/0")==0){
		stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
		}else if(strcmp(path,"/0/1")==0){
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}else if(strcmp(path,"/0/2")==0){
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
	*/
    return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;
     if(strcmp(path,"/")==0){
		filler(buf, "0",NULL, 0);
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		}else{
    struct node2 *temp;
    temp = root->next;
    while(temp->id != -2){
		if(strcmp(path,temp->dir)==0){
			char *str = malloc(sizeof(char)*10);
			sprintf(str,"%d",temp->id);
			filler(buf, str,NULL,0);
			}
		temp= temp->next;
		}
    }
	/*
    if(strcmp(path,"/")==0){
		filler(buf, "0",NULL, 0);
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		}else if(strcmp(path,"/0")==0){
			filler(buf,"1",NULL,0);
			filler(buf,"2",NULL,0);
			}
			* */
    return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{

    return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	
    return size;
}

static struct fuse_operations hello_oper = {
    .getattr	= hello_getattr,
    .readdir	= hello_readdir,
    .open	= hello_open,
    .read	= hello_read,
};


int main(int argc, char *argv[])
{
	/*Parent child loop logic*/
    queueInit();
    listInit();
    
    push(0,"",-1);
    
    char str[100];
    char incomePID[100];
    int w1 = 0;
    int w2 = 0;
    
    while(isQueueEmpty() == 0){
		struct node *tempNode;
		 tempNode = pop();
		
		sprintf (str, "pgrep -P %d", tempNode->id);
		FILE *fp = popen(str,"r");
		
		while(fscanf(fp,"%s",incomePID) != EOF){
			char *nDir = malloc(strlen(tempNode->dir) * sizeof(char) +8);
			memset(nDir, 0, sizeof(*nDir));
			int cid = atoi(incomePID);
			strcpy(nDir, tempNode->dir);
			strcat(nDir,"/");
			char * tempStr = malloc(sizeof(char)*20);
			sprintf(tempStr,"%d",tempNode->id);
			strcat(nDir, tempStr);
			/*
			strcpy(nDir,tempNode->dir);
			strcat(nDir,"/");
			strcat(nDir,incomePID);
			* */
			push(cid,nDir,tempNode->id);
			w2++;
        }
        listAdd(tempNode->id,tempNode->pid, tempNode->dir, w2);
        w2 = 0;
        w1++;
        free(tempNode);
    }
    struct node2 *temp;
    temp = root->next;
    while(temp->next->id!=-2){
		printf("pid:%d, id:%d, dir:%s, child:%d\n",temp->pid,temp->id,temp->dir,temp->childCount);
		temp = temp->next;
		}
		ll = root->next;
		/*Now we have all the proceses with it's id, parent id and child #*/
		temp = root->next;
		while(temp->id != -2){
					char * tempPath = malloc(sizeof(char)*strlen(temp->dir) +10000);
					strcpy(tempPath, temp->dir);
					strcat(tempPath, "/");
					char *ito = malloc(sizeof(char)*10);
					sprintf(ito,"%d",temp->id);
					strcat(tempPath,ito);
					
					printf("%s\n\n",tempPath);
							temp = temp->next;
				}
    return fuse_main(argc, argv, &hello_oper, NULL);
}