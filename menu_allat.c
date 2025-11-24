#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "owners.h"       // DB
#include "animals.h"      // Animal struktúra és függvények
#include "vizsgalatok.h"  // Vizsgálatok kezelése
#include "storage.h"
#include "menu_allat.h"
#include "debugmalloc.h"
#include "tulaj_valaszto.h"

/* ----------------------------------------------------------
 *  Állatokhoz tartozó almenü (csak a tulaj lesz láncolt lista, ez nem)
 * ---------------------------------------------------------- */

static void levag_ujonsor(char *s) {
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r'))
        s[--n] = '\0';
}

 void menu_allatok(DB *db) {
    int valasztas = -1;

    do {
        printf("\n=== ÁLLATOK MENÜ ===\n");
        printf("1. Új állat hozzáadása\n");
        printf("2. Állat adatainak módosítása\n");
        printf("3. Adott tulajdonos állatainak listázása\n");
        printf("4. Állat keresése név alapján\n");
        printf("5. Állat törlése vizsgálatokkal együtt\n");
        printf("0. Vissza a főmenübe\n");
        printf("-----------------------------\n");
        printf("Választás: ");

        if (scanf("%d", &valasztas) != 1) {
            puts("Érvénytelen bemenet.");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        // Szűrjük le az input után maradt sortörést
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        switch (valasztas) {
            /* Új állat felvétele */
            case 1:
                if (!db || db->tulaj_db == 0){
                    puts("Nincs egy tulajdonos sem a rendszerben, először vegyél fel tulajdonost");
                    break;
                }
                /* Tulaj kiválasztása*/
                int idx = valassz_tulaj(db);
                if (idx < 0){
                    break;
                }
                Owner *tulaj = &db->tulajok[idx];
                printf("Kivalasztott tulajdonos: %s; %s\n", tulaj->name, tulaj->contact);

                /* Állatok beolvasása file-ból */
                int allat_db = 0;
                Animal *allatok = allat_bet(&allat_db);
                if (!allatok && allat_db > 0){
                    puts("Nem sikerult beolvasni az allatokat.");
                    break;
                }

                /* Új állat adatainak bekérése*/
                char nev[101];
                char fajta[51];
                char szul[11];
                char oltas[11];
                
                printf("Allat neve: ");
                if(!fgets(nev, sizeof(nev), stdin)){
                    puts("Beolvasasi hiba. ");
                    free(allatok);
                    break;
                }
                levag_ujonsor(nev);
                if (!nev[0]){
                    puts("A nev nem lehet ures.");
                    free(allatok);
                }
                printf("Allat fajtaja: ");
                if(!fgets(fajta, sizeof(fajta), stdin)){
                    puts("Beolvasasi hiba");
                    free(allatok);
                    break;
                }
                levag_ujonsor(fajta);
                if (!fajta[0]){
                    puts("A fajta nem lehet ures! ");
                    free(allatok);
                    break;
                }

                printf("Szuletesi datum (EEEE-HH-NN): ");
                if(!fgets(szul, sizeof(szul), stdin)){
                    puts("Beolvasasi hiba");
                    free(allatok);
                    break;
                }
                levag_ujonsor(szul);
                if (!szul[0]) {
                    puts("A szuletesi datum nem lehet ures.");
                    free(allatok);
                    break;
                }

                printf("Utolso oltas datuma (EEEE-HH-DD), vagy ures ha nincsen");
                if(!fgets(fajta, sizeof(fajta), stdin)){
                    puts("Beolvasasi hiba");
                    free(allatok);
                    break;
                }
                /* Kovetkezo allat ID */
                int kov_id = 1;
                if (allat_db > 0 && allatok != NULL){
                    kov_id = allatok[allat_db - 1].id + 1;
                    if (kov_id <= 0)
                        kov_id = 1;
                }
                /* Tömb bővítése, új elem hozzáadása */
                Animal *ujtomb = realloc(allatok, (allat_db + 1) * sizeof(*allatok));
                if(!ujtomb){
                    puts("Hiba az allat hozzaadasakor");
                    free(allatok);
                    break;
                }
                allatok = ujtomb;

                allatok[allat_db].id = kov_id;
                allatok[allat_db].owner_id = tulaj->id;
                snprintf(allatok[allat_db].nev, sizeof(allatok[allat_db].nev), "%s", nev);
                snprintf(allatok[allat_db].fajta, sizeof(allatok[allat_db].nev), "%s", fajta);
                snprintf(allatok[allat_db].szul, sizeof(allatok[allat_db].szul), "%s", szul);

                /* Ha van oltás*/
                if(oltas[0]){
                    snprintf(allatok[allat_db].oltas, sizeof(allatok[allat_db].oltas), "%s", oltas);
                }
                /* Különben */
                else {
                    allatok[allat_db].oltas[0] = '\0';
                }
                allat_db++;

                /* Teljes állat file újraírása*/
                FILE *fp = fopen(FILE_AL, "w");
                /* Ha nem tudjuk megnyitni írásra */
                if (!fp){
                    puts("Nem sikerült megnyitni a file-t ujrairasra. ");
                    free(allatok);
                    break;
                }
                for (int i = 0; i < allat_db; ++i){
                    fprintf(fp, "%d;%d;%s;%s;%s;%s;%s\n",
                        allatok[i].id,
                        allatok[i].owner_id,
                        allatok[i].nev,
                        allatok[i].fajta,
                        allatok[i].szul,
                        allatok[i].oltas);
                }
                fclose(fp);
                free(allatok);
                puts("Uj allat sikeresen hozzaadva. ");
                break;
                
                


            case 2:
                /* Állat adatainak módosítása */
                /* Állatok beolvasása */
                allat_db = 0;
                allatok = allat_bet(&allat_db);
                if(!allatok && allat_db > 0) {
                    puts("Nem sikerult beolasni az allatokat.");
                    break;
                }
                if (allat_db == 0){
                    puts("Nincs egy allat sem a rendszerben");
                    free(allatok);
                    break;
                }
                /* Keresés névrészlet alapján */
                char minta[101];
                printf(" Keresett nev vagy nevreszelt: ");
                if (!fgets(minta, sizeof(minta), stdin)){
                    puts("Beolvasasi hiba ");
                    free(allatok);
                    break;
                }
                levag_ujonsor(minta);
                if (!minta[0]){
                    puts("Nevreszlet kotelezo");
                    free(allatok);
                    break;
                }
                int darab = 0;
                for (int i = 0; i < allat_db; ++i) {
                    if (strstr(allatok[i].nev, minta) != NULL) {
                        printf("%d) ID=%d; TulajID=%d; %s; %s; %s; %s\n",
                            ++darab,
                            allatok[i].id,
                            allatok[i].owner_id,
                            allatok[i].nev,
                            allatok[i].fajta,
                            allatok[i].szul,
                            allatok[i].oltas[0] ? allatok[i].oltas : "(nincs oltas)");
                    }
                }
                if (darab == 0){
                    puts("Nincs talalat. ");
                    free(allatok);
                    break;
                }
                printf("Melyik allattal dolgozzunk? (1..%d, 0 = megsem): ", darab);
                int kiv = 0;
                if (scanf("%d", &kiv) != 1){
                    int c3;
                    while((c3 = getchar()) != '\n' && c3 != EOF){}
                    puts("Beolvasasi hiba. ");
                    free(allatok);
                    break;
                }
                int c3;
                while ((c3 = getchar()) != '\n' && c3 != EOF) {}
                if (kiv == 0){
                    puts("Megsem");
                    free(allatok);
                    break;
                }
                if (kiv < 1 || kiv > darab){
                    puts("Ervenytelen sorszam. ");
                    free(allatok);
                    break;
                }

                idx = -1;
                int sorszam = 0;
                for (int i = 0; i < allat_db; ++i){
                    if(strstr(allatok[i].nev, minta) != NULL){
                        ++sorszam;
                        if (sorszam == kiv){
                            idx = 1;
                            break;
                        }
                    }
                }
                if (idx < 0){
                    puts("Kivalasztasi hiba");
                    free(allatok);
                    break;
                }

                /* Az allatok[idx] lesz a módosítandó sor*/
                Animal *a = &allatok[idx];

                printf("\n---Jelenlegi adatok---\n");
                printf("Nev: %s; Fajta: %s; Szuletesi datum: %s; oltas: %s\n",
                    a->nev,
                    a->fajta,
                    a->szul,
                    a->oltas[0] ? a->oltas : "(nincs oltas)");

                /* MIt módosítunk? */
                printf("Mit modositunk?\n");
                printf("1) Nev\n");
                printf("2) Fajta\n");
                printf("3) Szuletesi datum\n");
                printf("4) Utolso oltas datuma\n");
                printf("0) Megsem\n");
                printf("Valasztas: ");

                int mit = 0;
                if (scanf("%d", &mit) != 1){
                    int c4;
                    while ((c4 = getchar()) != '\n' && c4 != EOF){}
                    puts("Beolvasasi hiba. ");
                    free(allatok);
                    break;
                }
                int c4;
                while ((c4 = getchar()) != '\n' && c4 != EOF){}
                if (mit == 0){
                    puts("Megsem");
                    free(allatok);
                    break;
                }
                if (mit < 1 || mit > 4){
                    puts("Ervenytelen valasztas");
                    free(allatok);
                    break;
                }

                /* Új adat megadása */
                char uj[101];
                if (mit == 1)
                    printf("Uj nev: ");
                else if (mit == 2)
                    printf("Uj fajta: ");
                else if (mit == 3)
                    printf("Uj szuletesi datum (EEEE-HH-NN): ");
                else
                    printf("Uj utolso oltas datuma (EEEE-HH-NN) vagy ures sor: ");

                if(!fgets(uj, sizeof(uj), stdin)){
                    puts("Beolvasasi hiba. ");
                    free(allatok);
                    break;
                }
                levag_ujonsor(uj);
                if ((mit == 1 || mit == 2 || mit == 3) && !uj[0]) {
                    puts("Ez a mezo nem lehet ures.");
                    free(allatok);
                    break;
                }
                /* Előnézet (lehet rájön a felhasználó, hogy nem jó adatot módosított)*/
                char nev_pre[101];
                char fajta_pre[51];
                char szul_pre[11];
                char oltas_pre[11];

                /* preview */
                printf("\n--- Elonezet (mentes elott) ---\n");
                printf("%s; ", (mit == 1) ? uj : a->nev);
                printf("%s; ", (mit == 2) ? uj : a->fajta);
                printf("%s; ", (mit == 3) ? uj : a->szul);
                const char *olt_pre;
                if (mit == 4) {
                    if (uj[0])
                        olt_pre = uj;
                    else
                        olt_pre = "(nincs oltas)";
                } else {
                    olt_pre = a->oltas[0] ? a->oltas : "(nincs oltas)";
                }
                printf("%s\n", olt_pre);

                /* 9) Megerősítés az előnézet után */
                char valasz[8];
                printf("Biztosan modositod? (I/N): ");
                if (!fgets(valasz, sizeof(valasz), stdin)) {
                    puts("Megszakitva.");
                    free(allatok);
                    break;
                }
                levag_ujonsor(valasz);
                if (!(valasz[0] == 'I' || valasz[0] == 'i')) {
                    puts("Megsem.");
                    free(allatok);
                    break;
                }

                /* Ténylges módosítás */
                if (mit == 1) {
                    snprintf(a->nev, sizeof(a->nev), "%s", uj);
                } else if (mit == 2) {
                    snprintf(a->fajta, sizeof(a->fajta), "%s", uj);
                } else if (mit == 3) {
                    snprintf(a->szul, sizeof(a->szul), "%s", uj);
                } else { /* mit == 4 */
                    if (uj[0])
                        snprintf(a->oltas, sizeof(a->oltas), "%s", uj);
                    else
                        a->oltas[0] = '\0';
                }

                /* Teljes állatzfile újraírása */
                fp = fopen(FILE_AL, "w");
                if (!fp) {
                    puts("Nem sikerult megnyitni az allatok fajlt irasra.");
                    free(allatok);
                    break;
                }

                for (int i = 0; i < allat_db; ++i) {
                    fprintf(fp, "%d;%d;%s;%s;%s;%s\n",
                            allatok[i].id,
                            allatok[i].owner_id,
                            allatok[i].nev,
                            allatok[i].fajta,
                            allatok[i].szul,
                            allatok[i].oltas);
                }

                fclose(fp);
                free(allatok);

                puts("Modositas kesz.");
                break;


            case 3:{
                if (!db || db->tulaj_db == 0) {
                    puts("Nincs egy tulajdonos sem a rendszerben, elobb vegyel fel tulajdonost.");
                    break;
                }

                /* Tulaj kivalasztasa a kozos tulaj_valaszto modullal */
                int idx = valassz_tulaj(db);
                if (idx < 0) {
                    /* Nincs talalat */
                    break;
                }

                Owner *tulaj = &db->tulajok[idx];
                printf("Kivalasztott tulajdonos: %s; %s\n", tulaj->name, tulaj->contact);

                /* Allatok beolvasasa fajlbol */
                int allat_db = 0;
                Animal *allatok = allat_bet(&allat_db);
                if (!allatok && allat_db > 0) {
                    puts("Nem sikerult beolvasni az allatok fajljat.");
                    break;
                }

                if (allat_db == 0) {
                    puts("Nincs egy allat sem a rendszerben.");
                    free(allatok);
                    break;
                }

                /* Az adott tulajdonoshoz tartozó állatok listázása */
                int talalt = 0;
                printf("\n--- %s tulajdonos allatai ---\n", tulaj->name);

                for (int i = 0; i < allat_db; ++i) {
                    if (allatok[i].owner_id == tulaj->id) {
                        printf("%s; %s; %s; %s\n",
                            allatok[i].nev,
                            allatok[i].fajta,
                            allatok[i].szul,
                            allatok[i].oltas[0] ? allatok[i].oltas : "(nincs oltas)");
                        ++talalt;
                    }
                }

                if (!talalt) {
                    puts("Ennek a tulajdonosnak jelenleg nincs egy allat sem a rendszerben.");
                }

                free(allatok);
                break;
            }

                break;

            case 4: {int allat_db = 0;
                Animal *allatok = allat_bet(&allat_db);
                if (!allatok && allat_db > 0) {
                    puts("Nem sikerult beolvasni az allatok fajljat.");
                    break;
                }
                if (allat_db == 0) {
                    puts("Nincs egy allat sem a rendszerben.");
                    free(allatok);
                    break;
                }

                /*  nevreszlet keresese */
                char minta[101];
                printf("Keresett nev vagy nevreszlet: ");
                if (!fgets(minta, sizeof(minta), stdin)) {
                    puts("Beolvasasi hiba");
                    free(allatok);
                    break;
                }
                levag_ujonsor(minta);
                if (!minta[0]) {
                    puts("Nevreszlet kotelezo.");
                    free(allatok);
                    break;
                }

                /* Kereses + kiiras tulajjal egyutt */
                int talalt = 0;
                for (int i = 0; i < allat_db; ++i) {
                    if (strstr(allatok[i].nev, minta) != NULL) {

                        /* Megprobaljuk kikeresni a tulaj nevet/elerhetoseget is */
                        const char *tulajnev  = "(ismeretlen tulaj)";
                        const char *tulajeler = "";
                        if (db && db->tulajok && db->tulaj_db > 0) {
                            for (int j = 0; j < db->tulaj_db; ++j) {
                                if (db->tulajok[j].id == allatok[i].owner_id) {
                                    tulajnev  = db->tulajok[j].name;
                                    tulajeler = db->tulajok[j].contact;
                                    break;
                                }
                            }
                        }

                        printf("Nev=%s; Fajta=%s; Szuletesi datum=%s; Oltas=%s; Tulaj=%s; Elerhetoseg=%s\n",
                               allatok[i].nev,
                               allatok[i].fajta,
                               allatok[i].szul,
                               allatok[i].oltas[0] ? allatok[i].oltas : "(nincs oltas)",
                               tulajnev,
                               tulajeler);

                        ++talalt;
                    }
                }

                if (!talalt) {
                    puts("Nincs talalat.");
                }

                free(allatok);
                break;
            }

                break;

            case 5: {
                int allat_db = 0;
                Animal *allatok = allat_bet(&allat_db);
                if(!allatok && allat_db > 0) {
                    puts("Nem sikerult beolasni az allatokat.");
                    break;
                }
                if (allat_db == 0){
                    puts("Nincs egy allat sem a rendszerben");
                    free(allatok);
                    break;
                }
                /* Vizsgálatok beolvasása */
                int viz_db = 0;
                Vizsgalat *vizsg = vizsg_bet(&viz_db);
                if (!vizsg && viz_db > 0){
                    puts("Nem sikerult beolvasni a vizsgalatok filet");
                    free(allatok);
                    break;
                }
                /* Keresés névrészlet alapján */
                char minta[101];
                printf(" Keresett nev vagy nevreszelt: ");
                if (!fgets(minta, sizeof(minta), stdin)){
                    puts("Beolvasasi hiba ");
                    free(allatok);
                    break;
                }
                levag_ujonsor(minta);
                if (!minta[0]){
                    puts("Nevreszlet kotelezo");
                    free(allatok);
                    free(vizsg);
                    break;
                    int darab = 0;
                    for (int i = 0; i < allat_db; ++i){
                        if (strstr(allatok[i].nev, minta) != NULL){
                            printf("%d) %s; %s; %s; %s", ++darab, allatok[i].nev, allatok[i].fajta, allatok[i].szul, allatok[i].oltas[0] ? allatok[i].oltas : "(nincs oltas)");
                        }
                    }
                    if (darab == 0){
                        puts("Nincs talalat");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    /* Választás a találtak közül*/
                    printf("Melyik allattal dolgozzunk? (1..%d, 0 = megsem): ", darab);
                    int kiv = 0;
                    if (scanf("%d", &kiv) != 1){
                        int c;
                        while((c = getchar()) != '\n' && c != EOF){}
                        puts("Beolvasasi hiba");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF){}
                    if (kiv == 0){
                        puts("Megsem");
                        free(vizsg);
                        free(allatok);
                        break;
                    }
                    if (kiv < 1 || kiv > darab){
                        puts("Ervenytelen sorszam");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    int idx = -1;
                    int sorszam = 0;
                    for (int i = 0; i < allat_db; ++i){
                        if (strstr(allatok[i].nev, minta) != NULL){
                            ++sorszam;
                            if (sorszam == kiv) {
                                idx = i;
                                break;
                            }
                        }
                    }
                    if (idx < 0) {
                        puts("Kivalasztasi hiba");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    Animal *a = &allatok[idx];
                    int torlendo_id = a->id;

                    /* Előnézet */
                    printf("\nTorolni keszulod az alabbi allatot:\n");
                    printf("%s; %s; %s; %s", a->nev, a->fajta, a->szul, a->oltas);
                    printf("\nHozzatartozo vizsgalatok:\n");
                    int volt_vizsg = 0;
                    for (int i = 0; i < viz_db; ++i){
                        if (vizsg && vizsg[i].allat_id == torlendo_id){
                            printf("Datum = %s; Leiras = %s\n", vizsg[i].datum, vizsg[i].leiras);
                            volt_vizsg = 1;
                        }
                    }
                    if (!volt_vizsg){
                        puts("Ehhez az állathoz nem volt vizsgalat");
                    }
                    /* Megerosites */
                    char valasz [8];
                    printf("\nBiztosan toroljem az allatot ES az osszes hozza tartozo vizsgalatot? (I/N): ");
                    if (!fgets(valasz, sizeof(valasz), stdin)){
                        puts("Megszakitva");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    levag_ujonsor(valasz);
                    if (!(valasz[0] == 'I' || valasz[0] == 'i')){
                        puts("Megsem");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    /* Csúsztatás */
                    for (int i = idx + 1; i < allat_db; ++i){
                        allatok[i - 1] = allatok[i];
                    }
                    allat_db--;

                    /* Vizsgálatok törlése */
                    int uj_viz_db = 0;
                    if (viz_db > 0 && vizsg){
                        for (int i = 0; i < viz_db; ++i){
                            if (vizsg[i].allat_id != torlendo_id){
                                vizsg[uj_viz_db++] = vizsg[i];
                            }
                        }
                    }
                    /* Állatok file újraírása*/
                    FILE *fa = fopen(FILE_AL, "w");
                    if(!fa){
                        puts("Nem sikerult megnyitni a filet irasra. ");
                        free(allatok);
                        free(vizsg);
                        break;
                    }
                    for (int i = 0; i < allat_db; ++i){
                        fprintf(fa, "%d;%d;%s;%s;%s;%s\n",
                            allatok[i].id,
                            allatok[i].owner_id,
                            allatok[i].nev,
                            allatok[i].fajta,
                            allatok[i].szul,
                            allatok[i].oltas);
                    }
                    fclose(fa);

                    /* Vizsgálatok file fájl újraírása*/

                    FILE *fv = fopen(FILE_LAT, "w");
                    if (!fv){
                        puts("Nem sikerult megnyitni a filet irasra. ");
                        free(allatok);
                        free(vizsg);
                        break;
                    }

                    for (int i = 0; i < uj_viz_db; ++i){
                        fprintf(fv, "%d;%d;%s;%s\n", vizsg[i].id, vizsg[i].allat_id, vizsg[i].datum, vizsg[i].leiras);
                    }
                    fclose(fv);
                    free(allatok);
                    free(vizsg);
                    puts("Allat es minden hozzatartozo vizsgalat torolve");
                    break;




                }
                case 0:
                puts("Visszalépés a főmenübe...");
                break;

                default:
                puts("Ismeretlen menüpont, próbáld újra.");
                break;
            }

        }
    } while (valasztas != 0);
}
