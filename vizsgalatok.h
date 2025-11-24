#ifndef VIZSGALATOK_H
#define VIZSGALATOK_H


typedef struct {
int id;
int allat_id;
char datum[11]; // ÉÉÉÉ-HH-NN -> sima char, nem fogok dátumokkal számolni
char leiras[201];
} Vizsgalat;


Vizsgalat *vizsg_bet(int *out_db);


#endif