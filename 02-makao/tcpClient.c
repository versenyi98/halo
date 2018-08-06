#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> 

#include <string.h>

int main(int argc, char *argv[]){
//Szerver létrehozása
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
//játék
	
	for(;;){
		char ertek[32][15];				//kapott lapok értéke
		char szin[32][15];				//színe
		int meret = 0;					//játékos kezének mérete

		char kozepso_ertek[15];			//középső lap adatai
		char kozepso_szin[15];

		char ellenfel_meret_ch[15];		//ellenfél kezének mérete ~kódolva
		int ellenfel_meret;				//visszafejtve

		int break_ = 0;					//változó, arra az esetre ha vége lenne a meccsnek kibreakeljen
										//mindkét ciklusból

		for(;;){
		//lapok fogadása, ha vége a meccsnek akkor az értékbe fog érkezni az eredmény
			recv(network_socket, &ertek[meret], sizeof(ertek[meret]), 0);
			if(strcmp(ertek[meret], "Győztél") == 0 || strcmp(ertek[meret], "Vesztettél") == 0){
				printf("%s\n", ertek[meret]);
				break_ = 1;
				break;
			}
		//lap színének fogadása
			recv(network_socket, &szin[meret], sizeof(szin[meret]), 0);

		//ha az értékbe (és színbe is) megjön a "NULL" üzenet akkor kilép a ciklusból
		//lehetne úgy is mint amikor vége a meccsnek ... :/
			if(strcmp(ertek[meret], "NULL") == 0){
				break;
			}
			meret++;
		}
		if(break_){
			break;
		}
		
		//középső lap fogadása, ellenfél méretének fogadása
		recv(network_socket, &kozepso_ertek, sizeof(kozepso_ertek), 0);
		recv(network_socket, &kozepso_szin, sizeof(kozepso_szin), 0);
		recv(network_socket, &ellenfel_meret_ch, sizeof(ellenfel_meret_ch), 0);

		//visszafejtés, a 0. karakterből -'a' (int)re kasztolva, nem a legszebb megoldás
		ellenfel_meret = (int)(ellenfel_meret_ch[0]-'a');
		
		//kép törlése, adatok kiírása
		system("clear");
		printf("Középső lap: %s\t%s\n", kozepso_szin, kozepso_ertek);
		printf("Ellenfeled keze: %d\n", ellenfel_meret);

		printf("Kártyáid\n");
		for(int i = 0; i < meret; i++){
			printf("%d:\t%s\t%s\n", i+1, szin[i], ertek[i]);
		}

		printf("Mit lépsz? (lapot vagy kártya indexe)\n");
		
		//lépés beolvasása addig amíg jó nem lesz valamelyik esethez

		do{
			char lepes[15];
		//warningot dob erre a scanf-re de már nem mertem levenni az &-et
		//ezzel szívtam egy csomót és így épp megy 
			scanf("%s", lepes);

			if(strcmp(lepes, "feladom") == 0){
				printf("Feladtad\n");
				send(network_socket, lepes, sizeof(lepes), 0);
				close(network_socket);
				return 0;
			}
			if(strcmp(lepes, "lapot") == 0){
				printf("Lap kérése\n");
				send(network_socket, lepes, sizeof(lepes), 0);
				break;
			}
		//ha a lépés nem megy ki a méreten kívülre akkor ráellenőriz hogy a szín vagy érték stimmel-e
			if(atoi(lepes)-1 <= meret && strcmp(ertek[atoi(lepes)-1], kozepso_ertek) == 0 || strcmp(szin[atoi(lepes)-1], kozepso_szin) == 0){
				printf("Lap lerakás\n");
				send(network_socket, lepes, sizeof(lepes), 0);
				break;
			}
		}while(1);
	}		
		
	close(network_socket);
	return 0;
}


