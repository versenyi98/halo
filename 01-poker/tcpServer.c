#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h> 

#include <time.h>
#include <string.h>

typedef struct kartya{

	int azon;
	int szin;
	int ertek;
} KARTYA;

typedef struct combo{

	int tipus;
	int ertek;
	struct combo* kov;

} COMBO;

typedef struct szemely{
	
	KARTYA hand[5];
	COMBO *combo;

	char nev[30];
	int cserek_szama;
	int cserek[5];
	int gyozelmek;
	int socket;

} SZEMELY;

void kever(KARTYA kartyak[], int meret);
void osztas(KARTYA kartyak[], KARTYA hand[], int *index);
void rendez(KARTYA hand[]);

COMBO* royal_flush(COMBO* gyok, KARTYA hand[]);
COMBO* straight_flush(COMBO* gyok, KARTYA hand[]);
COMBO* four_of_a_kind(COMBO* gyok, KARTYA hand[]);
COMBO* full_house(COMBO* gyok, KARTYA hand[]);
COMBO* flush(COMBO* gyok, KARTYA hand[]);
COMBO* straight(COMBO* gyok, KARTYA hand[]);
COMBO* three_of_a_kind(COMBO* gyok, KARTYA hand[]);
COMBO* two_pair(COMBO* gyok, KARTYA hand[]);
COMBO* pair(COMBO* gyok, KARTYA hand[]);
COMBO* high_card(COMBO* gyok, KARTYA hand[], int num);

int winner(COMBO* gyok1, COMBO* gyok2);
void szabadit(COMBO* gyok);

void kliens_osztas(SZEMELY *kitol, SZEMELY *kinek);
void kliens_csere(SZEMELY *player, int *index, KARTYA pakli[]);

void combo_ir(COMBO* gyok);

int main(){

	int jatszma;
	printf("Add meg mennyit jatszma lesz: ");
	scanf("%d", &jatszma);

//*****SZERVER****************************************************
	//szerver socket létrehozása
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

//*****JÁTÉK******************************************************
	srand(time(NULL));
	KARTYA kartyak[52];

	SZEMELY* player1 = malloc(sizeof(SZEMELY));
	SZEMELY* player2 = malloc(sizeof(SZEMELY));

	int pakli_i = 0;



	player1->socket = accept(server_socket, NULL, NULL);
	player2->socket = accept(server_socket, NULL, NULL);
//Jaték előtti beszélgetés
	char msg_udv[256] = "A kapcsolat sikeresen letrejott a szerverrel.\nA jatszmak szama: ";
	send(player1->socket, msg_udv, sizeof(msg_udv), 0);
	send(player2->socket, msg_udv, sizeof(msg_udv), 0);

	char msg_meccsek[2];
	msg_meccsek[0] = 'a'+jatszma;

	send(player1->socket, msg_meccsek, sizeof(msg_meccsek), 0);
	send(player2->socket, msg_meccsek, sizeof(msg_meccsek), 0);

	char msg_nev[32] = "Mondd meg mi a neved: ";
 	send(player1->socket, msg_nev, sizeof(msg_nev), 0);
	recv(player1->socket, player1->nev, sizeof(player1->nev), 0);
 	send(player2->socket, msg_nev, sizeof(msg_nev), 0);
	recv(player2->socket, player2->nev, sizeof(player2->nev), 0);

for(int i = 0; i < jatszma; i++){
	kever(kartyak, 52);

	osztas(kartyak, player1->hand, &pakli_i);
	osztas(kartyak, player2->hand, &pakli_i);
	rendez(player1->hand);
	rendez(player2->hand);

//1. osztás
	kliens_osztas(player1, player1);
	kliens_osztas(player2, player2);
//csere
	kliens_csere(player1, &pakli_i, kartyak);
	kliens_csere(player2, &pakli_i, kartyak);
//2. osztás
	kliens_osztas(player1, player1);
	kliens_osztas(player2, player2);

	kliens_osztas(player1, player2);
	kliens_osztas(player2, player1);
//eredmény
	player1->combo = 0;
	player1->combo = royal_flush(player1->combo, player1->hand);
	combo_ir(player1->combo);

	printf("\n");

	player2->combo = 0;
	player2->combo = royal_flush(player2->combo, player2->hand);
	combo_ir(player2->combo);

	printf("\nA nyertes: %d\n", winner(player1->combo, player2->combo));

	szabadit(player1->combo);
	szabadit(player2->combo);

	char msg_next_round[50] = "Nyomj egy entert a kovetkezo korbe lepeshez...";
	char msg_nextr_resp1[10];
	char msg_nextr_resp2[10];

 	send(player1->socket, msg_next_round, sizeof(msg_next_round), 0);
	recv(player1->socket, &msg_nextr_resp1, sizeof(msg_nextr_resp1), 0);
 	send(player2->socket, msg_next_round, sizeof(msg_next_round), 0);
	recv(player2->socket, &msg_nextr_resp2, sizeof(msg_nextr_resp2), 0);




}
	close(server_socket);

	free(player1);
	free(player2);

	return 0;
}

