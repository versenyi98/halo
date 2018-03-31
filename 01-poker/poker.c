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

	srand(time(NULL));
	KARTYA kartyak[54];

	KARTYA hand1[5];
	COMBO* gyoker1 = 0;

	KARTYA hand2[5];
	COMBO* gyoker2 = 0;

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

	int pakli_i = 0;
	char character;

	pakli_i = 0;
	kever(kartyak, 52);

	osztas(kartyak, hand1, &pakli_i);
	rendez(hand1);

	gyoker1 = royal_flush(gyoker1, hand1);

	osztas(kartyak, hand2, &pakli_i);
	rendez(hand2);

	gyoker2 = royal_flush(gyoker2, hand2);

	kiir(hand1, szimbolum, spec); 
	combo_ir(gyoker1);
	kiir(hand2, szimbolum, spec); 
	combo_ir(gyoker2);

	winner(gyoker1, gyoker2);


	szabadit(gyoker1);
	szabadit(gyoker2);

	for(int i = 0; i < 4; i++){
		free(szimbolum[i]);
	}
	free(szimbolum);
	free(spec);


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

void kiir(KARTYA hand[], char** szimbolum, char* spec){
	for(int i = 0; i < 5; i++){
		printf("%s ", szimbolum[hand[i].szin]);
		if(hand[i].ertek <= 10){
			printf("%d\t", hand[i].ertek);
		} else{
			printf("%c\t", spec[hand[i].ertek-11]);
		}
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
