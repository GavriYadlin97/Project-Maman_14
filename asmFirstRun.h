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

typedef enum ARE {
    absolute = 0,
    externaly = 1,
    relocate = 2,
} AREcode;

typedef enum AddressMethod{
    immediate,
    direct,
    jumpWP,
    directRegister,
}addressMethod;


typedef struct Label {
    char *name;
    char *value;
    AREcode type;
} Label;

typedef struct Code {
    char *name;
    int place;
    char InstructionCode[WORD];
    AREcode addressing;
} code;

typedef struct Node {
    code data;
    struct Node *next;
} node;

typedef struct List {
    int count;
    node *head;
} list;

error clearWhiteSpace(char **line);

#endif //PROJECT_MAMAN_14_ASMFIRSTRUN_H
