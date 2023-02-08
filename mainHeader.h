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
}error;

/*Function header in main.c or funcLib.c only*/
error openFile(FILE **filePointer, char *filePath);
error createFile(FILE **filePointer, char *filePath);
error closeFile(FILE *filePointer);
error getToken(char **str, char **token, char *delim);


#endif /*PROJECT_MAMAN_14_MAINHEADER_H*/
