#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "storage.h"
#include "owners.h"
#include "menu_tulaj.h"
#include "debugmalloc.h"
#include "input.h"

int main(void) {
    DB db;
    db.tulajok = NULL;
    db.tulaj_db = 0;

    int valasztas;

    do {
        printf("\n=== Fomenu ===\n");
        printf("1) Tulajdonosok kezelese\n");
        printf("0) Kilepes\n");
        printf("Valasztas: ");

        valasztas = input_number();

        switch (valasztas) {
            case 0:
                printf("Kilepes...\n");
                break;
            case 1:
                menu_tulajdonosok(&db);
                break;
            default:
                printf("Ervenytelen opcio\n");
                break;
        }
    } while (valasztas != 0);

    free(db.tulajok);
    return 0;
}
