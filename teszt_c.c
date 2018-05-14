#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> 

#include <string.h>

int main(int argc, char *argv[]){
//*****SZERVER****************************************************
	//socket létrehozása
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	char server_ip[16];
   	sprintf(server_ip, "%s", argv[1]);

	int ip = inet_addr(server_ip);

	//adress
	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(9002);
	server_adress.sin_addr.s_addr = ip;

	//connect
	int connection = connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));
	if(connection != 0){
		printf("%d", connection);
		printf("Valami elromlott..\n");
		exit(-1);
	}
//*****p2p******************************************************
	char mesg_[50];
	recv(network_socket, &mesg_, sizeof(mesg_), 0); 

	if(strcmp(mesg_, "szerver") == 0){
		int server_socket;
		server_socket = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in server_adress;
		server_adress.sin_family = AF_INET;
		server_adress.sin_port = htons(9003);
		server_adress.sin_addr.s_addr = INADDR_ANY;

		bind(server_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));	

		listen(server_socket, 5); 

		int socket1 = accept(server_socket, NULL, NULL);
		char mesg_asd[15];
		strcpy(mesg_asd, "Siker");
		send(socket1, mesg_asd, sizeof(mesg_asd), 0);
		close(server_socket);
	} else{
		int network_socket1;
		network_socket1 = socket(AF_INET, SOCK_STREAM, 0);

		int ip1 = inet_addr(mesg_);

		//adress
		struct sockaddr_in server_adress1;
		server_adress1.sin_family = AF_INET;
		server_adress1.sin_port = htons(9003);
		server_adress1.sin_addr.s_addr = ip1;

		//connect
		connect(network_socket1, (struct sockaddr *) &server_adress1, sizeof(server_adress1));
		char mesg_asd[15];
		recv(network_socket1, &mesg_asd, sizeof(mesg_asd), 0);
		printf("%s", mesg_asd);
		close(network_socket1);
	}


	close(network_socket);
	return 0;
}


