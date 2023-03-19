/* Created by Adi.s & Gavri.y 30/01/2023. */
#include "mainHeader.h"
#include "preAsm.h"

/*Function checks if definition of mac is a command, if so returns error "mcrNameIncorrect",
 * if empty returns emptyArg, else returns success, function received char* */
error isCommand(char *command) {
    if (!command)
        return emptyArg;
    if (!strcmp(command, "mov"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "cmp"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "add"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "sub"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "not"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "clr"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "lea"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "inc"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "dec"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "jmp"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "bne"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "red"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "prn"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "jsr"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "rts"))
        return mcrNameIncorrect;
    else if (!strcmp(command, "stop"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".data"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".string"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".entry"))
        return mcrNameIncorrect;
    else if (!strcmp(command, ".extern"))
        return mcrNameIncorrect;
    else
        return success;
}

/* function free list and nodes, the function recived ListMcr * and NodeMcr** of the head of list*/
static error clearList (ListMcr *lst, NodeMcr **node) {
    if (node && *node) {
        if ((*node)->next)
            clearList(lst, &((*node)->next));
        free((*node)->data.name);
        free((*node)->data.code);
        free(*node);
        *node = NULL;
        lst->count--;
    } else if (lst->head)
        clearList(lst, &(lst->head));
    return success;
}

/*add one node to head of the list
 * received NodeMcr* of the node to add and ListMcr* */
void addToList(NodeMcr * newElement, ListMcr * list){
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

/* Function to create a new node
 * received char* of name and char* of code
 * the caller must free at the end*/
struct NodeMcr * createNode(char * name, char* code) {
    /* Allocate memory for the new node*/
    struct NodeMcr * newNode = (struct NodeMcr*) malloc(sizeof(struct NodeMcr));
    if (newNode == NULL) {
        printf("Memory allocation failed");
        exit(1);
    }
    /* Allocate memory for the string data and copy the input string to it*/
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
    freeString(&code);
    return newNode;
}

/*Checks if it is a definition of a macro "mcr" and if so return success
 * the function receives pointer of string*/
error is_mcr_def( char **lineOut) {
    char *word;
    getToken(lineOut, &word, " ");
    if (!word)
        return emptyArg;
    if (!strcmp(word, "mcr")) {
        free(word);
        return success;
    } else {
        free(word);
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
 * the function receives string of line, list of macros
 * if found name of macro, return success*/
error is_name_of_mcr(char* line,ListMcr * mcrList){
    char* word;
    int i;
    NodeMcr * currentNode=mcrList->head;
    if (mcrList->count==0)
        return noMcr;
    else {
        word=removeWhiteSpace(line);

        for(i=0;i< mcrList->count;i++){
            if(!strcmp(word,currentNode->data.name)){
                return success;
            }
            currentNode=currentNode->next;
        }
    }
    return noMcr;
}


error preAssembler(char* fileName){
    FILE *fpAm=NULL, *fileSrc=NULL;
    char *line,*name=NULL,*linecpy;
    int i;
    char *codemcr;
    error err=success;

    /*Building a linked list of macros*/
    ListMcr * mcrList = calloc(1,sizeof (ListMcr));
    NodeMcr * newNode ;/*= (NodeMcr *) malloc(sizeof (NodeMcr));*/
    checkAlloc(mcrList);
    /*checkAlloc(newNode);*/

    openFile(&fileSrc,fileName, ".as");
    if(fileSrc==NULL) {
        fprintf(stderr, "unable to open file %s, continuing to next file...", fileName);
        return fileOpeningErr;
    }
    createFile(&fpAm,fileName,".am");
    while (!feof(fileSrc)){
        if( getOneLine(&line,fileSrc)== success)
        {
            linecpy=(char *) malloc(sizeof (char)*LINE_MAX_LENGTH);
            checkAlloc(linecpy);
            strcpy(linecpy,line);

            /*checks name of macro */
            if(!is_name_of_mcr(linecpy,mcrList)){
                NodeMcr * currentNode=mcrList->head;
                linecpy= removeWhiteSpace(linecpy);
                for(i=0;i< mcrList->count;i++){
                    if(!strcmp(linecpy,currentNode->data.name)) {
                        break;
                    }
                    currentNode = currentNode->next;
                }
                fputs(currentNode->data.code,fpAm);
                fflush(fpAm);
                freeString(&line);
                freeString(&linecpy);
                linecpy=NULL;
                continue;
            }
                /*checks definition of macro "mcr"*/
            else if(!is_mcr_def(&linecpy)){
                if(strIsAlphaDigit(linecpy)!=success){/*definition of macro with characters */
                    err = mcrNameIncorrect;
                    removeWhiteSpace(line);
                    fprintf(stderr,"Error: The definition of the macro \"%s\" is incorrect\n",line);
                    continue;
                }
                name= malloc(sizeof (char )* strlen(linecpy)+1);
                name ="";
                getToken(&linecpy, &name, " ");
                if(name){/* definition of macro with space*/
                    err = mcrNameIncorrect;
                    removeWhiteSpace(line);
                    fprintf(stderr,"Error: The definition of the macro \"%s\" is incorrect\n",line);
                }
                name= realloc(name,sizeof(char )* strlen(linecpy)+1);
                strcpy(name, linecpy);
                if(isCommand(name)== success) {/* definition of macro is not a command */
                    freeString(&line);
                    freeString(&linecpy);
                    getOneLine(&line, fileSrc);
                    codemcr = calloc(1, sizeof(char));


                    /*copying code to the node of the list until "endmcr"*/
                    while ((is_mcrEnd(line)) != success) {
                        codemcr = concatenateStrings(codemcr, line);
                        getOneLine(&line, fileSrc);
                    }
                    newNode = createNode(name, codemcr);
                    addToList(newNode, mcrList);
                    codemcr = NULL;
                    freeString(&line);
                    freeString(&name);
                }
                else {
                    err = mcrNameIncorrect;
                    fprintf(stderr, "Error: The definition of a macro %s cannot be a name of command\n",name);
                    freeString(&line);
                    freeString(&linecpy);
                    getOneLine(&line, fileSrc);
                    while ((is_mcrEnd(line)) != success){
                        freeString(&line);
                        getOneLine(&line, fileSrc);
                    }
                    freeString(&line);
                    freeString(&name);
                }
            }
            else /*not macro definition or macro name*/
            {
                fputs(line, fpAm);
                fputs("\n",fpAm);
                fflush(fpAm);
                freeString(&line);
                freeString(&linecpy);
            }
        }
        else break;
    }
    fputs(line, fpAm);
    fputs("\n",fpAm);
    fclose(fileSrc);
    fclose(fpAm);
    if(err!=success){
        removeFile(fileName,".am");
        fprintf(stderr,"cannot create file %s.am\n",fileName);
    }
    clearList(mcrList,NULL);
    freeString(&line);
    freeString(&linecpy);
    freeString(&name);
    free(mcrList);

    return err;
}

/*The function concatenates strings of macro code
 * recevied char * of str1 and str2,
 * str1 is a string of code we have and str2 is a string of code to add*/
char* concatenateStrings(char* str1, char* str2) {
    int len1 = (int) strlen(str1);
    int len2 = (int) strlen(str2);
    char *result = malloc(len1 + len2 + 4);

    checkAlloc(result);
    str2 = removeWhiteSpace(str2);
    strcpy(result, str1);
    strcat(result, "\t");
    strcat(result, str2);
    strcat(result, "\n");
    freeString(&str1);
    freeString(&str2);
    return result;
}
