client: Client.c
	gcc Client.c -o client
server: Server.c
	gcc Server.c -o server
daemon:
	gcc FilePasserDaemon.c -o daemon
passer:
	gcc FilePasser.c -o passer