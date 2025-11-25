#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "owners.h"
#include "tulaj_valaszto.h"
#include "debugmalloc.h"

int choose_owner(DB *db) {
    if (!db || db->owner_cnt == 0) {
        puts("Nincs egy tulajdonos sem a rendszerben.");
        return -1;
    }

    char term[101];
    input_str("Keresett nev vagy nevreszlet: ", term, sizeof(term));

    if (!term[0]) {
        puts("Ures minta, nem tudok keresni.");
        return -1;
    }

    /* Print the results */
    int res_cnt = 0;
    for (int i = 0; i < db->owner_cnt; ++i) {
        if (strstr(db->owners[i].name, term) != NULL) {
            printf("%d) ID=%d; %s; %s\n", ++res_cnt, db->owners[i].id, db->owners[i].name, db->owners[i].contact);
        }
    }

    if (res_cnt == 0) {
        puts("Nincs talalat.");
        return -1;
    }

    int kiv = choose_result(1, db->owner_cnt);

    /* Search the real index of the selection in the src database. */
    int sorszam = 0;
    for (int i = 0; i < db->owner_cnt; ++i) {
        if (strstr(db->owners[i].name, term) != NULL) {
            ++sorszam;
            if (sorszam == kiv) {
                return i;  // <-- This is the index
            }
        }
    }

    puts("Kivalasztasi hiba.");
    return -1;
}
