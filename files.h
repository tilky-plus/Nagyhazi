#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "storage.h"
#include "owners.h"

bool owner_file_rewrite(DB *db);
bool owner_quicksave(int id, char* name, char *contact);

#endif