#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>

#define SEM_PRODUCER_DAEMON "/FilePasserDaemon"
#define SEM_CONSUMER_CLIENT "/FilePasser"
#define SEM_SHUTDOWN "/FilePasserShutdown"

#define BLOCK_SIZE 4096
#define IPC_RESULT_ERROR (-1)

#define SEM_PRODUCER_NAME "/FilePasserClient"
#define SEM_CONSUMER_NAME "/FilePasserServer"

typedef struct ClientMessageStructure {
    pid_t pid;
    char dir[256];
}   CMS;

static int get_shared_block(char *filename, int size){

	key_t key;

	key = ftok(filename, 0);

	if(key == IPC_RESULT_ERROR){
		return IPC_RESULT_ERROR;
	}

	return shmget(key,size, 0644 | IPC_CREAT);
}

char * attach_memory_block(char *filename, int size){
	int shared_block_id = get_shared_block(filename, size);
	char *result;
	result = shmat(shared_block_id, NULL, 0);
	if(result == (char *)IPC_RESULT_ERROR){
		return NULL;
	}
	return result;
}

bool detach_memory_block(char *block){
	return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroy_memory_block(char *filename){
	int shared_block_id = get_shared_block(filename, 0);
	if(shared_block_id == IPC_RESULT_ERROR){
		return NULL;
	}
	return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
	}

long int file_size(FILE *ptr) {
    fseek(ptr,0,SEEK_END);
    long int size = ftell(ptr);
    fseek(ptr,0,SEEK_SET);
    return size;
}