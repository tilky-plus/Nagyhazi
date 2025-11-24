#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "owners.h"
#include "tulaj_valaszto.h"
#include "debugmalloc.h"

int valassz_tulaj(DB *db) {
    if (!db || db->tulaj_db == 0) {
        puts("Nincs egy tulajdonos sem a rendszerben.");
        return -1;
    }

    char minta[101];
    input_str("Keresett nev vagy nevreszlet: ", minta, sizeof(minta));

    if (!minta[0]) {
        puts("Ures minta, nem tudok keresni.");
        return -1;
    }

    /* találatok kiírása, sorszámozás */
    int darab = 0;
    for (int i = 0; i < db->tulaj_db; ++i) {
        if (strstr(db->tulajok[i].name, minta) != NULL) {
            printf("%d) ID=%d; %s; %s\n",
                   ++darab,
                   db->tulajok[i].id,
                   db->tulajok[i].name,
                   db->tulajok[i].contact);
        }
    }

    if (darab == 0) {
        puts("Nincs talalat.");
        return -1;
    }

    /* melyik találatot válasszuk? */
    printf("Melyiket valasszam? (1..%d, 0 = megse): ", darab);

    int kiv = input_number();
    if (kiv <= 0 || kiv > darab) {
        puts("Megse.");
        return -1;
    }

    /* visszakeressük az EREDETI indexet a tömbben */
    int sorszam = 0;
    for (int i = 0; i < db->tulaj_db; ++i) {
        if (strstr(db->tulajok[i].name, minta) != NULL) {
            ++sorszam;
            if (sorszam == kiv) {
                return i;  // <-- EZ AZ INDEX
            }
        }
    }

    puts("Kivalasztasi hiba.");
    return -1;
}
