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

	char p[2048];
	char dest[100];
	strcpy(p,path);
	//parse_path(p,dest);

	res = -ENOENT;
	return 0;
}

static int pstreeFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	int res = 0;

	char p[2048];
	char dest[100];
	strcpy(p,path);
	parse_path(p,dest);

	res = -ENOENT;
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
