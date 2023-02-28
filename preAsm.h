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

char* concatenateStrings(char* str1, char* str2);
void addToList(Node* newElement, List* list);
struct Node* createNode(char * name, char* code);
error is_mcr_def( char* lineOut);
error is_mcrEnd(char *line);
error is_name_of_mcr(char* line,List* mcrList,char* code);
error preAssembler(FILE* fileSrc,char* fileName);
char* concatenateStrings(char* str1, char* str2);

#endif //PROJECT_MAMAN_14_PREASM_H
