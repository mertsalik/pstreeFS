#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static int pstreeFS_getattr(const char *path, struct stat *st_data)
{
	int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
	
	// dummy 
	stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
	// dummy 
	
    return 0;
}

static int pstreeFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
    (void) fi;

	// dummy
	filler(buf, "0",NULL, 0);
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	// dummy
	
	return 0;
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