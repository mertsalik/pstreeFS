#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/proc.h"


static int pstreeFS_getattr(const char *path, struct stat *st_data)
{
	int res = 0;
	memset(st_data, 0, sizeof(struct stat));

	if(strcmp(path,"/")==0){
		st_data->st_mode = S_IFDIR | 0755;
		st_data->st_nlink = 2;
		return 0;
	}

	int is_info = 0;
	char p[2048];
	char p2[2048];
	char dest[100];
	strncpy(p2,path,strlen(path)+1);
	strncpy(p,path,strlen(path)+1);
	printf("getattr -> path : %s\n",p);
	printf("getattr -> rpath : %s\n",path);
	is_info = is_info_query(p2);
	printf("getattr -> is_info : %d\n",is_info);
	if(!is_info){
		printf("CHECK FOR DIRECTORY!!!");
		strncpy(p,path,strlen(path));
		parse_path(p,dest);
		printf("getattr -> dest: %s\n",dest);
	// if dest -1 -> wrong path
	// if dest is empty its root
	// others are proc folders
	if(strcmp(dest,"")==0 || strcmp(dest,"0")==0){
		st_data->st_mode = S_IFDIR | 0755;
		st_data->st_nlink = 2;
	}else if(strcmp(dest,"-1")==0){
		res = -ENOENT;
	}else{
		node *head = (struct node*) malloc(sizeof(struct node));
		read_proc_list(head);
		while(head!=NULL){
			if(strcmp(dest,head->name)==0){
				st_data->st_mode = S_IFDIR | 0755;
				st_data->st_nlink = 2;
				break;
			}
			head = head->next;
		}
	}

	}else{
		printf("Looking for info.txt\n");
		// LOOKING FOR info.txt
		strncpy(p,path,strlen(path)+1);
		clear_info_query(p);
		parse_path(p,dest);
		if(strcmp(dest,"")==0 || strcmp(dest,"0")==0){
			// no process path like this
			res = -ENOENT;
		}else{
			node *head = (struct node*) malloc(sizeof(struct node));
			read_proc_list(head);
			while(head!=NULL){
				if(strcmp(dest,head->name)==0){
					printf("Here it is INFO.TXT FILE!!\n");
					st_data->st_mode = S_IFREG | 0444;
					st_data->st_nlink = 1;
					size_t size = 0;
					char buffer[20000];
					read_proc_stat(head->name, buffer);
					size += strlen(buffer);
					read_sched_file(head->name, buffer);
					size += strlen(buffer);
					read_status_file(head->name, buffer);
					size += strlen(buffer);
					st_data->st_size = size;
					break;
				}
				head = head->next;
			}
		}
	}

	return 0;
}

static int pstreeFS_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	int res = 0;
	if(strcmp(path,"/")==0){
		filler(buf, "0", NULL, 0);
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		return res;
	}

	char p[2048];
	char dest[100];
	strcpy(p,path);
	printf("readdir -> path : %s\n",p);
	parse_path(p,dest);
	printf("readdir -> dest : %s\n",dest);
	// if dest -1 -> wrong path
	// if dest is empty its root
	// others are proc folders

	if(strcmp(dest, "")==0){
		filler(buf, "0", NULL, 0);
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
	}
	else if(strcmp(dest, "-1")==0){
		res = -ENOENT;
	}else{
		node *head = (struct node*) malloc(sizeof(struct node));
		read_proc_list(head);
		while(head!=NULL){
			char stat[2048];
			read_proc_stat(head->name,stat);
			char ppid[20];
			get_ppid_from_stat(stat,ppid);
			if(strcmp(dest,ppid)==0){
				char name[200];
				pid_to_name(head->name,name);
				filler(buf, name, NULL, 0);
			}
			head = head->next;
		}
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		if(strcmp(dest, "0")!=0){
			filler(buf, "info.txt", NULL, 0);
		}
	}
	return res;
}
static int pstreeFS_open(const char *path, struct fuse_file_info *fi)
{
	int res;

	int flags = fi->flags;
	if ((flags & O_WRONLY) || (flags & O_RDWR) || (flags & O_CREAT) || (flags & O_EXCL) || (flags & O_TRUNC) || (flags & O_APPEND)) {
		return -EROFS;
	}

	return 0;
}

