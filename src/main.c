#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "lib/proc.h"

static const char *info_path = "/0/info.txt";
static const char *root = "/";

static int pstreeFS_getattr(const char *path, struct stat *st_data)
{
	int res = 0;
	memset(st_data, 0, sizeof(struct stat));
	// read all procs
	node *head = (struct node*) malloc(sizeof(struct node));
	read_proc_list(head);
	// temp dir variable
	char t_dir[2048];
	strcpy(t_dir,"/0/");

	if(strcmp(path, root)==0 || strcmp(path, "/0")==0){
		st_data->st_mode = S_IFDIR | 0755;
		st_data->st_nlink = 2;
	}else{
		int found = 0;
		while(head!=NULL){
			strcat(t_dir, head->name);
			if(strcmp(path, t_dir)==0){
				st_data->st_mode = S_IFDIR | 0755;
				st_data->st_nlink = 2;
				found = 1;
			}else{
				strcpy(t_dir,"/0/");
			}
			head = head->next;
		}
		if(!found)
			res = -ENOENT;
	}
	
    return 0;
}

static int pstreeFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	int res = 0;
	// if its root we have to show init proc folder only
	if(strcmp(path, root)==0){
		filler(buf, "0",NULL, 0);
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
	}else if(strcmp(path, "/0")==0){
		node *head = (struct node*) malloc(sizeof(struct node));
		head->next = NULL;
		read_proc_list(head);
		while(head!=NULL){
			char stat_content[2048];
			read_proc_stat(head->name,stat_content);
			char ppid[10];
			get_ppid_from_stat(stat_content,ppid);
			if(strcmp(ppid,"0")==0){
				filler(buf, head->name, NULL, 0);
			}
			head = head->next;
		}
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		filler(buf, info_path+3, NULL, 0);
	}
	else{
		// get childs of given process id
		res = -ENOENT;
	}

	return res;
}
static int pstreeFS_open(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

static int pstreeFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	char *info_str = "Hello World!\n";
	size_t len;
	(void) fi;
	if(strcmp(path, info_path) != 0)
		return -ENOENT;
	len = strlen(info_str);
	if(offset < len){
		if( offset + size > len ){
			size = len - offset;
		}
		memcpy(buf, info_str + offset, size);
	}else{
		size = 0;
	}
	return size;
}

static struct fuse_operations pstreeFS_oper = {
    .getattr	= pstreeFS_getattr,
    .readdir	= pstreeFS_readdir,
    .open	= pstreeFS_open,
    .read	= pstreeFS_read,
};

int main(int argc, char *argv[]){
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	
	fuse_main(args.argc, args.argv, &pstreeFS_oper, NULL);

    return 0;
}
