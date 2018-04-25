#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h> 

#include <string.h>

int main(int argc, char *argv[]){

	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	char server_ip[16];
   	sprintf(server_ip, "%s", argv[1]);

	int ip = inet_addr(server_ip);

	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(9002);
	server_adress.sin_addr.s_addr = ip;

	int connection;
	connection =  connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));
	if(connection != 0){
		printf("Valami elromlott..\n");
	}

	char uzenet[256];
	strcpy(uzenet, "sck ur mom");

	send(network_socket, uzenet, sizeof(uzenet), 0);


	return 0;
}
