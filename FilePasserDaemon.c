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

    sem_unlink(SEM_CONSUMER_NAME);
	sem_unlink(SEM_PRODUCER_NAME);
	
	sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0660, 0);
	sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0660, 1);

    // pravi se named pipe, ako uspe, otvara se
    if ((mkfifo(pipeName,0666)) != 0) {
        printf("Unable to create a pipe\n");
        exit(1);
    }
    pipeStream = fopen(pipeName,"rb");

    CMS TempMsg;

    while (1) {
        sem_wait(sem_prod);
            if ( fread(&TempMsg,sizeof(CMS),1,pipeStream) == 1 ) { 
                printf("%d --- %s\n", TempMsg.pid, TempMsg.dir);
                fflush(stdout);
                    char cmd[600];
                    strcpy(cmd, "./server ");
                    char tmpstr[10];
                    sprintf(tmpstr,"%d",TempMsg.pid);
                    strcat(cmd,tmpstr);
                    strcat(cmd," ");
                    strcat(cmd,TempMsg.dir);
                    //printf("%s\n", cmd);
                    system(cmd);
            } else { printf("Error reading the structure\n");}
        sem_post(sem_cons);
    }
    
    /* {
         char cmd[400];
         strcpy(cmd, "./server ");
         char tmpstr[10];
         sprintf(tmpstr,"%d",TempMsg.pid);
         strcat(cmd,tmpstr);
         strcat(cmd," ");
         strcat(cmd,TempMsg.dir);
         printf("%s", cmd);
         system(cmd);
     }
    */
    
    return 0;
}