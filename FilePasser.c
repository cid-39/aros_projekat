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

    //otvaranje pipe-a
    pipeStream = fopen(pipeName,"ab");

    CMS TempMsg;
    TempMsg.pid = getpid();
    strcpy(TempMsg.dir,argv[1]);

    pid_t pid = fork();
    if (pid==0) {
        char cmd[356];
        strcpy(cmd,"./client ");
        char tmpstr[10];
        sprintf(tmpstr,"%d",TempMsg.pid);
        strcat(cmd,tmpstr);
        printf("%s", cmd);
        system(cmd);
    }

    fwrite(&TempMsg,sizeof(CMS),1,pipeStream);
    fclose(pipeStream);
    sleep(5);
    return 0;
 }
