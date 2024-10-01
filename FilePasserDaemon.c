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
    printf("   _____ __      ___              \n  / __(_) /__   / _ \\___ ____ ___ ___ ____\n / _// / / -_) / ___/ _ `(_-<(_-</ -_) __/\n/_/ /_/_/\\__/ /_/   \\_,_/___/___/\\__/_/   \n");
    char pipeName[] = "FilePasser";
    FILE *pipeStream;

    sem_unlink(SEM_CONSUMER_CLIENT);
	sem_unlink(SEM_PRODUCER_DAEMON);
    sem_unlink(SEM_SHUTDOWN);
	
	sem_t *sem_prod = sem_open(SEM_PRODUCER_DAEMON, O_CREAT, 0660, 0);
	sem_t *sem_cons = sem_open(SEM_CONSUMER_CLIENT, O_CREAT, 0660, 1);
    sem_t *sem_shut = sem_open(SEM_SHUTDOWN, O_CREAT, 0660, 1);

    // pravi se named pipe, ako uspe, otvara se
    if ((mkfifo(pipeName,0666)) != 0) {
        printf("Unable to create a pipe.\nCheck if the pipe already exists.\n");
        exit(1);
    }
    printf("Setup done, waiting for requests...\n");
    pipeStream = fopen(pipeName,"rb");

    CMS TempMsg;
    int shutdown;

    while (1) {
        sem_wait(sem_prod);
            sem_getvalue(sem_shut, &shutdown);
            if ( shutdown == 0 ) {
                printf("Recieved signal for shutdown.\n");
                break;
            }
            if ( fread(&TempMsg,sizeof(CMS),1,pipeStream) == 1 ) { 
                printf("Process %d requested %s\n", TempMsg.pid, TempMsg.dir);
                fflush(stdout);
                
                pid_t cpid = fork();
                if (cpid==0) {    
                    char *args[]={"./server",NULL,NULL};

                    char tmpstr[10];
                    sprintf(tmpstr,"%d",TempMsg.pid);
                    args[1] = tmpstr;

                    args[2] = TempMsg.dir;
                    
                    execv(args[0], args);
                }
            } else { printf("Error reading the structure.\n");}
        sem_post(sem_cons);
        
    }

    // Closing sequence
    if (remove("FilePasser") == 0) 
        printf("Closed pipe successfully!\n");
    else
        printf("Unable to delete the pipe, existing anyways...\n");
    
    printf("Bye.\n");
    return 0;
}