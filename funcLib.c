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

/* '*str' and '*token' MUST be freed by the caller!
 * Function receives pointer to:
 * A source string (pointer to char *)
 * An empty Token string (pointer to char *) must be unallocated
 * A list of deliminators (string)
 * Function returns the first token found, and removes if from the source string
 * if none of the delim was found, source is unchanged,token will be NULL */
error getToken(char **str, char **token, char *delim) {
    int i = 0;
    char *chAfterTok;
    /*Check that all parameters are good*/
    if (!delim || !str || !(*str) || !token)
        return emptyArg;
    /*Skip white spaces from the beginning of the string*/
    while (isspace((*str)[i]))
        i++;
    /*If reached the end of the string*/
    if ((*str)[i] == '\0') {
        free(*str);
        *str = NULL;
        return emptyArg;
    }
    /*If deliminator is found. Else, deliminator wasn't found*/
    if ((chAfterTok = strpbrk(*str + i, delim))) {
        *(chAfterTok++) = '\0'; /*put '\0' then increase by one*/
        /*Prepare a place for the token, size of that ever is left in '*str' +1 */
        *token = (char *) malloc(chAfterTok - (*str + i));
        if (!*token)
            return memoryAllocErr;
        /*Copy token*/
        memcpy(*token, *str + i, chAfterTok - (*str + i));
        /*Remove token from str by overwriting it*/
        memmove(*str, chAfterTok, strlen(chAfterTok) + 1);
        *str = realloc(*str, strlen(*str) + 1); /*free unneeded space from "tail"*/
        if (!(*str))
            return memoryAllocErr;
    } else  /*Leave str as is*/
        *token = NULL;
    return success;
} /*Caller MUST free '*token' and '*str' */