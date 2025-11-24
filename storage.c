#include "storage.h"
#include <stdio.h>
#include "debugmalloc.h"

bool init_files(void) {
    const char *files[] = { FILE_TL, FILE_AL, FILE_LAT };
    int db = 3;

    for (int i = 0; i < db; i++) {
        FILE *f = fopen(files[i], "r");
        if (f == NULL) {
            printf("%s nem talalhato, letrehozom...\n", files[i]);
            f = fopen(files[i], "w");
            if (f == NULL) {
                printf("Nem sikerult letrehozni: %s\n", files[i]);
                return false;
            }
        } else {
            printf("%s elerheto\n", files[i]);
        }
        fclose(f);
    }

    printf("Minden file ellenorizve es elerheto.\n");
    return true;
}
