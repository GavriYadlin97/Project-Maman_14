/* Created by Adi.s & Gavri.y 30/01/2023. */

#include "mainHeader.h"

/* Function receives a pointer to a FILE pointer and a file path (string)
 * The function opens the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error openFile (FILE **filePointer, char *filePath) {
    *filePointer = fopen(filePath, "r");
    if (!*filePointer)
        return fileOpeningErr;
    return success;
}

/* Function receives a pointer to a FILE pointer and a file path (string)
 * The function creates the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error createFile (FILE **filePointer, char *filePath) {
    *filePointer = fopen(filePath, "w");
    if (!*filePointer)
        return fileOpeningErr;
    return success;
}

/* Function receives a FILE pointer
 * The function closes the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error closeFile (FILE *filePointer) {
    if (fclose(filePointer) == EOF)
        return fileClosingErr;
    return success;
}