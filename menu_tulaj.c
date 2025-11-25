#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "owners.h"
#include "storage.h"
#include "files.h"
#include "menu_tulaj.h"
#include "tulaj_valaszto.h"
#include "debugmalloc.h"
#include "input.h"

void add_owner(DB *db) {
    char name[101], contact[101];

    input_str("Tulajdonos neve: ", name, sizeof(name));

    if (!name[0]) {
        puts("A nev nem lehet ures.");
        return;
    }

    input_str("Elerhetoseg: ", contact, sizeof(contact));

    /* Next ID */
    int next_id = (db->owner_cnt > 0) ? db->owners[db->owner_cnt - 1].id + 1 : 1;

    /* Expand and insert the new owner to the array */
    Owner *new = realloc(db->owners, (db->owner_cnt + 1) * sizeof(*db->owners));
    if (!new) {
        puts("Memoria hiba.");
        return;
    }
    db->owners = new;

    db->owners[db->owner_cnt].id = next_id;
    snprintf(db->owners[db->owner_cnt].name, sizeof(db->owners[db->owner_cnt].name), "%s", name);
    snprintf(db->owners[db->owner_cnt].contact, sizeof(db->owners[db->owner_cnt].contact), "%s", contact);
    db->owner_cnt++;

    if (!owner_quicksave(next_id, name, contact))
        return;

    printf("Kesz: felvettuk (ID=%d).\n", next_id);
}


void search_owner(DB *db) {
    char term[101];

    input_str("Keresett nev vagy nevreszlet: ", term, sizeof(term));

    if (!term[0]) {
        puts("Ures szoveg, erre nem tudok rakeresni");
        return;
    }

    int res_cnt = 0;
    for (int i = 0; i < db->owner_cnt; ++i) {
        if (strstr(db->owners[i].name, term) != NULL) {
            printf("%d; %s; %s\n", db->owners[i].id, db->owners[i].name, db->owners[i].contact);
            res_cnt++;
        }
    }

    if (!res_cnt)
        puts("Nincs talalat");
}

void list_owners(DB *db) {
    if (db->owner_cnt == 0) {
        puts("Nincs szemely felveve");
        return;
    }

    for (int i = 0; i < db->owner_cnt; ++i)
        printf("%d; %s; %s\n", db->owners[i].id, db->owners[i].name, db->owners[i].contact);
}

void modify_owner(DB *db) {
    if (db->owner_cnt == 0) {
        puts("Nincs szemely felveve.");
        return;;
    }

    int idx = choose_owner(db);
    if (idx < 0)
        return;

    Owner *t = &db->owners[idx];
    printf("Kivalasztott tulajdonos: ID=%d; %s; %s\n", t->id, t->name, t->contact);

    /* Get what to modify */
    printf("Mit modositunk? 1) Nev  2) Elerhetoseg  (0 = megsem): ");
    int type = input_number();
    if (type != 1 && type != 2) {
        puts("Megsem.");
        return;
    }

    /* Read new data */
    char new[101];
    if (type == 1)
        printf("Uj nev: ");
    else
        printf("Uj elerhetoseg: ");

    input_str(NULL, new, sizeof(new));

    if (type == 1 && !new[0]) {
        puts("A nev nem lehet ures.");
        return;;
    }

    /* Preview */
    int  id = t->id;
    char name_buf[101];
    char contact_buf[101];

    if (type == 1) {
        snprintf(name_buf, sizeof(name_buf), "%s", new);
        snprintf(contact_buf, sizeof(contact_buf), "%s", t->contact);
    } else {
        snprintf(name_buf, sizeof(name_buf), "%s", t->name);
        snprintf(contact_buf, sizeof(contact_buf), "%s", new);
    }

    printf("\n--- Elonezet (mentes elott) ---\n");
    printf("%d;%s;%s\n", id, name_buf, contact_buf);

    if (!simple_yesno("Biztosan modositod?")) {
        return;;
    }

    /* Memoria frissitese */
    if (type == 1) {
        snprintf(t->name, sizeof(t->name), "%s", new);
    } else {
        snprintf(t->contact, sizeof(t->contact), "%s", new);
    }

    if (!owner_file_rewrite(db))
        return;;

    puts("Mentes kesz");
}

void delete_owner(DB *db) {
    if (db->owner_cnt == 0) {
        puts("Nincs szemely felveve.");
        return;
    }

    int idx = choose_owner(db);
    if (idx < 0)
        return;

    Owner *t = &db->owners[idx];
    printf("\nTorolni keszulod a kovetkezot:\n");
    printf("%d;%s;%s\n", t->id, t->name, t->contact);

    if (!simple_yesno("Biztosan toroljem?"))
        return;;

    /* Shift the array */
    for (int i = idx; i < db->owner_cnt - 1; ++i) {
        db->owners[i] = db->owners[i + 1];
    }
    db->owner_cnt--;

    /* Resize the array */
    if (db->owner_cnt == 0) {
        free(db->owners);
        db->owners = NULL;
    } else {
        Owner *new = realloc(db->owners, db->owner_cnt * sizeof(*db->owners));
        if (new) {
            db->owners = new;
        }
    }

    owner_file_rewrite(db);
    puts("Torles kesz.");
}

void menu_owners(DB *db) {
    while (true) {
        printf("\n--- Tulajdonosok kezelese ---\n");
        printf("1) Uj tulajdonos felvitele\n");
        printf("2) Tulajdonos keresese\n");
        printf("3) Osszes tulajdonos listazasa\n");
        printf("4) Tulajdonos adatainak modositasa\n");
        printf("5) Tulajdonos torlese\n");
        printf("0) Vissza a fomenube\n");
        printf("Valasztas: ");

        int input = input_number();

        switch (input) {
            case 0:
                return;

            case 1:
                add_owner(db);
                break;

            case 2:
                search_owner(db);
                break;

            case 3:
                list_owners(db);
                break;

            case 4:
                modify_owner(db);
                break;

            case 5:
                delete_owner(db);
                break;

            default:
                printf("Ervenytelen valasztas.\n");
                break;
        }
    }
}
