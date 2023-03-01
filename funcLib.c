/* Created by Adi.s & Gavri.y 30/01/2023. */

#include "mainHeader.h"

/*remove white spaces from the beginning of the string
 * received pointer to string
 * return pointer to string*/
char* removeWhiteSpace(char* str) {
    int len = (int) strlen(str), i;
    char *result = str;
    for (i = 0; i < len; i++) {
        if (isspace(str[i])) {
            result++;
        } else {
            break;
        }
    }
    return result;
}

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
    if (!str || !(*str) || !token) {
        if (token)
            *token = NULL;
        if (str)
            str = NULL;
        return emptyArg;
    }
    /*if delim is null, copy entire string to token*/
    if (!delim) {
        *token = (char *) malloc(strlen(*str) + 1);
        if (!*token)
            return memoryAllocErr;
        strcpy(*token, *str);
        free(*str);
        *str = NULL;
        return success;
    }
    /*Skip white spaces from the beginning of the string*/
    while (isspace((*str)[i]))
        i++;
    /*If reached the end of the string*/
    if ((*str)[i] == '\0') {
        free(*str);
        *str = NULL;
        *token = NULL;
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

error insertSuffix(char *str,char **newStr,char *suffix) {
    *newStr = (char *) malloc(strlen(str) + strlen(suffix) + 1);
    if (!*newStr)
        return memoryAllocErr;
    strcpy(*newStr, str);
    strcat(*newStr, suffix);
    return success;
}

error removeComments(char **str) {
    if (str == NULL || *str == NULL)
        return emptyArg;
    char *ch;
    ch = strpbrk(*str, ";");
    if (!ch)
        return success;
    *ch = '\0';
    *str = (char *) realloc(*str, 1 + strlen(*str));
    if (!(*str))
        return memoryAllocErr;
    return success;
}

/* get one line in a time, the function receives pointer to a string and pointer to a file,
 * takes one line from the file and copy it to the receiving string*/
error getOneLine(char **line_out, FILE * fp) {
    size_t buffer_size = LINE_MAX_LENGTH;
    int bytes_readen = 0;
    char *buffer = (char *) malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        return noMemory;
    }
    while (1) {
        char current = fgetc(fp);
        if (current == EOF) {
            buffer[bytes_readen] = '\n';
            buffer[++bytes_readen] = '\0';
            *line_out = buffer;
            return endOfFile;
        } else if (current == '\n') {
            buffer[bytes_readen] = '\0';
            *line_out = buffer;
            return success;
        } else {
            buffer[bytes_readen++] = current;
        }
    }
}