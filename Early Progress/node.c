#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "node.h"


int getNumberOfChildren(char * pid)
{
	int counter=0;
	char cmd[100];
	sprintf(cmd,"pgrep -P %s",pid);
    FILE *fp = popen( cmd, "r");
    if (fp == NULL)
    {
        printf("ERROR!\n");
    }
	
    char parentID[256];
    while (fscanf(fp, "%s", parentID) != EOF)
    {
		counter++;
    }
    pclose(fp);

    return counter;
}
char * getNamesOfTheProcess(char *pid,int number)
{
	int i=0;
		char** result = (char**)malloc(number * sizeof(char*));
	int index;
	for (index=0;index<number;++index)
	{
		result[index] = (char*)malloc(10 * sizeof(char));
	}
	char cmd[100];
	sprintf(cmd,"ps -p %s -o comm=",pid);
    FILE *fp = popen( cmd, "r");
    if (fp == NULL)
    {
        printf("ERROR!\n");
    }
	
    char ID[10];
    while (fscanf(fp, "%s", ID) != EOF)
    {
			strcpy(result[0],ID);
    }

    pclose(fp);
    return result[0];
}

char * getPidsOfTheChildren(char *pid,int number)
{
	int i=0;
	char** result = (char**)malloc(number * sizeof(char*));
	int index;
	for (index=0;index<number;++index)
	{
		result[index] = (char*)malloc(10 * sizeof(char));
	}
	char cmd[100];
	sprintf(cmd,"pgrep -P %s",pid);
    FILE *fp = popen( cmd, "r");
    if (fp == NULL)
    {
        printf("ERROR!\n");
    }
	
    char ID[10];
    while (fscanf(fp, "%s", ID) != EOF)
    {
			strcpy(result[i],ID);
			i++;
    }

    pclose(fp);

    return result[number-1];
}
//Node * getChildren(char * pid);
