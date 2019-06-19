#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "wiringPi.h"


#define BUFSIZE 1000
#define PORT 397
#define QUEUE 3	//WEarteschlange fuer Verbindungen

//void writeLEDs(int state[4]);

void *mirror(void* arg){
	int clientfd = *(int *)arg;
	char inbuffer[BUFSIZE];
	int ende = 0;
	int count;
	//lesen der empfangen Zeichen
	while(ende == 0){
	count = read(clientfd, inbuffer, sizeof(inbuffer));
	switch(count){
		case 0: printf("0,0,0,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 0);
			digitalWrite(24, 0);
			digitalWrite(25, 0); break;
		case 1: printf("0,0,0,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 0);
			digitalWrite(24, 0);
			digitalWrite(25, 0); break;
		case 2: printf("0,0,1,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 1);
			digitalWrite(24, 0);
			digitalWrite(25, 0); break;
		case 3: printf("0,0,1,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 1);
			digitalWrite(24, 0);
			digitalWrite(25, 0); break;
		case 4: printf("0,1,0,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 0);
			digitalWrite(24, 1);
			digitalWrite(25, 0); break;
		case 5: printf("0,1,0,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 0);
			digitalWrite(24, 1);
			digitalWrite(25, 0); break;
		case 6: printf("0,1,1,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 1);
			digitalWrite(24, 1);
			digitalWrite(25, 0); break;
		case 7: printf("0,1,1,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 1);
			digitalWrite(24, 1);
			digitalWrite(25, 0); break;
		case 8: printf("1,0,0,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 0);
			digitalWrite(24, 0);
			digitalWrite(25, 1); break;
		case 9: printf("1,0,0,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 0);
			digitalWrite(24, 0);
			digitalWrite(25, 1); break;
		case 10: printf("1,0,1,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 1);
			digitalWrite(24, 0);
			digitalWrite(25, 1); break;
		case 11: printf("1,0,1,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 1);
			digitalWrite(24, 0);
			digitalWrite(25, 1); break;
		case 12: printf("1,1,0,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 0);
			digitalWrite(24, 1);
			digitalWrite(25, 1); break;
		case 13: printf("1,1,0,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 0);
			digitalWrite(24, 1);
			digitalWrite(25, 1); break;
		case 14: printf("1,1,1,0\n");
			digitalWrite(18, 0);
			digitalWrite(23, 1);
			digitalWrite(24, 1);
			digitalWrite(25, 1); break;
		case 15: printf("1,1,1,1\n");
			digitalWrite(18, 1);
			digitalWrite(23, 1);
			digitalWrite(24, 1);
			digitalWrite(25, 1); break;
		case 'q':	printf("0,0,0,0\n");
					digitalWrite(18, 0);
					digitalWrite(23, 0);
					digitalWrite(24, 0);
					digitalWrite(25, 0);
					write(clientfd, "LED", count);
					close(clientfd);
					ende = 1;
					break;
		default:printf("Sie haben eine ungueltige Zahl eingegeben\n"); break;
		
	} 
}
	
	//sendbuffer ==> clientsocket
	write(clientfd, "LED", count);
	close(clientfd);
	
}

int main(int argc, char *argv[])
{
	//wiringPi setup
	wiringPiSetupGpio();
	pinMode(18, OUTPUT);
	pinMode(23, OUTPUT);
	pinMode(24, OUTPUT);
	pinMode(25, OUTPUT);
	
	//Socket
	int server_socket, rec_socket;
	int len;
	struct sockaddr_in serverinfo, clientinfo;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	serverinfo.sin_family = AF_INET; //iPv4
	//hört auf allen Interfaces: 0.0.0.0
	serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);
	serverinfo.sin_port = htons(PORT);

	//verbinden
	if (bind(server_socket, (struct sockaddr *) &serverinfo, sizeof(serverinfo)) != 0){
		printf("Fehler socket!\n");
		return -1;
	}

	//Server wartet auf connect von Client
	listen(server_socket, QUEUE); 	
	
	// Endlosschleife Server zur Abarbeitung der Client Anfragen
	while(1){
		printf("Server wartet......\n");
		//Verbinung von Client
		rec_socket = accept(server_socket,(struct sockaddr*)&clientinfo, &len);
		printf("Verbindung von %s:%d\n", inet_ntoa(clientinfo.sin_addr), ntohs(clientinfo.sin_port));
		
		pthread_t child;
		//Thread mit Funktion mirror(rec_socket);
		if (pthread_create(&child, NULL, mirror, &rec_socket) != 0){ 
			perror("child is wrong");
			return -1;
		}
		else {
			pthread_join(child, NULL);
			printf("Abgekoppelt!\n"); // Fehlerfall: Abbruch
			pthread_detach(child);// abkoppeln vom Hauptprozess
		}
		
		//Verbindung zum Client getrennt
		close(rec_socket);
	}
	

	close(rec_socket);

	return 0;
}



