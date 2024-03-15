#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>

#define BLOCK_SIZE 4096
#define IPC_RESULT_ERROR (-1)

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
	
int main(int argc, char *argv[]){
	char str[20]="/proc/";
	strcat(str,argv[1]);
	char *block = attach_memory_block(str, BLOCK_SIZE);

	if(block == NULL){
		printf("Error! Unable to get shm block!");
		return -1;
	}

	sem_unlink(SEM_CONSUMER_NAME);
	sem_unlink(SEM_PRODUCER_NAME);
	
	sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0660, 0);
	sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0660, 1);
	
    FILE *file_ptr = fopen("Kopija", "wb");
    unsigned char bytes[BLOCK_SIZE];
    long int blocks_num;
	int size_of_remainder;

    // recieving amount of blocks
    sem_wait(sem_prod);
    memcpy(&blocks_num,block,sizeof(long int));
	sem_post(sem_cons);
	// recieving size of remainder
	sem_wait(sem_prod);
    memcpy(&size_of_remainder,block,sizeof(int));
	sem_post(sem_cons);

    // recieving file blocks
	for (int i = 0; i < blocks_num; i++) {
		sem_wait(sem_prod);
        memcpy(bytes,block,BLOCK_SIZE);
        fwrite(bytes,1,BLOCK_SIZE,file_ptr);
		sem_post(sem_cons);
	}

	//recieving file remainder
	sem_wait(sem_prod);
    memcpy(bytes,block,size_of_remainder);
    fwrite(bytes,1,size_of_remainder,file_ptr);
	sem_post(sem_cons);
	
	sem_close(sem_cons);
	sem_close(sem_prod);
	detach_memory_block(block);
	fclose(file_ptr);
	return 0;
}