pstreeFS
========

Fake pstree application using Fuse


***Description***

Using FUSE, implement a file system (pstreeFS) that will let the user navigate the process hierarchy as folders and files, similar to the output of the "pstree" command. Every process in the system has to be represented as a folder. The name of the folder has to be a combination of the name and the pid of the program associated with the process. The child processes of a processes have to be represented as subfolders of the parent process folder. Every process folder has to contain a text file called "info.txt" which provides information about that process, such as its cmdline (found in /proc/<pid>/cmdline), state (found in /proc/<pid>/status) and scheduler information (as in /proc/<pid>/sched). The files and folders in the file system will be read-only with the exception that removing a directory will send a SIGKILL signal to the associated process
