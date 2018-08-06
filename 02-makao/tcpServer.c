#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h> 

#include <string.h>
#include <time.h>

typedef struct kartya{

	char szin[15];
	char ertek[15];
	char allapot[15];

} KARTYA;

typedef struct player{

	char nev[15];
	int socket;
	KARTYA hand[32];
	int hand_meret;

} PLAYER;

void kartya_generalas(KARTYA pakli[32]);
void lap_huzas(PLAYER* player, KARTYA pakli[32], KARTYA kozepso);
void adat_kuldes(PLAYER player, PLAYER ellenfel, KARTYA kozepso);
int adat_fogadas(PLAYER* player, KARTYA pakli[32], KARTYA* kozepso, int* huzas);
void vege(PLAYER player);

int main(){
//Szerver létrehozása
	//szerver socket létrehozása
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

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

	listen(server_socket, 4); 

//Jaték
//játék előtti móka
	KARTYA pakli[32];

	KARTYA kozepso;

	PLAYER player1;
	PLAYER player2;

	player1.socket = accept(server_socket, NULL, NULL);	//1. kliens csatlakozása
	player2.socket = accept(server_socket, NULL, NULL);	//2. kliens csatlakozása

	printf("Kliensek csatlakoztatva\n");

	strcpy(player1.nev, "1.játékos");					//nevek beállítása
	strcpy(player2.nev, "2.játékos");

	player1.hand_meret = 0;								//kéz méretek beállítása
	player2.hand_meret = 0;

	printf("Nevek megadva\n");

	kartya_generalas(pakli);							//lapok legenerálása sorban

	printf("Generalas kesz\n");

	int koz_num = rand()%32;							//középső lap kihúzása
	strcpy(pakli[koz_num].allapot, "Középső");
	kozepso = pakli[koz_num];

	for(int i = 0; i < 5; i++){							//első 5 lap húzása mindkét játékosnak
		lap_huzas(&player1, pakli, kozepso);
		lap_huzas(&player2, pakli, kozepso);
	}
	
	printf("Osztas kesz\n");

//játékmenet

	//ez a változó mutatja majd meg hogy hányat kell húzni ha hetesre nem hetessel válaszol
	int huzas = 0;

	for(;;){

	//ha a második játékosnak 0 kártyája van akkor break-el azelőtt hogy küldene valamit az elsőnek
		if(player2.hand_meret == 0){
			break;
		}
	//középső lap, ellenfél kezének mérete, saját lapok kiküldése
		adat_kuldes(player1, player2, kozepso);
	//válasz fogadása, ha -1 akkor feladta
		int fogad1 = adat_fogadas(&player1, pakli, &kozepso, &huzas);
		if(fogad1 == -1){
			close(server_socket);
			return 0;
		}
	//ugyanezek a 2. játékosnál
 		if(player1.hand_meret == 0){
			break;
		}

		adat_kuldes(player2, player1, kozepso);

		int fogad2 = adat_fogadas(&player2, pakli, &kozepso, &huzas);
		if(fogad2 == -1){
			close(server_socket);
			return 0;
		}
	//szerverre a pakli kiírása, lapok állapotával együtt
		for(int i = 0; i < 32; i++){
			printf("%s\t%s\t%s\n", pakli[i].szin, pakli[i].ertek, pakli[i].allapot);
		}
		printf("---------------------------------------\n");
	}

	vege(player1);
	vege(player2);

	return 0;
}

void kartya_generalas(KARTYA pakli[32]){

	//0-tól 31-ig generál számot, 4 féle szín 8 féle érték -> 32 kártya

	char szinek[4][15];
	char ertekek[8][15];

	strcpy(szinek[0], "Piros");
	strcpy(szinek[1], "Tök");
	strcpy(szinek[2], "Zöld");
	strcpy(szinek[3], "Makk");

	strcpy(ertekek[0], "Hét");
	strcpy(ertekek[1], "Nyolc");
	strcpy(ertekek[2], "Kilenc");
	strcpy(ertekek[3], "Tíz");
	strcpy(ertekek[4], "Alsó");
	strcpy(ertekek[5], "Felső");
	strcpy(ertekek[6], "Király");
	strcpy(ertekek[7], "Ász");

	for(int i = 0; i < 32; i++){
	//Az első 8 kártya piros lesz, majd tök ...
	//Az értékek sorban nőnek héttől ászig
		strcpy(pakli[i].szin, szinek[i/8]);
		strcpy(pakli[i].ertek, ertekek[i%8]);
	//Minden kártya először a pakliban lesz -> "Pakli" állapot
		strcpy(pakli[i].allapot, "Pakli");
	}
}