void kliens_osztas(SZEMELY* kitol, SZEMELY* kinek){

	char msg_nev_osztas[30];
	strcpy(msg_nev_osztas, kitol->nev);
	send(kinek->socket, msg_nev_osztas, sizeof(msg_nev_osztas), 0);


	for(int i = 0; i < 5; i++){
		char lap_ertek[2];
		char lap_szin[2];

		lap_ertek[0] = 'a'+kitol->hand[i].ertek;						//lap értéke betűként úgy hogy az 'a' = 0
		lap_ertek[1] = '\0';

		lap_szin[0] = 'a'+kitol->hand[i].szin;							//lap színe betűként úgy hogy az 'a' = 0. szín
		lap_szin[1] = '\0';

		printf("%d ", kitol->hand[i].ertek);									//lap értéke a szerverről nézve
		send(kinek->socket, lap_ertek, sizeof(lap_ertek), 0);	//lap elküldése a kliensnek

		printf("%d \t", kitol->hand[i].szin);									//lap színe a szerverről nézve
		send(kinek->socket, lap_szin, sizeof(lap_szin), 0);		//lap elküldése a kliensnek
	}
	printf("\n");
}

void kliens_csere(SZEMELY* player, int *index, KARTYA pakli[]){
	char msg_csere[100] = "Mennyit kivansz cserelni? ";
	send(player->socket, msg_csere, sizeof(msg_csere), 0);

	char msg_csere_valasz[2];
	recv(player->socket, &msg_csere_valasz, sizeof(msg_csere_valasz), 0);

	player->cserek_szama = (int)(msg_csere_valasz[0]-'a');
	printf("%s csereinek szama: %d\n", player->nev, player->cserek_szama);

	for(int i = 0; i < player->cserek_szama; i++){
		char cserek_ch[2];
		recv(player->socket, &cserek_ch, sizeof(cserek_ch), 0);
		player->cserek[i] = (int)(cserek_ch[0]-'a');
	}
	for(int i = 0; i < player->cserek_szama; i++){
		player->hand[player->cserek[i]-1] = pakli[*index];
		*index = *index+1;
	}

	rendez(player->hand);
}

void kever(KARTYA kartyak[], int meret){

	int counter = 0;

	for(int i = 0; i < meret; i++){
		KARTYA* uj = (KARTYA *)malloc(sizeof(KARTYA));
		do{
			counter = 0;
			uj->azon = rand()%meret;
			for(int j = 0; j < i; j++){
				if(uj->azon == kartyak[j].azon){
					counter++;
					break;
				}
			}
		}while(counter != 0);
		
		uj->szin = uj->azon/13;
		uj->ertek = (uj->azon)%13+2;

		kartyak[i] = *uj;

		free(uj);
	}
}

