#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <fcntl.h>
#include <semaphore.h>

#include "FilePasser.h"

int main(int argc, char *argv[]){
	char str[20]="/proc/";
	strcat(str,argv[1]);
	char *block = attach_memory_block(str, BLOCK_SIZE);

	if(block ==NULL){
		printf("Error! Unable to get shm block!");
		return -1;
	}

	sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0660, 0);
	sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0660, 1);

    FILE *file_ptr;
    file_ptr = fopen(argv[2], "rb");
    if (file_ptr==NULL)
    {
        printf("Error opening a file\n");
        return -1;
    }
	
    long int size_of_file = file_size(file_ptr);
    unsigned char bytes[BLOCK_SIZE];
	long int blocks_num = size_of_file / BLOCK_SIZE;
	int size_of_remainder = size_of_file % BLOCK_SIZE;

    // sending amount of blocks
    sem_wait(sem_cons);
    memcpy(block,&blocks_num,sizeof(long int));
    sem_post(sem_prod);
	// sending size of remainder 
	sem_wait(sem_cons);
    memcpy(block,&size_of_remainder,sizeof(int));
    sem_post(sem_prod);

    // sending file blocks
	for(int i = 0; i < blocks_num; i++) {
		sem_wait(sem_cons);
        fread(bytes,1,BLOCK_SIZE,file_ptr);
        memcpy(block,bytes,BLOCK_SIZE);
		sem_post(sem_prod);
	}

	// sending file remainder
	sem_wait(sem_cons);
	fread(bytes,1,size_of_remainder,file_ptr);
    memcpy(block,bytes,size_of_remainder);
	sem_post(sem_prod);

	sem_close(sem_prod);
	sem_close(sem_cons);
	detach_memory_block(block);
	fclose(file_ptr);
	return 0;
}