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
#define MAX_MCR 256
#define TABLE_SIZE 100
#define WORD 15
#define LABEL_MAX_SIZE 31
#define SUFFIX_LENGTH 5

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
    undefinedCommand,
    undefinedAddressing,
    labelExists,
    labelTooLong,
    wrongDefLabel,
    meaninglessLabel,
    unknownArg,
    missingClosingParentheses,
    missingParentheses,
    missingComma,
    missingArg,
    tooManyArg,
    wrongArg,
    missingLabel,
}error;

/*Enum for opcodes */
typedef enum Opcode {
    mov = 0,
    cmp = 1,
    add = 2,
    sub = 3,
    not = 4,
    clr = 5,
    lea = 6,
    inc = 7,
    dec = 8,
    jmp = 9,
    bne = 10,
    red = 11,
    prn = 12,
    jsr = 13,
    rts = 14,
    stop = 15,
    data,
    string,
    entry,
    external,
    none,
} opcode;


/*Function header in main.c or funcLib.c only*/
error openFile(FILE **filePointer, char *filePath, char *suffix);
error createFile(FILE **filePointer, char *filePath);
error closeFile(FILE *filePointer);
error getToken(char **str, char **token, char *delim);
error getOneLine(char **line_out, FILE * fp);
error insertSuffix(char *str,char **newStr,char *suffix);
error removeComments(char **str);
char* removeWhiteSpace(char* str);
void freeString(char** ptr);
error checkAlloc (void *);



#endif /*PROJECT_MAMAN_14_MAINHEADER_H*/
