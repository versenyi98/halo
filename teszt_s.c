#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <unistd.h> 

int main(){
//*****SZERVER****************************************************
	//szerver socket létrehozása
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in socket_adress1;
	struct sockaddr_in socket_adress2;
	socklen_t addr_size;
	//adress
	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(9002);
	server_adress.sin_addr.s_addr = INADDR_ANY;

	//bind
	int binding = bind(server_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));	
	if(binding != 0){
		printf("Nem sikerult a porthoz csatlakozni!\n");
		exit(-1);
	}	

	listen(server_socket, 5); 
//*****p2p******************************************************

//elfogadás
	int socket1 = accept(server_socket, (struct sockaddr *) &socket_adress1, &addr_size);
	int socket2 = accept(server_socket, (struct sockaddr *) &socket_adress2, &addr_size);

	char mesg_1[50];
	char mesg_2[50];

	strcpy(mesg_1, inet_ntoa(socket_adress1.sin_addr));
	strcpy(mesg_2, "szerver");



	close(server_socket);
	return 0;
}

