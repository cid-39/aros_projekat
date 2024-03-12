#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <fcntl.h>
#include <semaphore.h>

#define BLOCK_SIZE 4096
#define IPC_RESULT_ERROR (-1)
#define FILENAME "Server.c"

#define SEM_PRODUCER_NAME "/myproducer"
#define SEM_CONSUMER_NAME "/myconsumer"

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

int main(int argc, char *argv[]){

	char *block = attach_memory_block(FILENAME, BLOCK_SIZE);

	if(block ==NULL){
		printf("Error! Unable to get shm block!");
		return -1;
	}
	
	
    FILE *file_ptr;
    file_ptr = fopen(argv[1], "rb");
    if (file_ptr==NULL)
    {
        printf("Error opening a file\n");
        return -1;
    }

    long int size_of_file = file_size(file_ptr);
	
	sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, 0);
	
	sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, 1);
	
    unsigned char byte;
    // sending size of file
    sem_wait(sem_cons);
    memcpy(block,&size_of_file,sizeof(long int));
    sem_post(sem_prod);

    // sending file
	for(int i = 0; i< size_of_file; i++){
	sem_wait(sem_cons);
        fread(&byte,1,1,file_ptr);
        //printf("Pisanje %x u blok\n", byte);
        memcpy(block,&byte,1);
		sem_post(sem_prod);
	}
	
	sem_close(sem_prod);
	sem_close(sem_cons);
	detach_memory_block(block);
	fclose(file_ptr);
	return 0;
}