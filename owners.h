#ifndef OWNERS_H
#define OWNERS_H

typedef struct Owner {
    int id;
    char name[101];
    char contact[101];
    struct Owner *next;
} Owner;

typedef struct DB {
    Owner *tulajok;
    int tulaj_db;
} DB;

Owner *tulajd_bet(int *out_db);

#endif
