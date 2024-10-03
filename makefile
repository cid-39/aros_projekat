all: client server daemon passer
 
client: FilePasser.h Client.c
	gcc Client.c -o client

server: FilePasser.h Server.c
	gcc Server.c -o server

daemon: FilePasser.h FilePasserDaemon.c
	gcc FilePasserDaemon.c -o daemon

passer: FilePasser.h FilePasser.c
	gcc FilePasser.c -o passer