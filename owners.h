#ifndef OWNERS_H
#define OWNERS_H

typedef struct Owner {
    int id;
    char name[101];
    char contact[101];
    struct Owner *next;
} Owner;

typedef struct DB {
    Owner *owners;
    int owner_cnt;
} DB;

Owner *owner_file_load(int *dst);

#endif
