/* Created by Gavri.y & Adi.s 28/01/2023. */

#ifndef WORD
#define WORD 15
#endif

#ifndef PROJECT_MAMAN_14_ASMFIRSTRUN_H
#define PROJECT_MAMAN_14_ASMFIRSTRUN_H

#define PRM_1_START 0
#define PRM_2_START 2
#define OPCODE_START 4
#define SRC_OP_START 8
#define DEST_OP_START 10
#define ARE_START 12

typedef enum AddressMethod{
    immediate,
    direct,
    jumpWP,
    directRegister,
}addressMethod;

typedef struct Code {
    char name [LABEL_MAX_SIZE];
    opcode type;
    int place;
    char InstructionCode[WORD];
} Code;

typedef struct Node {
    Code data;
    struct Node *next;
} Node;

typedef struct List {
    int count;
    Node *head;
} list;

error clearWhiteSpace(char **line);
error firstRun (char *);
void reverseSTR(char str[]);

#endif //PROJECT_MAMAN_14_ASMFIRSTRUN_H
