#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static const char *info_path = "/info.txt";

static int pstreeFS_getattr(const char *path, struct stat *st_data)
{
	int res = 0;
	memset(st_data, 0, sizeof(struct stat));

	if(strcmp(path, "/")==0){
		st_data->st_mode = S_IFDIR | 0755;
		st_data->st_nlink = 2;
	}else if(strcmp(path, info_path)==0){
		st_data->st_mode = S_IFREG | 0444;
		st_data->st_nlink = 1;
		st_data->st_size = 50;
	}else{
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
	if(strcmp(path,"/")==0){
		filler(buf, "0",NULL, 0);
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		filler(buf, info_path + 1, NULL, 0);
	}else{
		// get childs of given process id
		res = -ENOENT;
	}

	return res;
}
static int pstreeFS_open(const char *path, struct fuse_file_info *fi)
{
	// dummy
	return 0;
}

static int pstreeFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	// dummy
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
