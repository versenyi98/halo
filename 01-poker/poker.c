#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

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
void kiir(KARTYA hand[]);

COMBO* royal_flush(COMBO* gyok, KARTYA hand[]);
COMBO* straight_flush(COMBO* gyok, KARTYA hand[]);
COMBO* four_of_a_kind(COMBO* gyok, KARTYA hand[]);
COMBO* full_house(COMBO* gyok, KARTYA hand[]);
COMBO* flush(COMBO* gyok, KARTYA hand[]);
COMBO* straight(COMBO* gyok, KARTYA hand[]);
COMBO* three_of_a_kind(COMBO* gyok, KARTYA hand[]);
COMBO* two_pair(COMBO* gyok, KARTYA hand[]);
COMBO* pair(COMBO* gyok, KARTYA hand[]);
COMBO* high_card(COMBO* gyok, KARTYA hand[]);

int main(){

	KARTYA kartyak[54];
	KARTYA hand1[5];
	KARTYA hand2[5];
	COMBO* gyoker = 0;


	int pakli_i = 0;

	kever(kartyak, 54);
	osztas(kartyak, hand1, &pakli_i);
	csere(kartyak, hand1, &pakli_i);
	rendez(hand1);
//
	hand1[0].ertek = 3;
	hand1[0].szin = 0;

	hand1[1].ertek = 3;
	hand1[1].szin = 1;

	hand1[2].ertek = 3;
	hand1[2].szin = 2;

	hand1[3].ertek = 5;
	hand1[3].szin = 3;

	hand1[4].ertek = 5;
	hand1[4].szin = 0;

	kiir(hand1);

	gyoker = royal_flush(gyoker, hand1);
	printf("%d\n", gyoker->tipus);
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
	printf("HAND1\n");
	kiir(hand);

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

	printf("CSERE UTAN\n");
	kiir(hand);

}

void kiir(KARTYA hand[]){
	for(int i = 0; i < 5; i++){
		printf("E: %d\tSZ: %d\n", hand[i].ertek, hand[i].szin);
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

	printf("RENDEZES\n");
	kiir(hand);
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
		gyok->tipus = 10;
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
		gyok->tipus = 9;
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
			gyok->tipus = 8;
			gyok->ertek = hand[i].ertek;
			gyok->kov = 0;								//magas lap
		}
	}
	if(gyok == 0){
		gyok = full_house(gyok, hand);
	}

	return gyok;
}

COMBO* full_house(COMBO* gyok, KARTYA hand[]){

	int egyezes;

	for(int i = 0; i < 5; i++){
		for(int j = i; j < 5; j++){
			if(hand[i].ertek == hand[j].ertek){
				egyezes++;
			}
		}	
	}
	if(egyezes == 9){								//3+2+1+2+1 || 2+1+3+2+1
		gyok = (COMBO*)malloc(sizeof(COMBO*));
		gyok->tipus = 7;
		gyok->ertek = 0;
		gyok->kov = 0;								//drill

	} else{
		gyok = 0;
	}



	return gyok;
}

/*
COMBO* flush(COMBO* gyok, KARTYA hand[]);
COMBO* straight(COMBO* gyok, KARTYA hand[]);
COMBO* three_of_a_kind(COMBO* gyok, KARTYA hand[]);
COMBO* two_pair(COMBO* gyok, KARTYA hand[]);
COMBO* pair(COMBO* gyok, KARTYA hand[]);
COMBO* high_card(COMBO* gyok, KARTYA hand[]);



*/





