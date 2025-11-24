#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "owners.h"
#include "storage.h"
#include "menu_tulaj.h"
#include "tulaj_valaszto.h"
#include "debugmalloc.h"

static void levag_ujonsor(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r'))
        s[--n] = '\0';
}

void menu_tulajdonosok(DB *db) {
    int valasztas;

    for (;;) {
        printf("\n--- Tulajdonosok kezelese ---\n");
        printf("1) Uj tulajdonos felvitele\n");
        printf("2) Tulajdonos keresese\n");
        printf("3) Osszes tulajdonos listazasa\n");
        printf("4) Tulajdonos adatainak modositasa\n");
        printf("5) Tulajdonos torlese\n");
        printf("0) Vissza a fomenube\n");
        printf("Valasztas: ");

        if (scanf("%d", &valasztas) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            continue;
        }
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }

        switch (valasztas) {
            case 1: { /* Uj tulajdonos felvitele */
                char nev[101], eler[101];

                printf("Tulajdonos neve: ");
                if (!fgets(nev, sizeof(nev), stdin)) {
                    puts("Hiba");
                    break;
                }
                levag_ujonsor(nev);
                if (!nev[0]) {
                    puts("A nev nem lehet ures.");
                    break;
                }

                printf("Elerhetoseg: ");
                if (!fgets(eler, sizeof(eler), stdin)) {
                    puts("Hiba");
                    break;
                }
                levag_ujonsor(eler);

                /* Kovetkezo ID */
                int kov_id = (db->tulaj_db > 0)
                           ? db->tulajok[db->tulaj_db - 1].id + 1
                           : 1;

                /* Tomb bovites es beszuras */
                Owner *uj = realloc(db->tulajok,
                                    (db->tulaj_db + 1) * sizeof(*db->tulajok));
                if (!uj) {
                    puts("Memoria hiba.");
                    break;
                }
                db->tulajok = uj;

                db->tulajok[db->tulaj_db].id = kov_id;
                snprintf(db->tulajok[db->tulaj_db].name, sizeof(db->tulajok[db->tulaj_db].name), "%s", nev);
                snprintf(db->tulajok[db->tulaj_db].contact, sizeof(db->tulajok[db->tulaj_db].contact), "%s", eler);
                db->tulaj_db++;

                /* Azonnali mentes: hozzaappendeljuk a fajl vegere */
                FILE *fp = fopen(FILE_TL, "a");
                if (!fp) {
                    puts("Nem sikerult irni a fileba (memoriaban benne van).");
                    break;
                }
                fprintf(fp, "%d;%s;%s\n", kov_id, nev, eler);
                fclose(fp);

                printf("Kesz: felvettuk (ID=%d).\n", kov_id);
                }
                break;

            case 2: { /* Tulajdonos keresese (nevreszletre) */
                char minta[101];

                printf("Keresett nev vagy nevreszlet: ");
                if (!fgets(minta, sizeof(minta), stdin)) {
                    puts("Beolvasasi hiba.");
                    break;
                }
                levag_ujonsor(minta);
                if (!minta[0]) {
                    puts("Ures szoveg, erre nem tudok rakeresni");
                    break;
                }

                int talalatok = 0;
                for (int i = 0; i < db->tulaj_db; ++i) {
                    if (strstr(db->tulajok[i].name, minta) != NULL) {
                        printf("%d; %s; %s\n",
                               db->tulajok[i].id,
                               db->tulajok[i].name,
                               db->tulajok[i].contact);
                        talalatok++;
                    }
                }
                if (!talalatok)
                    puts("Nincs talalat");
                break;
            }

            case 3: /* Osszes tulajdonos listazasa */
                if (db->tulaj_db == 0) {
                    puts("Nincs szemely felveve");
                    break;
                }
                for (int i = 0; i < db->tulaj_db; ++i) {
                    printf("%d; %s; %s\n",
                           db->tulajok[i].id,
                           db->tulajok[i].name,
                           db->tulajok[i].contact);
                }
                break;

            case 4: { /* Tulajdonos adatainak modositasa: TULAJ_VALASZT használata */
                if (db->tulaj_db == 0) {
                    puts("Nincs szemely felveve.");
                    break;
                }

                int idx = valassz_tulaj(db);
                if (idx < 0) {
                    /* nincs talalat / megse */
                    break;
                }

                Owner *t = &db->tulajok[idx];
                printf("Kivalasztott tulajdonos: ID=%d; %s; %s\n",
                       t->id, t->name, t->contact);

                /* Mit modositunk? */
                printf("Mit modositunk? 1) Nev  2) Elerhetoseg  (0 = megse): ");
                int mit = 0;
                if (scanf("%d", &mit) != 1) {
                    int c2;
                    while ((c2 = getchar()) != '\n' && c2 != EOF) {}
                    puts("Beolvasasi hiba.");
                    break;
                }
                int c2;
                while ((c2 = getchar()) != '\n' && c2 != EOF) {}

                if (mit == 0) {
                    puts("Megse.");
                    break;
                }
                if (mit != 1 && mit != 2) {
                    puts("Ervenytelen valasztas.");
                    break;
                }

                /* Uj adat beolvasasa */
                char uj[101];
                if (mit == 1)
                    printf("Uj nev: ");
                else
                    printf("Uj elerhetoseg: ");

                if (!fgets(uj, sizeof(uj), stdin)) {
                    puts("Hiba.");
                    break;
                }
                levag_ujonsor(uj);
                if (mit == 1 && !uj[0]) {
                    puts("A nev nem lehet ures.");
                    break;
                }

                /* Elonezet */
                int  id = t->id;
                char nev_buf[101];
                char eler_buf[101];

                if (mit == 1) {
                    snprintf(nev_buf, sizeof(nev_buf), "%s", uj);
                    snprintf(eler_buf, sizeof(eler_buf), "%s", t->contact);
                } else {
                    snprintf(nev_buf, sizeof(nev_buf), "%s", t->name);
                    snprintf(eler_buf, sizeof(eler_buf), "%s", uj);
                }

                printf("\n--- Elonezet (mentes elott) ---\n");
                printf("%d;%s;%s\n", id, nev_buf, eler_buf);

                /* Megerosites */
                char valasz[8];
                printf("Biztosan modositod? (I/N): ");
                if (!fgets(valasz, sizeof(valasz), stdin)) {
                    puts("Megszakitva");
                    break;
                }
                levag_ujonsor(valasz);
                if (!(valasz[0] == 'I' || valasz[0] == 'i')) {
                    puts("Megse");
                    break;
                }

                /* Memoria frissitese */
                if (mit == 1) {
                    snprintf(t->name, sizeof(t->name), "%s", uj);
                } else {
                    snprintf(t->contact, sizeof(t->contact), "%s", uj);
                }

                /* Teljes fajl ujrairasa */
                FILE *fp = fopen(FILE_TL, "w");
                if (!fp) {
                    puts("Nem sikerult megnyitni a fajlt irasra");
                    break;
                }
                for (int i = 0; i < db->tulaj_db; ++i) {
                    fprintf(fp, "%d;%s;%s\n",
                            db->tulajok[i].id,
                            db->tulajok[i].name,
                            db->tulajok[i].contact);
                }
                fclose(fp);

                puts("Mentes kesz");
                break;
            }

            case 5: { /* Tulajdonos torlese: TULAJ_VALASZTO használata */
                if (db->tulaj_db == 0) {
                    puts("Nincs szemely felveve.");
                    break;
                }

                int idx = valassz_tulaj(db);
                if (idx < 0) {
                    /* nincs talalat / megse */
                    break;
                }

                Owner *t = &db->tulajok[idx];
                printf("\nTorolni keszulod a kovetkezot:\n");
                printf("%d;%s;%s\n", t->id, t->name, t->contact);

                char valasz[8];
                printf("Biztosan toroljem? (I/N): ");
                if (!fgets(valasz, sizeof(valasz), stdin)) {
                    puts("Megszakitva.");
                    break;
                }
                levag_ujonsor(valasz);
                if (!(valasz[0] == 'I' || valasz[0] == 'i')) {
                    puts("Megse.");
                    break;
                }

                /* Elem kitorlese a dinamikus tombbol: elcsusztatjuk a hatso elemeket */
                for (int i = idx; i < db->tulaj_db - 1; ++i) {
                    db->tulajok[i] = db->tulajok[i + 1];
                }
                db->tulaj_db--;

                /* Opcionális: tomb zsugoritasa */
                if (db->tulaj_db == 0) {
                    free(db->tulajok);
                    db->tulajok = NULL;
                } else {
                    Owner *uj = realloc(db->tulajok, db->tulaj_db * sizeof(*db->tulajok));
                    if (uj) {
                        db->tulajok = uj;
                    }
                }

                /* Teljes fajl ujrairasa */
                FILE *fp = fopen(FILE_TL, "w");
                if (!fp) {
                    puts("Nem sikerult megnyitni a filet irasra.");
                    break;
                }
                for (int i = 0; i < db->tulaj_db; ++i) {
                    fprintf(fp, "%d;%s;%s\n",
                            db->tulajok[i].id,
                            db->tulajok[i].name,
                            db->tulajok[i].contact);
                }
                fclose(fp);

                puts("Torles kesz.");
                break;
            }

            case 0:
                /* vissza a fomenube */
                return;

            default:
                printf("Ervenytelen valasztas.\n");
                break;
        }
    }
}
