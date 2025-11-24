#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vizsgalatok.h"
#include "storage.h"
#include "debugmalloc.h"

Vizsgalat *vizsg_bet(int *out_db) {
    if (out_db) *out_db = 0;

    FILE *f = fopen(FILE_LAT, "r");
    if (!f) {
        printf("Nem sikerult megnyitni a vizsgalatok fajlt.\n");
        return NULL;
    }

    Vizsgalat *tomb = NULL;
    int db = 0;
    char sor[512];

    while (fgets(sor, sizeof(sor), f)) {
        if (sor[0] == '\0' || sor[0] == '\n')
            continue;

        Vizsgalat *uj = realloc(tomb, (db + 1) * sizeof(Vizsgalat));
        if (!uj) {
            free(tomb);
            fclose(f);
            return NULL;
        }
        tomb = uj;

        sscanf(sor, "%d;%d;%10[^;];%200[^\n]",
               &tomb[db].id,
               &tomb[db].allat_id,
               tomb[db].datum,
               tomb[db].leiras);
        db++;
    }

    fclose(f);
    if (out_db) *out_db = db;
    return tomb;
}
