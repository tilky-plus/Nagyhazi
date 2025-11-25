#include <stdio.h>
#include <stdlib.h>
#include "owners.h"
#include "storage.h"
#include "debugmalloc.h"

#define ARBITRARY_LINE_LENGTH 512

Owner *owner_file_load(int *dst) {
    if (dst) *dst = 0;

    FILE *f = fopen(FILE_TL, "r");
    if (!f) {
        printf("Nem sikerult megnyitni a tulajdonosok fajlt.\n");
        return NULL;
    }

    Owner *owner_arr = NULL;
    int cnt = 0;
    char line[ARBITRARY_LINE_LENGTH];

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '\0' || line[0] == '\n')  /* Ignore empty lines */
            continue;

        Owner *new = realloc(owner_arr, (cnt + 1) * sizeof(Owner));
        if (!new) {
            free(owner_arr);
            fclose(f);
            if (dst) *dst = 0;
            return NULL;
        }
        owner_arr = new;

        /* An owner structure's in a file should be: id;name;contact */
        sscanf(line, "%d;%100[^;];%100[^\n]", &owner_arr[cnt].id, owner_arr[cnt].name, owner_arr[cnt].contact);
        owner_arr[cnt].next = NULL;
        cnt++;
    }

    fclose(f);
    if (dst) *dst = cnt;
    return owner_arr;
}
