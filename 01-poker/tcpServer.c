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

void kever(KARTYA kartyak[], int meret);
void osztas(KARTYA kartyak[], KARTYA hand[], int *index);
void csere(KARTYA kartyak[], KARTYA hand[], int *index);
void rendez(KARTYA hand[]);
void kiir(KARTYA hand[], char** szimbolum, char* spec);

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

void winner(COMBO* gyok1, COMBO* gyok2);
void szabadit(COMBO* gyok);
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
	
	int client_socket1;
	int client_socket2;

	client_socket1 = accept(server_socket, NULL, NULL);
	client_socket2 = accept(server_socket, NULL, NULL);
//*****JÁTÉK******************************************************
	
	srand(time(NULL));
	KARTYA kartyak[54];

	KARTYA hand1[5];
	COMBO* gyoker1 = 0;

	KARTYA hand2[5];
	COMBO* gyoker2 = 0;

	int pakli_i = 0;
	char character;

	pakli_i = 0;
	kever(kartyak, 52);

	osztas(kartyak, hand1, &pakli_i);
	rendez(hand1);
	osztas(kartyak, hand2, &pakli_i);
	rendez(hand2);

	char udvozlo[512] = "A kapcsolat sikeresen letrejott a szerverrel.\nA jatszmak szama: ";
	send(client_socket1, udvozlo, sizeof(udvozlo), 0);
	send(client_socket2, udvozlo, sizeof(udvozlo), 0);

	//jatszmak szama
	char uzenet[10];
	uzenet[0] = 'a'+jatszma;

	send(client_socket1, uzenet, sizeof(uzenet), 0);
	send(client_socket2, uzenet, sizeof(uzenet), 0);
//1. osztás
	for(int i = 0; i < 5; i++){
		char lap_ertek[10];
		lap_ertek[0] = 'a'+hand1[i].ertek;
		lap_ertek[1] = '\0';
		printf("%d ", hand1[i].ertek);
		send(client_socket1, lap_ertek, sizeof(lap_ertek), 0);

		char lap_szin[10];
		lap_szin[0] = 'a'+hand1[i].szin;
		lap_szin[1] = '\0';
		printf("%d \t", hand1[i].szin);
		send(client_socket1, lap_szin, sizeof(lap_szin), 0);
	}
	printf("\n");
	for(int i = 0; i < 5; i++){
		char lap_ertek[10];
		lap_ertek[0] = 'a'+hand2[i].ertek;
		lap_ertek[1] = '\0';
		printf("%d ", hand2[i].ertek);
		send(client_socket2, lap_ertek, sizeof(lap_ertek), 0);

		char lap_szin[10];
		lap_szin[0] = 'a'+hand2[i].szin;
		lap_szin[1] = '\0';
		printf("%d \t", hand2[i].szin);
		send(client_socket2, lap_szin, sizeof(lap_szin), 0);
	}
	printf("\n");
//csere
	char csere_uzenet[100] = "Mennyit kivansz cserelni? ";
	send(client_socket1, csere_uzenet, sizeof(csere_uzenet), 0);
	send(client_socket2, csere_uzenet, sizeof(csere_uzenet), 0);

	char valasz1[10];
	char valasz2[10];

	char go[10] = "Mehet";
	recv(client_socket1, &valasz1, sizeof(valasz1), 0);
	recv(client_socket2, &valasz2, sizeof(valasz2), 0);

	send(client_socket1, &go, sizeof(go), 0);

	printf("1. jatekos csereinek szama: %d\n", (int)(valasz1[0]-'a'));
	printf("2. jatekos csereinek szama: %d\n", (int)(valasz2[0]-'a'));

	int csere_t1[(int)(valasz1[0]-'a')];
	for(int i = 0; i < (int)(valasz1[0]-'a'); i++){
		char csere_index[10];
		recv(client_socket1, &csere_index, sizeof(csere_index), 0);
		csere_t1[i] = (int)(csere_index[0]-'a');
	}
	for(int i = 0; i < (int)(valasz1[0]-'a'); i++){
		hand1[csere_t1[i]-1] = kartyak[pakli_i];
		pakli_i++;
	}

	send(client_socket2, &go, sizeof(go), 0);

	int csere_t2[(int)(valasz2[0]-'a')];
	for(int i = 0; i < (int)(valasz2[0]-'a'); i++){
		char csere_index[10];
		recv(client_socket2, &csere_index, sizeof(csere_index), 0);
		csere_t2[i] = (int)(csere_index[0]-'a');
	}
	for(int i = 0; i < (int)(valasz2[0]-'a'); i++){
		hand2[csere_t2[i]-1] = kartyak[pakli_i];
		pakli_i++;
	}
	rendez(hand1);
	rendez(hand2);
//2. osztás
	for(int i = 0; i < 5; i++){
		char lap_ertek[10];
		lap_ertek[0] = 'a'+hand1[i].ertek;
		lap_ertek[1] = '\0';
		printf("%d ", hand1[i].ertek);
		send(client_socket1, lap_ertek, sizeof(lap_ertek), 0);

		char lap_szin[10];
		lap_szin[0] = 'a'+hand1[i].szin;
		lap_szin[1] = '\0';
		printf("%d \t", hand1[i].szin);
		send(client_socket1, lap_szin, sizeof(lap_szin), 0);
	}
	printf("\n");

	for(int i = 0; i < 5; i++){
		char lap_ertek[10];
		lap_ertek[0] = 'a'+hand2[i].ertek;
		lap_ertek[1] = '\0';
		printf("%d ", hand2[i].ertek);
		send(client_socket2, lap_ertek, sizeof(lap_ertek), 0);

		char lap_szin[10];
		lap_szin[0] = 'a'+hand2[i].szin;
		lap_szin[1] = '\0';
		printf("%d \t", hand2[i].szin);
		send(client_socket2, lap_szin, sizeof(lap_szin), 0);
	}
	printf("\n");
//eredmény


	close(server_socket);

	return 0;
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

void combo_ir(COMBO* gyok){

	if(gyok != 0){
		printf("E: %d \t T: %d \n",gyok->ertek, gyok->tipus);
		combo_ir(gyok->kov);
	}
}

void winner(COMBO* gyok1, COMBO* gyok2){

	if(gyok1 == 0 && gyok2 == 0){
		printf("Dontetlen\n");
	} else{
		if(gyok2 == 0 || gyok1->tipus > gyok2->tipus || (gyok1->tipus == gyok2->tipus && gyok1->ertek > gyok2->ertek)){
			printf("Elso jatekos nyert\n");
		} else if(gyok1 == 0 || gyok1->tipus < gyok2->tipus || (gyok1->tipus == gyok2->tipus && gyok1->ertek < gyok2->ertek)){
			printf("Masodik jatekos nyert\n");
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


