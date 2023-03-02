/* Created by Gavri.y & Adi.s 28/01/2023. */

#ifndef WORD
#define WORD 15
#endif

#ifndef PROJECT_MAMAN_14_ASMFIRSTRUN_H
#define PROJECT_MAMAN_14_ASMFIRSTRUN_H

#define PRM_1_START 0
#define REGI_1_START 0
#define PRM_2_START 2
#define OPCODE_START 4
#define REGI_2_START 6
#define SRC_AM_START 8
#define DEST_AM_START 10
#define ARE_START 12

typedef enum AddressMethod {
    immediate = 0,
    direct = 1,
    jumpWP = 2,
    directRegister = 3,
} addressMethod;

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
void reverseSTR(char str[], int);
error idArg(char *arg, addressMethod *AMarg);
struct Node* createNodeFirstRun(char * name, opcode opcode, int place, char* instructionCode);

#endif //PROJECT_MAMAN_14_ASMFIRSTRUN_H
