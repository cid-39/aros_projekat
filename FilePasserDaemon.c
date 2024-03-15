#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

typedef struct ClientMessageStructure {
    pid_t pid;
    char dir[256];
}   CMS;

int main(int argc, char *argv[]) {
    char pipeName[] = "FilePasser";
    FILE *pipeStream;

    // pravi se named pipe, ako uspe, otvara se
    if ((mkfifo(pipeName,0666)) != 0) {
        printf("Unable to create a pipe\n");
        exit(1);
    }
    pipeStream = fopen(pipeName,"rb");

    CMS TempMsg;

    if ( fread(&TempMsg,sizeof(CMS),1,pipeStream) == 1 ) { 
        printf("%d --- %s\n", TempMsg.pid, TempMsg.dir);
    }
    char cmd[400];
    strcpy(cmd, "./server ");

    char tmpstr[10];
    sprintf(tmpstr,"%d",TempMsg.pid);
    strcat(cmd,tmpstr);
    strcat(cmd," ");
    strcat(cmd,TempMsg.dir);
    printf("%s", cmd);
    system(cmd);
    return 0;
}