static int pstreeFS_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{

	char p[2048];
	char dest[30];
	strncpy(p,path,strlen(path)+1);
	clear_info_query(p);
	parse_path(p,dest);

	printf("read -> dest : %s\n",dest);
	printf("read -> path : %s\n",path);
	size_t len;
	(void) fi;
	char info_str[600000];
	char buffer[200000];
	read_proc_stat(dest,buffer);
	strcat(info_str,buffer);
	read_sched_file(dest,buffer);
	strcat(info_str,buffer);
	read_status_file(dest,buffer);
	strcat(info_str,buffer);
	
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

/**
 *	Create a file node
 */
static int pstreeFS_mknod(const char *path, mode_t mode, dev_t rdev)
{
    (void)path;
    (void)mode;
    (void)rdev;
    return -EROFS;
}

/**
 *	Create directory
 */
static int pstreeFS_mkdir(const char *path, mode_t mode)
{
    (void)path;
    (void)mode;
    return -EROFS;
}

/**
 *	Remove a file
 */
static int pstreeFS_unlink(const char *path)
{
    (void)path;
    return -EROFS;
}

/**
 * 	Remove a directory
 */
static int pstreeFS_rmdir(const char *path)
{
    (void)path;
    char p[2048];
    char dest[30];
    strncpy(p,path,strlen(path)+1);
    parse_path(p,dest);
    printf("rm dest: %s\n",dest);
    kill_proc(dest);
    
    return 0;
    //return -EROFS;
}

/**
 * 	Create a symbolic link
 */
static int pstreeFS_symlink(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EROFS;
}

/**
 *	Rename a file
 */
static int pstreeFS_rename(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EROFS;
}

/**
 *	Create a hard link to a file
 */
static int pstreeFS_link(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EROFS;
}


/**
 * 	Change the permission bits of a file
 */
static int pstreeFS_chmod(const char *path, mode_t mode)
{
    (void)path;
    (void)mode;
    return -EROFS;

}

/**
 *	Change the owner and group of a file
 */
static int pstreeFS_chown(const char *path, uid_t uid, gid_t gid)
{
    (void)path;
    (void)uid;
    (void)gid;
    return -EROFS;
}

/**
 *	Change the size of a file
 */
static int pstreeFS_truncate(const char *path, off_t size)
{
    (void)path;
    (void)size;
    return -EROFS;
}

/**
 *	Change the access and modification times of a file
 */
static int pstreeFS_utime(const char *path, struct utimbuf *buf)
{
    (void)path;
    (void)buf;
    return -EROFS;
}

/**
 *	Write to file
 */
static int pstreeFS_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
    (void)path;
    (void)buf;
    (void)size;
    (void)offset;
    (void)finfo;
    return -EROFS;
}



static struct fuse_operations pstreeFS_oper = {
    .getattr	= pstreeFS_getattr,
    .readdir	= pstreeFS_readdir,
    .open		= pstreeFS_open,
    .read		= pstreeFS_read,
    .mknod 		= pstreeFS_mknod,
    .symlink 	= pstreeFS_symlink,
    .unlink		= pstreeFS_unlink,
    .rmdir		= pstreeFS_rmdir,
    .rename 	= pstreeFS_rename,
    .link		= pstreeFS_link,
    .chmod		= pstreeFS_chmod,
    .chown		= pstreeFS_chown,
    .truncate	= pstreeFS_truncate,
    .utime		= pstreeFS_utime,
    .write		= pstreeFS_write,
};

int main(int argc, char *argv[]){
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	
	fuse_main(args.argc, args.argv, &pstreeFS_oper, NULL);

    return 0;
}
