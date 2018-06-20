#include <stdio.h>
#include<stdlib.h>
#include <string.h>

/*
int main(void)
{
    int i = 0;
    char* strArray[50];
    char writablestring[]= "ls -l | gedit one.txt | gcc hello.c";
    char *token = strtok(writablestring,"|");


    while(token != NULL)
    {
        strArray[i] = malloc(strlen(token) + 1);
        strcpy(strArray[i], token);
        printf("%s\n", token);
        token = strtok(NULL, " ");
        i++;
    }
    return 0;
} */

int main()
{
    //char str[] = "ls -l|gedit one.txt|gcc hello.c";
    char str[] = "ssv=ls -l";
    char *token;
    char *rest = str;
    int i = 0;
    char* strArray[50];

    //char** arr[50];
    //arr = (char**)malloc(50*sizeof(char*))

    while ((token = strtok_r(rest, "=", &rest)))
    {
      strArray[i] = malloc(strlen(token) + 1);
      strcpy(strArray[i], token);
      printf("%s\n", strArray[i]);
      i++;
    }
    return(0);
}
