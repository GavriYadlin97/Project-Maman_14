/* Created by Adi.s & Gavri.y 30/01/2023. */

#include "mainHeader.h"

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
    return;
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
            *line_out=buffer;
            free(buffer);
            return endOfFile;
        }
        else if (current == '\n') {
            buffer[bytes_readen] = '\0';
            *line_out=buffer;
            free(buffer);
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
error is_mcr_def(char * line, char* lineOut){
    char * word;
    strcpy(&lineOut,line);
    getToken(&lineOut,&word," ");
    if(!strcmp(word,"mcr")){
        return success;
    }
    else return noMcr;
}

/*Checks if it is a definition of an end of macro "endmcr" and if so return success
 * the function receives pointer of string line */
error is_mcrEnd(char *line){
    char* linecpy;
    char * word;
    strcpy(linecpy,line);
    getToken(&linecpy,&word," ");
    if(!strcmp(word,"endmcr")){
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
    Node * currentNode;
    if (mcrList->count==0)
        return noMcr;
    else {
        strcpy(linecpy,line);
        getToken(&linecpy,&word," ");
        for(i=0;i<= mcrList->count;i++){
            if(!strcmp(word,currentNode->data.name)){
                code= currentNode->data.code;
                return success;
            }
            currentNode=currentNode->next;
        }
    }
    return noMcr;
}
/* the function return the name of the macro, the caller will call only if found definition of macro*/
char* getNameOfMcr(char* line,char * name){
    getToken(&line,&name," ");
}


error preAssembler(FILE* fileSrc,char* fileName){
    FILE *fpSrc;
    FILE *fpAm;
    char ** line,*code, **linecpy,*name;
    int error=0;
    Node* ptrmcr;
    line= (char **) malloc(sizeof (char)*LINE_MAX_LENGTH);
    linecpy=(char **) malloc(sizeof (char)*LINE_MAX_LENGTH);

    /*Building a linked list of macros*/
    List * mcrList = calloc(1,sizeof (List));
    if(!mcrList){
        return noMemory;
    }
    Node * newNode = (Node*) malloc(sizeof (Node));
    if(!newNode){
        return noMemory;
    }

    //fpSrc= fopen(fileSrc,"r");
    fpAm= fopen(fileName,"w");
    while (!feof(fileSrc)){
        if(!(error = getOneLine(&line,fileSrc)))
        {
            /*checks name of macro */
            if(!is_name_of_mcr(line,mcrList,code)){
                free(line);
                fputs(code,fpAm);
                free(code);
            }
            /*checks definition of macro "mcr"*/
            if(!is_mcr_def(line,linecpy)){
                getNameOfMcr(line,name);
                ptrmcr->data.name=name;
                addToList(ptrmcr,mcrList);
                free(line);
                free(linecpy);
                free(name);
                getOneLine(&line,"");
                /*copying code to the node of the list until "endmcr"*/
                while (!is_mcrEnd(line)){
                    strcpy(ptrmcr->data.code,line);
                }
            }
            else /*not macro definition or macro name*/
            {
                fputs(line, fpAm);
                continue;
            }
        }
    }
}


