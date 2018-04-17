#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h> 

#include <time.h>
#include <string.h>

void csere(int csere[5], int meret);

int main(){
	//tömbök lefoglalása
	char** szimbolum = malloc(4*sizeof(char*));
	for(int i = 0; i < 4; i++){
		szimbolum[i] = malloc(10*sizeof(char));
	}

	strcpy(szimbolum[0], "\xE2\x99\xA0");
	strcpy(szimbolum[1], "\xE2\x99\xA3");
	strcpy(szimbolum[2], "\xE2\x99\xA5");
	strcpy(szimbolum[3], "\xE2\x99\xA6");

	char* spec = malloc(4*sizeof(char));
	spec[0] = 'J';
	spec[1] = 'Q';
	spec[2] = 'K';
	spec[3] = 'A';

	//socket létrehozása
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	//adress
	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(9002);
	server_adress.sin_addr.s_addr = INADDR_ANY;

	//connect
	int connection = connect(network_socket, (struct sockaddr *) &server_adress, sizeof(server_adress));
	if(connection != 0){
		printf("Valami elromlott..\n");
	}
	
	//fogadás
	char server_response[512];
	recv(network_socket, &server_response, sizeof(server_response), 0);

	//fogadott üzenet
	printf("A szerver uzenete: %s", server_response);

	char jatszmak[10];
	recv(network_socket, &jatszmak, sizeof(jatszmak), 0);
	printf("%d\n", (int)(jatszmak[0]-'a'));
//1. osztás
	for(int i = 0; i < 5; i++){
		char lap_ertek[10];
		recv(network_socket, &lap_ertek, sizeof(lap_ertek), 0);
		if((int)(lap_ertek[0]-'a') > 10){
			printf("%c ", spec[(int)(lap_ertek[0]-'a')-11]);
		}else{
			printf("%d ", (int)(lap_ertek[0]-'a'));
		}
		char lap_szin[10];
		recv(network_socket, &lap_szin, sizeof(lap_szin), 0);
		printf("%s\t", szimbolum[(int)(lap_szin[0]-'a')]);
	}
	printf("\n");
//csere
	char csere_uzenet[100];
	recv(network_socket, &csere_uzenet, sizeof(csere_uzenet), 0);


	printf("Szerver: %s", csere_uzenet);

	int cserek = -1;
	while(cserek < 0 || cserek > 5){
		scanf("%d", &cserek);
	}
	char cserek_ch[10];
	cserek_ch[0] = 'a'+cserek;
	send(network_socket, cserek_ch, sizeof(cserek_ch), 0);

	char go[10];
	recv(network_socket, &go, sizeof(go), 0);

	int csere_t[5];
	csere(csere_t, cserek);

	for(int i = 0; i < cserek; i++){
		char csere_index[10];
		csere_index[0] = 'a'+csere_t[i];
		send(network_socket, csere_index, sizeof(csere_index), 0);
	}

//2. osztás
	for(int i = 0; i < 5; i++){
		char lap_ertek[10];
		recv(network_socket, &lap_ertek, sizeof(lap_ertek), 0);
		if((int)(lap_ertek[0]-'a') > 10){
			printf("%c ", spec[(int)(lap_ertek[0]-'a')-11]);
		}else{
			printf("%d ", (int)(lap_ertek[0]-'a'));
		}
		char lap_szin[10];
		recv(network_socket, &lap_szin, sizeof(lap_szin), 0);
		printf("%s\t", szimbolum[(int)(lap_szin[0]-'a')]);
	}
	printf("\n");
//eredmény






	close(network_socket);

	//tömbök szabadítása
	for(int i = 0; i < 4; i++){
		free(szimbolum[i]);
	}
	free(szimbolum);
	free(spec);

	return 0;
}

void csere(int csere_t[5], int meret){

	for(int i = 0; i < meret; i++){
		int temp;
		int counter = 0;
		do{
			counter = 0;
			printf("%d. csere : ", (i+1));
			scanf("%d", &temp);

			if(temp < 1 || temp > 5){
				counter++;
			}
			for(int j = 0; j < i; j++){
				if(temp == csere_t[j]){
					counter++;
					break;
				}
			}
			csere_t[i] = temp;
		}while(counter != 0);
	}
}
