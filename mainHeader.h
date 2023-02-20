/* Created by Gavri.y & Adi.s 28/01/2023. */

/*Please include this header in EVERY .c file*/

#ifndef PROJECT_MAMAN_14_MAINHEADER_H
#define PROJECT_MAMAN_14_MAINHEADER_H

/*Libraries that will be used wherever the header is included*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*Various defines*/
#define LINE_MAX_LENGTH 81
#define TRUE 1
#define FALSE 0


/*Enum for various error handling*/
typedef enum Error{
    success,
    fileOpeningErr,
    fileClosingErr,
    emptyArg,
    memoryAllocErr,
    noMemory,
    endOfFile,
    noMcr,
}error;

/*Enum for opcodes */
typedef enum Opcode{
    mov,cmp,add,sub,not,clr,lea,inc,dec,jmp,bne,red,prn,jsr,rts,stop,
}opcode;

typedef struct Mcr{
    char* name;
    char * code;
}Mcr;

typedef struct Node{
    struct Mcr data;
    struct Node* next;
}Node;

typedef struct List{
    int count;
    Node* head;
}List;





/*Function header in main.c or funcLib.c only*/
error openFile(FILE **filePointer, char *filePath);
error createFile(FILE **filePointer, char *filePath);
error closeFile(FILE *filePointer);
error getToken(char **str, char **token, char *delim);
error preAssembler(FILE* fileSrc,char* fileName);
char* concatenateStrings(char* str1, char* str2);

#endif /*PROJECT_MAMAN_14_MAINHEADER_H*/
