#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include <stdbool.h>

// fájlok nevei 
#define FILE_TL  "tulajdonosok.txt"
#define FILE_AL  "allatok.txt"
#define FILE_LAT "vizsgalatok.txt"

bool init_files(void);

#endif