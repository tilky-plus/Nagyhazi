#include "files.h"

bool owner_file_rewrite(DB *db) {
    FILE *fp = fopen(FILE_TL, "w");

    if (!fp) {
        puts("Nem sikerult megnyitni a fajlt irasra");
        return false;
    }

    for (int i = 0; i < db->owner_cnt; ++i)
        fprintf(fp, "%d;%s;%s\n", db->owners[i].id, db->owners[i].name, db->owners[i].contact);

    fclose(fp);
    return true;
}


bool owner_quicksave(int id, char* name, char *contact) {
    FILE *fp = fopen(FILE_TL, "a");
    if (!fp) {
        puts("Nem sikerult irni a fileba (memoriaban benne van).");
        return false;
    }

    fprintf(fp, "%d;%s;%s\n", id, name, contact);
    fclose(fp);

    return true;
}