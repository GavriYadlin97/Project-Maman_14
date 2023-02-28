/* Created by Adi.s & Gavri.y 30/01/2023. */

#ifndef PROJECT_MAMAN_14_PREASM_H
#define PROJECT_MAMAN_14_PREASM_H

typedef struct Mcr {
    char *name;
    char *code;
} Mcr;

typedef struct Node {
    struct Mcr data;
    struct Node *next;
} Node;

typedef struct List {
    int count;
    Node *head;
} List;


#endif //PROJECT_MAMAN_14_PREASM_H
