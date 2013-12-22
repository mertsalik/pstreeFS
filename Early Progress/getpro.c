#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned int main() {
	DIR *dir_p;
	struct dirent *dir_entry_p;
	char dir_name[40];										// ??? buffer overrun potential
	char target_name[252];	
	int target_result;
	char exe_link[252];
	int errorcount;
	int result;

	errorcount=0;
	result=0;
	dir_p = opendir("/proc/"); 																// Open /proc/ directory
	while(NULL != (dir_entry_p = readdir(dir_p))) {											// Reading /proc/ entries
		if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {		// Checking for numbered directories 
			strcpy(dir_name, "/proc/");
			strcat(dir_name, dir_entry_p->d_name);
			strcat(dir_name, "/"); 															// Obtaining the full-path eg: /proc/24657/ 
			exe_link[0] = 0;
			strcat(exe_link, dir_name);
			strcat(exe_link, "exe");													 	// Getting the full-path of that exe link
			target_result = readlink(exe_link, target_name, sizeof(target_name)-1);			// Getting the target of the exe ie to which binary it points to
			if (target_result > 0) {
				target_name[target_result] = 0;	
					result = atoi(dir_entry_p->d_name);
					printf("name: %s : id = %d   \n", target_name, result);			
			}
		}
	}
	closedir(dir_p);
	printf("getProcessID(%s) : id = 0 (could not find process)\n", target_name);
	return result;
}
