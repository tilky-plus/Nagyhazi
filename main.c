#include <stdio.h>
#include <stdlib.h>
#include "storage.h"
#include "owners.h"
#include "menu_tulaj.h"
#include "debugmalloc.h"

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
        if (scanf("%d", &valasztas) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            continue;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF) { }

        switch (valasztas) {
            case 1:
                menu_tulajdonosok(&db);
                break;
            case 0:
                printf("Kilepes...\n");
                break;
            default:
                printf("Ervenytelen opcio\n");
        }
    } while (valasztas != 0);

    free(db.tulajok);
    return 0;
}
