#ifndef TULAJ_VALASZTO_H
#define TULAJ_VALASZTO_H

#include "owners.h"   // DB, Owner

/* Mivel tulajdonos keresést több helyen is használunk, 
ezért jó öletnek tartottam külön függvénybe kirendezni*/
int valassz_tulaj(DB *db);

#endif
