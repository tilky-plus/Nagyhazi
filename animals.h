#ifndef ANIMALS_H
#define ANIMALS_H

typedef struct Animal {
    int id;
    int owner_id;
    char nev[101];
    char fajta[51];
    char szul[11]; // YYYY-MM-DD
    char oltas[11]; // utolso oltas datuma vagy ures string
} Animal;

Animal *allat_bet(int *out_db);

#endif