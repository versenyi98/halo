#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h> 

#include <time.h>
#include <string.h>

void szerver_csere(int socket);
void szerver_osztas(int socket, char** szimbolum, char* special);

int main(){
//tömbök lefoglalása
	//francia kártya szimbólumok
	char** szimbolum = malloc(4*sizeof(char*));
	for(int i = 0; i < 4; i++){
		szimbolum[i] = malloc(10*sizeof(char));
	}

	strcpy(szimbolum[0], "\xE2\x99\xA0");
	strcpy(szimbolum[1], "\xE2\x99\xA3");
	strcpy(szimbolum[2], "\xE2\x99\xA5");
	strcpy(szimbolum[3], "\xE2\x99\xA6");

	//speciális kártyák értékei
	char* spec = malloc(4*sizeof(char));
	spec[0] = 'J';
	spec[1] = 'Q';
	spec[2] = 'K';
	spec[3] = 'A';

	//combok nevei
	char** combo_nev = malloc(10*sizeof(char*));
	for(int i = 0; i < 10; i++){
		combo_nev[i] = malloc(10*sizeof(char));
	}

	strcpy(combo_nev[0], "Magas lap");
	strcpy(combo_nev[1], "Par");
	strcpy(combo_nev[2], "Ket par");
	strcpy(combo_nev[3], "Drill");
	strcpy(combo_nev[4], "Sor");
	strcpy(combo_nev[5], "Flush (szin)");
	strcpy(combo_nev[6], "Full House");
	strcpy(combo_nev[7], "Asz");
	strcpy(combo_nev[8], "Sor flush");
	strcpy(combo_nev[9], "Royal flush");

//*****SZERVER****************************************************
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
//*****JÁTÉK******************************************************
//Jaték előtti beszélgetés
	char msg_udv[256];
	recv(network_socket, &msg_udv, sizeof(msg_udv), 0);
	printf("Szerver: %s", msg_udv);

	char msg_meccsek[2];
	recv(network_socket, &msg_meccsek, sizeof(msg_meccsek), 0);

	int jatszma = (int)(msg_meccsek[0]-'a');
	printf("%d\n", jatszma);

	char msg_nev[32];
	recv(network_socket, &msg_nev, sizeof(msg_nev), 0);
	printf("Szerver: %s", msg_nev);

	char msg_nev_valasz[30];
	scanf("%s", msg_nev_valasz);
	send(network_socket, msg_nev_valasz, sizeof(msg_nev_valasz), 0);
for(int i = 0; i < jatszma; i++){
//1. osztás
	szerver_osztas(network_socket, szimbolum, spec);
//csere
	szerver_csere(network_socket);
//2. osztás
	szerver_osztas(network_socket, szimbolum, spec);
	szerver_osztas(network_socket, szimbolum, spec);

	char msg_next_round[50];
	recv(network_socket, &msg_next_round, sizeof(msg_next_round), 0);
	printf("Szerver: %s\n", msg_next_round);

	char msg_nextr_resp[10];
	fgets(msg_nextr_resp, 10, stdin);
	fgets(msg_nextr_resp, 10, stdin);
  msg_nextr_resp[strlen(msg_nextr_resp)-1] = '\0';
	send(network_socket, msg_nextr_resp, sizeof(msg_nextr_resp), 0);


	system("clear");
}
	close(network_socket);

	//tömbök szabadítása
	for(int i = 0; i < 4; i++){
		free(szimbolum[i]);
	}
	for(int i = 0; i < 10; i++){
		free(combo_nev[i]);
	}
	free(combo_nev);
	free(szimbolum);
	free(spec);

	return 0;
}
void szerver_osztas(int socket, char** szimbolum, char* special){

	char msg_nev_osztas[30];
	recv(socket, msg_nev_osztas, sizeof(msg_nev_osztas), 0);

	printf("%s\t", msg_nev_osztas);

	for(int i = 0; i < 5; i++){
		char lap_ertek[2];
		char lap_szin[2];

		recv(socket, &lap_ertek, sizeof(lap_ertek), 0);				//lap értékének fogadása
		recv(socket, &lap_szin, sizeof(lap_szin), 0);					//lap színének fogadása

		if((int)(lap_ertek[0]-'a') > 10){
			printf("%c ", special[(int)(lap_ertek[0]-'a')-11]);		//speciális lap vagy sima
		}else{
			printf("%d ", (int)(lap_ertek[0]-'a'));
		}
		printf("%s\t", szimbolum[(int)(lap_szin[0]-'a')]);		//szimbólum kiolvasása a szimbólum tömbből
	}
	printf("\n");

}

void szerver_csere(int socket){

	char msg_csere[100];
	recv(socket, &msg_csere, sizeof(msg_csere), 0);

	printf("Szerver: %s", msg_csere);

	int cserek_szama = -1;
	while(cserek_szama < 0 || cserek_szama > 5){
		scanf("%d", &cserek_szama);
	}
	char msg_csere_valasz[2];
	msg_csere_valasz[0] = 'a'+cserek_szama;
	send(socket, msg_csere_valasz, sizeof(msg_csere_valasz), 0);

	int cserek[5];
	for(int i = 0; i < cserek_szama; i++){
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
				if(temp == cserek[j]){
					counter++;
					break;
				}
			}
			cserek[i] = temp;
		}while(counter != 0);
	}

	for(int i = 0; i < cserek_szama; i++){
		char cserek_ch[2];
		cserek_ch[0] = 'a'+cserek[i];
		send(socket, cserek_ch, sizeof(cserek_ch), 0);
	}
}


