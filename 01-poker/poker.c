#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
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
void kiir(KARTYA hand[], char* szimbolum[4]);

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

void combo_ir(COMBO* gyok, char* nev[10]);
void winner(COMBO* gyok1, COMBO* gyok2);

int main(){

	KARTYA kartyak[54];

	KARTYA hand1[5];
	COMBO* gyoker1 = 0;

	KARTYA hand2[5];
	COMBO* gyoker2 = 0;

	char* combo_nev[10];

	for(int i = 0; i < 10; i++){
		combo_nev[i] = (char*)malloc(sizeof(char*));
	}

	strcpy(combo_nev[0], "Magas lap");
	strcpy(combo_nev[1], "Egy par");
	strcpy(combo_nev[2], "Ket par");
	strcpy(combo_nev[3], "Drill");
	strcpy(combo_nev[4], "Sor");
	strcpy(combo_nev[5], "Flush");
	strcpy(combo_nev[6], "Full House");
	strcpy(combo_nev[7], "Poker");
	strcpy(combo_nev[8], "Sor Flush");
	strcpy(combo_nev[9], "Royal Flush");

	char* szimbolum[4];

	for(int i = 0; i < 4; i++){
		szimbolum[i] = (char*)malloc(sizeof(char*));
	}

	strcpy(szimbolum[0], "\xE2\x99\xA0");
	strcpy(szimbolum[1], "\xE2\x99\xA3");
	strcpy(szimbolum[2], "\xE2\x99\xA5");
	strcpy(szimbolum[3], "\xE2\x99\xA6");

	int pakli_i = 0;

	kever(kartyak, 54);

	osztas(kartyak, hand1, &pakli_i);
	rendez(hand1);
	printf("1:\t");

	gyoker1 = royal_flush(gyoker1, hand1);


	osztas(kartyak, hand2, &pakli_i);
	rendez(hand2);
	printf("2:\t");

	gyoker2 = royal_flush(gyoker2, hand2);


	if(gyoker2->tipus > 3 || gyoker1->tipus > 3){
		kiir(hand1, szimbolum);
		combo_ir(gyoker1, combo_nev);
		kiir(hand2, szimbolum);
		combo_ir(gyoker2, combo_nev);
	} else{
		system("./poker");
	}

/*
	csere(kartyak, hand1, &pakli_i);

	rendez(hand1);
	printf("VEGSO HAND\n");
	kiir(hand1, szimbolum);
*/

	winner(gyoker1, gyoker2);

	return 0;
}

void kever(KARTYA kartyak[], int meret){

	srand(time(NULL));
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
//		uj->azon = i;
		uj->szin = uj->azon/14;
		uj->ertek = (uj->azon%14)+1;
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

void csere(KARTYA kartyak[], KARTYA hand[], int *index){

	printf("Mennyit szeretnel cserelni? (0-5) : ");

	int csere = -1;
	while(csere < 0 || csere > 5){
		scanf("%d", &csere);
	}

	
	int indexek[csere];
	for(int i = 0; i < csere; i++){
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
				if(temp == indexek[j]){
					counter++;
					break;
				}
			}
			indexek[i] = temp;
		}while(counter != 0);
	}

	for(int i = 0; i < csere; i++){
		hand[indexek[i]-1] = kartyak[*index];
		*index = *index + 1;
	}
}

void kiir(KARTYA hand[], char** szimbolum){
	for(int i = 0; i < 5; i++){
		printf("%s %d\t", szimbolum[hand[i].szin], hand[i].ertek);
	}
	printf("\n");
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
		gyok = (COMBO*)malloc(sizeof(COMBO*));
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
	if(eggyel_nagyobb == 4 && szin_egyez == 4 && elozo != 14){
		gyok = (COMBO*)malloc(sizeof(COMBO*));
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
			gyok = (COMBO*)malloc(sizeof(COMBO*));
			gyok->tipus = 7;
			gyok->ertek = hand[i].ertek;
			gyok->kov = high_card(gyok->kov, hand, 4);
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
		gyok = (COMBO*)malloc(sizeof(COMBO*));
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
		gyok = (COMBO*)malloc(sizeof(COMBO*));
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
		} else{
			break;
		}
	}
	if(counter == 4){
		gyok = (COMBO*)malloc(sizeof(COMBO*));
		gyok->tipus = 4;
		gyok->ertek = hand[4].ertek;
		gyok->kov = 0;
	} else{
		gyok = three_of_a_kind(gyok, hand);
	}
	return gyok;
}

COMBO* three_of_a_kind(COMBO* gyok, KARTYA hand[]){
	
	for(int i = 1; i < 4; i++){
		if(hand[i-1].ertek == hand[i].ertek && hand[i].ertek == hand[i+1].ertek){
			gyok = (COMBO*)malloc(sizeof(COMBO*));
			gyok->tipus = 3;
			gyok->ertek = hand[i].ertek;
			gyok->kov = pair(gyok, hand);	
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
		gyok = (COMBO*)malloc(sizeof(COMBO*));
		gyok->tipus = 2;
		gyok->ertek = 0;
		gyok->kov = (COMBO*)malloc(sizeof(COMBO*));
		gyok->kov->tipus = 1;
		gyok->kov->ertek = nagy;
		gyok->kov->kov = (COMBO*)malloc(sizeof(COMBO*));
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
				if(i < 3 && hand[i+2].ertek != hand[i].ertek){
				gyok = (COMBO*)malloc(sizeof(COMBO*));
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
		gyok = (COMBO*)malloc(sizeof(COMBO*));
		gyok->tipus = 0;
		gyok->ertek = hand[num].ertek; 
		gyok->kov = high_card(gyok->kov, hand, num-1);
		return gyok;
	}
	
}

void combo_ir(COMBO* gyok, char* nev[10]){

	if(gyok != 0){
		printf("Ertek: %d \t Nev: %s \n",gyok->ertek, nev[gyok->tipus]);
		combo_ir(gyok->kov, nev);
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