void osztas(KARTYA kartyak[], KARTYA hand[], int *index){

	for(int i = 0; i < 5; i++){
		hand[i] = kartyak[*index];
		*index = *index + 1;
	}

}

void rendez(KARTYA hand[]){

	for(int i = 0; i < 5; i++){
		int kulcs = i;
		for(int j = i; j < 5; j++){
			if(hand[kulcs].ertek > hand[j].ertek){
				kulcs = j;
			}	
		}
		KARTYA temp = hand[kulcs];
		hand[kulcs] = hand[i];
		hand[i] = temp;
	}
}

COMBO* royal_flush(COMBO* gyok, KARTYA hand[]){

	int szin = hand[0].szin;
	int elozo = hand[0].ertek;

	int szin_egyez = 0;
	int eggyel_nagyobb = 0;

	for(int i = 1; i < 5; i++){
		if(szin == hand[i].szin){
			szin_egyez++;
		}
		if(elozo + 1 == hand[i].ertek){
			eggyel_nagyobb++;
			elozo = hand[i].ertek;
		}
	}

	if(eggyel_nagyobb == 4 && szin_egyez == 4 && elozo == 14){
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 9;
		gyok->ertek = 0;
		gyok->kov = 0;
	} else{
		gyok = straight_flush(gyok, hand);
	}
	
	return gyok;
}

COMBO* straight_flush(COMBO* gyok, KARTYA hand[]){

	int szin = hand[0].szin;
	int elozo = hand[0].ertek;

	int szin_egyez = 0;
	int eggyel_nagyobb = 0;

	for(int i = 1; i < 5; i++){
		if(szin == hand[i].szin){
			szin_egyez++;
		}
		if(elozo + 1 == hand[i].ertek){
			eggyel_nagyobb++;
			elozo = hand[i].ertek;
		}
	}

	if(hand[3].ertek == 5 && hand[4].ertek == 14){
		eggyel_nagyobb++;
	}

	if(eggyel_nagyobb == 4 && szin_egyez == 4){
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 8;
		gyok->ertek = elozo;
		gyok->kov = 0;	
	} else{
		gyok = four_of_a_kind(gyok, hand);
	}

	return gyok;
}

COMBO* four_of_a_kind(COMBO* gyok, KARTYA hand[]){

	for(int i = 0; i < 2; i++){
		int talalt = 0;
		for(int j = i; j < 5; j++){
			if(hand[j].ertek == hand[i].ertek){
				talalt++;
			}
		}
		if(talalt == 4){
			gyok = malloc(sizeof(COMBO));
			gyok->tipus = 7;
			gyok->ertek = hand[i].ertek;
			gyok->kov = 0;
		}
	}
	if(gyok == 0){
		gyok = full_house(gyok, hand);
	}

	return gyok;
}

COMBO* full_house(COMBO* gyok, KARTYA hand[]){

	int egyezes = 0;

	for(int i = 0; i < 5; i++){
		for(int j = i; j < 5; j++){
			if(hand[i].ertek == hand[j].ertek){
				egyezes++;
			}
		}	
	}
	if(egyezes == 9){
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 6;
		gyok->ertek = 0;
		gyok->kov = three_of_a_kind(gyok->kov, hand);

	} else{
		gyok = flush(gyok, hand);
	}


	return gyok;
}

COMBO* flush(COMBO* gyok, KARTYA hand[]){

	int egyezes = 0;
	
	for(int i = 0; i < 5; i++){
		if(hand[i].szin == hand[0].szin){
			egyezes++;	
		}
	}
	if(egyezes == 5){
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 5;
		gyok->ertek = 0;
		gyok->kov = high_card(gyok->kov, hand, 4);
	} else{
		gyok = straight(gyok, hand);	
	}
	
	return gyok;
}