void lap_huzas(PLAYER* player, KARTYA pakli[32], KARTYA kozepso){

	srand(time(NULL));

	int i;

	do{
	//az i 0 és 31 között random szám ...
		i = rand()%32;
	//... addig amíg talál olyan index nem lesz amin "Pakli állapotú lap lesz
	} while(strcmp(pakli[i].allapot, "Pakli") != 0);

	//állapotba átmásolja a player nevét
	strcpy(pakli[i].allapot, player->nev);
	//player kezébe adja
	player->hand[player->hand_meret] = pakli[i];
	//player kezének mérete nő
	player->hand_meret++;
	
	int pakliban = 0;
	
	//ellenőrzi hogy van-e Pakli állapotú kártya
	for(int i = 0; i < 32; i++){
		if(strcmp(pakli[i].allapot, "Pakli") == 0){
			pakliban++;
			break;
		}
	}
	//ha nincs, akkor a "Középső" állapotú lapokat berakja a
	//pakliba újra, de csak akkor ha nem az aktuális középsőről van szó
	if(pakliban == 0){
		for(int i = 0; i < 32; i++){
			if(strcmp(pakli[i].allapot, "Középső") == 0 && (strcmp(pakli[i].ertek, kozepso.ertek) != 0 || strcmp(pakli[i].szin, kozepso.szin) != 0)){
				strcpy(pakli[i].allapot, "Pakli");
			}
		}
	}
}

void adat_kuldes(PLAYER player, PLAYER ellenfel, KARTYA kozepso){

	// NULL üzenet, akkor kell ha már elküldük az összes lapot
	// azért kell mert a másik oldalon végtelen ciklussal fogad, mivel nem tudja hogy mennyit fogadjon
	char null_mesg[15];
	strcpy(null_mesg, "NULL");

	//kiküldi a playernek a lapjait
	for(int i = 0; i < player.hand_meret; i++){
		send(player.socket, player.hand[i].ertek, sizeof(player.hand[i].ertek), 0);
		send(player.socket, player.hand[i].szin, sizeof(player.hand[i].szin), 0);
	}
	//ha a lapokat kiküldte, elküldi a null üzenetet, ott megszakad a lapok olvasása
	send(player.socket, null_mesg, sizeof(null_mesg), 0);
	send(player.socket, null_mesg, sizeof(null_mesg), 0);

	//elküldi a középső lap értékét, színét
	send(player.socket, kozepso.ertek, sizeof(kozepso.ertek), 0);
	send(player.socket, kozepso.szin, sizeof(kozepso.szin), 0);

	//az ellenfél méretét egy 15 hossz tömb első karakterében tárolja úgy hogy
	// az 'a' betűt elcsúsztatja a méret értékével
	// a túloldalon úgy kapod vissza hogy elcsúsztatott érték - 'a' = méret 
	// lehetne szebben is ...
	char ellenfel_meret[15];
	ellenfel_meret[0] = ellenfel.hand_meret + 'a';

	//elküldi a tömböt
	send(player.socket, ellenfel_meret, sizeof(ellenfel_meret), 0);

}

int adat_fogadas(PLAYER* player, KARTYA pakli[32], KARTYA* kozepso, int* huzas){

	//megkapja mit lépett a player
	char lepes[15];
	recv(player->socket, &lepes, sizeof(lepes), 0);

	//ha feladta akkor ezt -1-gyel jelzi a visszatérésnél
	if(strcmp(lepes, "feladom") == 0){
		return -1;
	} else if(strcmp(lepes, "lapot") == 0){
	//ha lapot kér, akkor húzunk neki egyet, és ha a huzas valtozo nem 0,
	//akkor húzunk még annyit amennyit kell a hetesek miat
		lap_huzas(player, pakli, *kozepso);
		if(*huzas){
			for(int i = 0; i < *huzas; i++){
				lap_huzas(player, pakli, *kozepso);
			}
			*huzas = 0;
		}
	} else{
	//ebben az esetben lapot rakott le, ha nem hetest akkor húzunk annyit amennyi a huzas
		if(*huzas && strcmp(player->hand[atoi(lepes)-1].ertek, "Hét") != 0){
			for(int i = 0; i < *huzas; i++){
				lap_huzas(player, pakli, *kozepso);
			}
			*huzas = 0;
	//ha hetes akkor a huzas 2-vel nő
		} else if(strcmp(player->hand[atoi(lepes)-1].ertek, "Hét") == 0){
			*huzas = *huzas + 2;
		}

	for(int i = 0; i < 32; i++){
	//megkeressük a pakliban azt a lapot amit a játékos le akar rakni,
	//középső állapotuvá teszi
		if(strcmp(player->hand[atoi(lepes)-1].ertek, pakli[i].ertek) == 0){
			if(strcmp(player->hand[atoi(lepes)-1].szin, pakli[i].szin) == 0){
					strcpy(pakli[i].allapot, "Középső");
				}
			}
		}
	//az aktuális középső ez a lap lesz
		*kozepso = player->hand[atoi(lepes)-1];
	//majd a lerakott laptól kezdve minden eggyel előrébb csúsztat
	//kéz méretet csökkenti eggyel
		for(int i = atoi(lepes)-1; i < player->hand_meret-1; i++){
			player->hand[i] = player->hand[i+1];
		}
		player->hand_meret--;
	}
	return 0;
}

void vege(PLAYER player){

	char uzenet[15];

	//ha nem 0 a kéz méret akkor vesztített a játékos, egyébként nyert

	if(player.hand_meret){
		strcpy(uzenet, "Vesztettél");
	} else{
		strcpy(uzenet, "Győztél");
	}
	
	send(player.socket, uzenet, sizeof(uzenet), 0);

}




