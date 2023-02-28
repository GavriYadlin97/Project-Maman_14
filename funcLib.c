/* Created by Adi.s & Gavri.y 30/01/2023. */

#include "mainHeader.h"
#include "preAsm.h"

/*add one node to head of the list*/
void addToList(Node* newElement, List* list){
    if(list->head == NULL)
    {
        list->head = newElement;
        list->count++;
        return;
    }
    newElement->next = list->head;
    list->head = newElement;
    list->count++;
}
// Function to create a new node
struct Node* createNode(char * name, char* code) {
    // Allocate memory for the new node
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed");
        exit(1);
    }
    // Allocate memory for the string data and copy the input string to it
    newNode->data.name = (char*) malloc((strlen(name) + 1) * sizeof(char));
    if (!newNode->data.name) {
        printf("Memory allocation failed");
        exit(1);
    }
    newNode->data.code = (char*) malloc((strlen(code) + 1) * sizeof(char));
    if (!newNode->data.code) {
        printf("Memory allocation failed");
        exit(1);
    }
    strcpy(newNode->data.name, name);
    strcpy(newNode->data.code,code);
    newNode->next = NULL;
    return newNode;
}

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
    if (!delim || !str || !(*str) || !token) {
        if (token)
            *token = NULL;
        if (str)
            str = NULL;
        return emptyArg;
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
    char * buffer = (char*)malloc(buffer_size*sizeof(char));
    if (buffer == NULL) {
        return noMemory;
    }

    while (1) {
        char current = fgetc(fp);
        if (current == EOF) {
            buffer[bytes_readen] = '\0';
            *line_out=buffer;
            return endOfFile;
        }
        else if (current == '\n') {
            buffer[bytes_readen] = '\0';
            *line_out=buffer;
            return success;
        }
        /* Assuming a line of no more than 80 characters
        else if (bytes_readen >= LINE_MAX_LENGTH - 1) {
            *buffer= (char) realloc(*buffer,(int)((int)buffer_size*sizeof(char) )*2);
            if (buffer == NULL) {
                return noMemory;
            }
        }*/
        else {
            buffer[bytes_readen++] = current;
        }
    }
}

/*Checks if it is a definition of a macro "mcr" and if so return success
 * the function receives two pointers of string, line and copy of line*/
error is_mcr_def( char* lineOut){
    char * word;
    //strcpy(lineOut,line);
    getToken(&lineOut,&word," ");
    if(!strcmp(word,"mcr")){
        return success;
    }
    else{
        free(lineOut);
        return noMcr;
    }
}

/*Checks if it is a definition of an end of macro "endmcr" and if so return success
 * the function receives pointer of string line */
error is_mcrEnd(char *line){
    char* linecpy;
    linecpy= removeWhiteSpace(line);
    if(!strcmp(linecpy,"endmcr")){
        return success;
    }
    else return noMcr;
}

/* function checks if it is a name of macro
 * the function receives string of line, list of macros , and string of code
 * if found name of macro, the string of code receive the code of the matching macro*/
error is_name_of_mcr(char* line,List* mcrList,char* code){
    char* linecpy=(char *) malloc(sizeof (char )*LINE_MAX_LENGTH);
    char* word;
    int i;
    Node * currentNode=mcrList->head;
    if (mcrList->count==0)
        return noMcr;
    else {
        strcpy(linecpy,line);
        word=removeWhiteSpace(linecpy);

        for(i=0;i< mcrList->count;i++){
            if(!strcmp(word,currentNode->data.name)){
                code= currentNode->data.code;
                return success;
            }
            currentNode=currentNode->next;
        }
    }
    return noMcr;
}


error preAssembler(FILE* fileSrc,char* fileName){
    FILE *fpAm;
    char *line,*name,*code="";
    int i;
    line= (char *) malloc(sizeof (char)*LINE_MAX_LENGTH);

    /*Building a linked list of macros*/
    List * mcrList = calloc(1,sizeof (List));
    if(!mcrList){
        return noMemory;
    }
    Node * newNode = (Node*) malloc(sizeof (Node));
    if(!newNode){
        return noMemory;
    }

    fpAm= fopen(fileName,"w+");
    while (!feof(fileSrc)){
        if( (getOneLine(&line,fileSrc))== success)
        {
            char* linecpy=(char *) malloc(sizeof (char)*LINE_MAX_LENGTH);
            strcpy(linecpy,line);

            /*checks name of macro */
            if(!is_name_of_mcr(linecpy,mcrList,code)){
                Node * currentNode=mcrList->head;
                linecpy= removeWhiteSpace(linecpy);
                for(i=0;i< mcrList->count;i++){
                    if(!strcmp(linecpy,currentNode->data.name)) {
                        break;
                    }
                    currentNode = currentNode->next;
                }

                free(line);
                code= strdup(currentNode->data.code);
                fputs(code,fpAm);
                continue;
            }
            /*checks definition of macro "mcr"*/
            else if(!is_mcr_def(linecpy)){
                name= malloc(sizeof (char )* strlen(linecpy)+1);
                strcpy(name, linecpy);
                free(line);
                free(linecpy);

                getOneLine(&line,fileSrc);
                char * codemcr="";
                /*copying code to the node of the list until "endmcr"*/
                while ((is_mcrEnd(line))!= success){

                    codemcr = concatenateStrings(codemcr, line);
                    getOneLine(&line,fileSrc);
                }
                newNode= createNode(name,codemcr);
                addToList(newNode,mcrList);
            }
            else /*not macro definition or macro name*/
            {
                fflush(fpAm);
                fputs(line, fpAm);
                fputs("\n",fpAm);

            }

        }
    }
    int len= strlen(line);
    strncat(line,line,len);
    fputs(line, fpAm);
    fclose(fpAm);
    return success;
}


char* concatenateStrings(char* str1, char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = malloc(len1 + len2 + 2);
    if (result == NULL) {
        printf("Error: Out of memory.\n");
        exit(1);
    }
    //str1=removeWhiteSpace(str1);
    str2=removeWhiteSpace(str2);
    strcpy(result, str1);
    strcat(result,"\t\t");
    strcat(result, str2);
    strcat(result,"\n");
    return result;
}