COMBO* straight(COMBO* gyok, KARTYA hand[]){

	int counter = 0;

	for(int i = 0; i < 4; i++){
		if(hand[i+1].ertek-hand[i].ertek == 1){
			counter++;
		}
	}
	if(hand[3].ertek == 5 && hand[4].ertek == 14){
		counter++;
	}
	if(counter == 4){
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 4;
		gyok->ertek = hand[4].ertek;
		gyok->kov = 0;
	} else{
		gyok = three_of_a_kind(gyok, hand);
	}
	return gyok;
}

COMBO* three_of_a_kind(COMBO* gyok, KARTYA hand[]){

	for(int i = 0; i < 3; i++){
		if(hand[i].ertek == hand[i+1].ertek && hand[i+1].ertek == hand[i+2].ertek){
			gyok = malloc(sizeof(COMBO));
			gyok->tipus = 3;

			gyok->ertek = hand[i].ertek;
			gyok->kov = 0;	
			break;
		}
	} 
	if(gyok == 0){
		gyok = two_pair(gyok, hand);
	}

	return gyok;
}

COMBO* two_pair(COMBO* gyok, KARTYA hand[]){

	int nagy = 0;
	int kicsi = 0;
	int counter = 0;

	for(int i = 0; i < 4; i++){
		if(hand[i].ertek == hand[i+1].ertek){
			counter++;
			if(hand[i].ertek > nagy){
				kicsi = nagy;
				nagy = hand[i].ertek;
			}
		}
	}
	if(counter == 2){
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 2;
		gyok->ertek = 0;
		gyok->kov = malloc(sizeof(COMBO));
		gyok->kov->tipus = 1;
		gyok->kov->ertek = nagy;
		gyok->kov->kov = malloc(sizeof(COMBO));
		gyok->kov->kov->tipus = 1;
		gyok->kov->kov->ertek = kicsi;
		gyok->kov->kov->kov = high_card(gyok->kov->kov->kov, hand, 4);
	} else{
		gyok = pair(gyok, hand);

	}

	return gyok;
}

COMBO* pair(COMBO* gyok, KARTYA hand[]){

	for(int i = 0; i < 4; i++){
		if(hand[i].ertek == hand[i+1].ertek){
			if(i < 3 && hand[i+2].ertek == hand[i].ertek){
				i = i+2;
			} else{
				gyok = malloc(sizeof(COMBO));
				gyok->tipus = 1;
				gyok->ertek = hand[i].ertek;
				gyok->kov = high_card(gyok, hand, 4);			//magas lap
				break;
			}
		}
	}
	if(gyok == 0){
		gyok = high_card(gyok, hand, 4);
	}
	
	return gyok;
}

COMBO* high_card(COMBO* gyok, KARTYA hand[], int num){

	if(num < 0){
		return 0;
	} else{
		gyok = malloc(sizeof(COMBO));
		gyok->tipus = 0;
		gyok->ertek = hand[num].ertek; 
		gyok->kov = high_card(gyok->kov, hand, num-1);
		return gyok;
	}
	
}

int winner(COMBO* gyok1, COMBO* gyok2){

	if(gyok1 == 0 && gyok2 == 0){
		return -1;
	} else{
		if(gyok2 == 0 || gyok1->tipus > gyok2->tipus || (gyok1->tipus == gyok2->tipus && gyok1->ertek > gyok2->ertek)){
			return 0;
		} else if(gyok1 == 0 || gyok1->tipus < gyok2->tipus || (gyok1->tipus == gyok2->tipus && gyok1->ertek < gyok2->ertek)){
			return 1;
		} else{
			winner(gyok1->kov, gyok2->kov);
		}
	}
}

void szabadit(COMBO* gyok){

	if(gyok != NULL){
		szabadit(gyok->kov);
		free(gyok);
	}

}

void combo_ir(COMBO* gyok){

	if(gyok != 0){
		printf("E: %d \t T: %d \n",gyok->ertek, gyok->tipus);
		combo_ir(gyok->kov);
	}
}


