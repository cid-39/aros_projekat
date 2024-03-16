#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>

#define SEM_PRODUCER_NAME "/FilePasserMyProducer"
#define SEM_CONSUMER_NAME "/FilePasserMyConsumer"

typedef struct ClientMessageStructure {
    pid_t pid;
    char dir[256];
}   CMS;

 int main(int argc, char *argv[]) {
    char pipeName[] = "FilePasser";
    FILE *pipeStream;

    sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, 0);
	sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, 1);

    //otvaranje pipe-a
    pipeStream = fopen(pipeName,"ab");

    CMS TempMsg;
    TempMsg.pid = getpid();
    strcpy(TempMsg.dir,argv[1]);

    pid_t cpid = fork();
    if (cpid==0) {
        char cmd[356];
        strcpy(cmd,"./client ");
        char tmpstr[10];
        sprintf(tmpstr,"%d",TempMsg.pid);
        strcat(cmd,tmpstr);
        //printf("%s\n", cmd);
        system(cmd);
        exit(0);
    }
    int status;
    sem_wait(sem_cons);
    fwrite(&TempMsg,sizeof(CMS),1,pipeStream);
    sem_post(sem_prod);
    fclose(pipeStream);
    wait(&status);
    return 0;
 }