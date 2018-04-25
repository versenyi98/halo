#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h> 

int main(){

	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	//adress
	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(9002);
	server_adress.sin_addr.s_addr = INADDR_ANY;

	//bind
	bind(server_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));	
	
	listen(server_socket, 5); 

	int client1 = accept(server_socket, NULL, NULL);

	char uzenet[256];
	recv(client1, &uzenet, sizeof(uzenet), 0);
	printf("%s\n", uzenet);

	return 0;
}
