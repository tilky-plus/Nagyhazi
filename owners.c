#include <stdio.h>
#include <stdlib.h>
#include "owners.h"
#include "storage.h"
#include "debugmalloc.h"

Owner *tulajd_bet(int *out_db) {
    if (out_db) *out_db = 0;

    FILE *f = fopen(FILE_TL, "r");
    if (!f) {
        printf("Nem sikerult megnyitni a tulajdonosok fajlt.\n");
        return NULL;
    }

    Owner *tomb = NULL;
    int db = 0;
    char sor[512];

    while (fgets(sor, sizeof(sor), f)) {
        if (sor[0] == '\0' || sor[0] == '\n')  // ures sor kihagyasa
            continue;

        Owner *uj = realloc(tomb, (db + 1) * sizeof(Owner));
        if (!uj) {
            free(tomb);
            fclose(f);
            if (out_db) *out_db = 0;
            return NULL;
        }
        tomb = uj;

        // id;name;contact
        sscanf(sor, "%d;%100[^;];%100[^\n]",
               &tomb[db].id,
               tomb[db].name,
               tomb[db].contact);

        tomb[db].next = NULL;
        db++;
    }

    fclose(f);
    if (out_db) *out_db = db;
    return tomb;
}
