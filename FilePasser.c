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

#include "FilePasser.h"

int main(int argc, char *argv[]) {
    char pipeName[] = "FilePasser";
    FILE *pipeStream;

    sem_t *sem_prod = sem_open(SEM_PRODUCER_DAEMON, 0);
	sem_t *sem_cons = sem_open(SEM_CONSUMER_CLIENT, 1);
    sem_t *sem_shut = sem_open(SEM_SHUTDOWN, O_CREAT, 0660, 1);

    //otvaranje pipe-a
    pipeStream = fopen(pipeName,"ab");

    if (strcmp(argv[1], "-s") == 0) {
        sem_wait(sem_cons);
        sem_wait(sem_shut);
        printf("Shutting down daemon...\n");
        sem_post(sem_prod);
        return 0;
    }
    
    CMS TempMsg;
    TempMsg.pid = getpid();
    strcpy(TempMsg.dir,argv[1]);

    pid_t cpid = fork();
    if (cpid==0) {
        char *args[]={"./client",NULL,NULL};
        
        char tmpstr[10];
        sprintf(tmpstr,"%d",TempMsg.pid);
        args[1] = tmpstr;

        char* tmpstr2 = strtok(argv[1],"/");
        char* filename; 
        while (1) {
            if ( tmpstr2 != NULL)
                filename = tmpstr2;
            else break;
            tmpstr2 = strtok(NULL, "/");
        }
        args[2] = filename;

        execv(args[0], args);
        exit(0);
    }

    int status;
    
    sem_wait(sem_cons);
    fwrite(&TempMsg,sizeof(CMS),1,pipeStream);
    sem_post(sem_prod);
    fclose(pipeStream);
    wait(&status);
    if (!status) printf("File recieved.\n");
    else printf("An error has occurred.\n");
    return 0;
 }