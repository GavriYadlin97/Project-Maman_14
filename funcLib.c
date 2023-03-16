/* Created by Adi.s & Gavri.y 30/01/2023. */

#include "mainHeader.h"


void freeString(char** ptr){
    if(*ptr){
        free(*ptr);
        *ptr=NULL;
    }
}

/*remove white spaces from the beginning of the string
 * received pointer to string
 * return pointer to string*/
char* removeWhiteSpace(char* str) {
    int len = (int) strlen(str), i;
    char *result = str;
    for (i = 0; i < len; i++) {
        if (isspace(str[i]))
            result++;
        else
            break;
    }
    memmove(str, result, strlen(result) + 1);
    return str;
}

void checkAlloc (void *test) {
    if (!test) {
        perror("Memory allocation error");
        exit(1);
    }
}

/*Frees several pointers, sets them to Null, called with address of the pointer:
 * LAST ONE MUST BE NULL: freeMulti(&a,&b,&c,NULL);*/
void freeMulti(void *ptr, ...) {
    void **nextPtr = ptr;
    va_list args;
    va_start(args, ptr);
    while (nextPtr != NULL) {
        free(*nextPtr);
        *nextPtr = NULL;
        nextPtr = va_arg(args, void **);
    }
    va_end(args);
}

/* Function receives a pointer to a FILE pointer and a file path (string)
 * The function opens the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error openFile (FILE **filePointer, char *filePath, char *suffix) {
    char *path = (char *) malloc(strlen(filePath) + strlen(suffix) + 1);
    checkAlloc(path);
    strcpy(path, filePath);
    strcat(path, suffix);
    *filePointer = fopen(path, "r");
    free(path);
    if (!*filePointer) {
        perror("File opening error");
        return fileOpeningErr;
    }
    return success;
}

/* Function receives a pointer to a FILE pointer and a file path (string)
 * The function creates the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error createFile (FILE **filePointer, char *filePath, char *suffix) {
    char *path = (char *) malloc(strlen(filePath) + strlen(suffix) + 1);
    checkAlloc(path);
    strcpy(path, filePath);
    strcat(path, suffix);
    *filePointer = fopen(path, "w");
    free(path);
    if (!*filePointer) {
        perror("File creating error");
        return fileOpeningErr;
    }
    return success;
}

/* Function receives a FILE pointer
 * The function closes the file and alerts if any errors occurred
 * returns an error code represented as an error enum*/
error closeFile (FILE *filePointer) {
    return ((fclose(filePointer) == EOF) ? fileClosingErr : success);
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
    /*Skip white spaces from the beginning of the string*/
    while (isspace((*str)[i]))
        i++;
    /*if delim is null, copy entire string to token*/
    if (!delim) {
        if (strcmp(*str, "")) {
            *token = (char *) malloc(strlen(*str + i) + 1);
            checkAlloc(token);
            strcpy(*token, *str + i);
            free(*str);
            *str = NULL;
        } else
            *token = NULL;
        return success;
    }
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
        *token = (char *) calloc(chAfterTok - (*str + i), sizeof(char));
        checkAlloc(*token);
        /*Copy token*/
        memcpy(*token, *str + i, chAfterTok - (*str + i));
        /*Remove token from str by overwriting it*/
        memmove(*str, chAfterTok, strlen(chAfterTok) + 1);
        *str = (char *) realloc(*str, (strlen(*str) + 1));/*free unneeded space from "tail"*/
        checkAlloc(*str);
    } else  /*Leave str as is*/
        *token = NULL;
    return success;
} /*Caller MUST free '*token' and '*str' */


error removeComments(char **str) {
    char *ch;
    if (str == NULL || *str == NULL)
        return emptyArg;
    ch = strpbrk(*str, ";");
    if (!ch)
        return success;
    *ch = '\0';
    *str = (char *) realloc(*str, 1 + strlen(*str));
    checkAlloc(*str);
    return success;
}

/* get one line in a time, the function receives pointer to a string and pointer to a file,
 * takes one line from the file and copy it to the receiving string*/
error getOneLine(char **line_out, FILE * fp) {
    int bytes_readen = 0;
    char *buffer = (char *) malloc(LINE_MAX_LENGTH * sizeof(char));
    checkAlloc(buffer);
    while (1) {
        char current = (char) fgetc(fp);
        if (current == EOF || current == '\n' || current == '\r') {
            /*buffer[bytes_readen] = '\n';*/
            buffer[bytes_readen] = '\0';
            (*line_out) = strdup(buffer);
            free(buffer);
            return (current == EOF) ? endOfFile : success;
        } else
            buffer[bytes_readen++] = current;
    }
}