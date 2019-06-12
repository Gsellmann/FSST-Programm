/* Einfacher Portscanner in C mit Threads
   Kompilieren mit: 
   gcc Hausaufgabe.c -o Hausaufgabe -Wall -lpthread -lwiringPi
*/
//GPIO|Pin|WiringPi
//  25| 22|  6
//  24| 18|  5
//  23| 16|  4
//  18| 12|  1
//
//nc localhost 4000
//F9 Compilieren
//echo graz | nc localhost 5000
//
#include <stdio.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <wiringPi.h>
//integer var = std::atoi (string zum umwandeln)   string -> integer
#define BUFSIZE 1000
#define PORT 4000
#define QUEUE 3
#define pin8 6
#define pin4 5
#define pin2 4
#define pin1 1




int  leuchten(int value){  //Leuchtet die Lampen im Dezimalwert von value
	if(wiringPiSetup() == -1)
			return NULL;
			
	pinMode(pin8, OUTPUT);
	pinMode(pin4, OUTPUT);
	pinMode(pin2, OUTPUT);
	pinMode(pin1, OUTPUT);
	
	if(value%2 == 1)
			digitalWrite(pin1,1);
		else
			digitalWrite(pin1,0);
		if(value%4 >= 2)
			digitalWrite(pin2,1);
		else
			digitalWrite(pin2,0);
		if(value%8 >= 4)
			digitalWrite(pin4,1);
		else
			digitalWrite(pin4,0);
		if(value%16 >= 8)
			digitalWrite(pin8,1);
		else
			digitalWrite(pin8,0);
		return NULL;
	}

void *mirror(void* arg)  // Funktion wenn mit wenn Client verbunden
{
		int clientfd = *(int *)arg;  
		char inbuffer[BUFSIZE];
		int value = 0;
		write(clientfd, "Hallo\n" , 7);  //write(wohin senden, was senden, wie viele Byte senden)
		while(1){
			
			read(clientfd, inbuffer, sizeof(inbuffer));  
			if(inbuffer[0] == 'q' || inbuffer[0] == 'Q')
				break;
			
			wiringPiSetup();
			
			value = atoi(inbuffer);
			//leuchten(value);
						
			if(value >=0 && value <= 15){
				leuchten(value);
				
			}

	}
		close(clientfd);
		return NULL;
		
}

int main()  // argv = Argumente die ich über die Konsole übergebe
{
    //Socket
    int server_socket, rec_socket;
    unsigned int len;
    struct sockaddr_in serverinfo, clientinfo;
  
    //Serversocket konfigurieren
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP
    serverinfo.sin_family = AF_INET; // IPv4
    //hoert auf allen Interfaces: 0.0.0.0 bzw. :::
    serverinfo.sin_addr.s_addr = htonl(INADDR_ANY);
    serverinfo.sin_port = htons(PORT);
    
    // Verbinde Socket mit IP-Adresse und Port
    if (bind(server_socket, (struct sockaddr *)&serverinfo, sizeof(serverinfo)) !=0){
		printf("Fehler Socket\n");
		return 1; //Rueckgabe

	}	
	listen(server_socket, QUEUE);  //Server wartet auf connect vom Client	
	//Endloschschleife zur Abarbeitung der Client-Anfragen
	while(1){
		
		printf("Server wartet...\n");
		//Verbindung vom Client eingetroffen
		rec_socket = accept(server_socket, (struct sockaddr *)&clientinfo, &len);
		printf("Verbindung von %s: %d\n", inet_ntoa(clientinfo.sin_addr), ntohs(clientinfo.sin_port));
		
		pthread_t child;  //Thread-Struktur
		//  Thread mit Funktion mirror(rec_socket) erzeugen
		if (pthread_create(&child, NULL, mirror, &rec_socket) != 0) {
			perror("child error");  //Fehlerfall: Abbruch
			return 1;
		}
		else {  // Kind erzeugt:
			printf("Abgekoppelt!\n");
			pthread_detach(child);  //abkoppeln vom Hauptprozessor
			
		}	

	}
	
    return 0;
}

