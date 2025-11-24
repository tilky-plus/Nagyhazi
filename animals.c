#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "animals.h"
#include "storage.h"
#include "debugmalloc.h"

static void levag_ujonsor(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

Animal *allat_bet(int *ki_db) {
    if (ki_db) *ki_db = 0;

    FILE *fp = fopen(FILE_AL, "r");
    if (!fp) {
        printf("Nem sikerult megnyitni az allatok fajlt.\n");
        return NULL;
    }

    Animal *tomb = NULL;
    int darab = 0;
    char sor[1024];

    while (fgets(sor, sizeof(sor), fp)) {
        levag_ujonsor(sor);
        if (!sor[0]) continue;

        Animal *uj = realloc(tomb, (darab + 1) * sizeof(*tomb));
        if (!uj) {
            fclose(fp);
            free(tomb);
            return NULL;
        }
        tomb = uj;

        int talalat = sscanf(sor, "%d;%d;%100[^;];%50[^;];%10[^;];%10[^\n]",
                             &tomb[darab].id,
                             &tomb[darab].owner_id,
                             tomb[darab].nev,
                             tomb[darab].fajta,
                             tomb[darab].szul,
                             tomb[darab].oltas);

        if (talalat < 5) {
            /* kotelezo 5 mezo (az oltas hianyozhat), kulonben kihagyjuk */
            continue;
        }
        if (talalat == 5) {
            /* nincs oltas -> uresre all */
            tomb[darab].oltas[0] = '\0';
        }

        darab++;
    }

    fclose(fp);
    if (ki_db) *ki_db = darab;
    return tomb;
}
