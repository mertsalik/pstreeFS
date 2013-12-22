#ifndef NUL


#define PID_SIZE 10
#define NAME_SIZE 1000



typedef struct node
{
  char name[NAME_SIZE];
  char pid[PID_SIZE];

} Node;

int getNumberOfChildren(char * pid);
char * getNamesOfTheProcess(char * pid, int number);
char * getPidsOfTheChildren(char * pid, int number);
//Node * getChildren(char * pid);


 
#endif
