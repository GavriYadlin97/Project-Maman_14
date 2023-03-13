/* Created by Adi.s & Gavri.y 30/01/2023. */
#include "mainHeader.h"
#include "preAsm.h"

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

/*add one node to head of the list*/
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

/* Function to create a new node*/
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
    return newNode;
}

/*Checks if it is a definition of a macro "mcr" and if so return success
 * the function receives two pointers of string, line and copy of line*/
error is_mcr_def( char* lineOut){
    char * word;
    getToken(&lineOut,&word," ");
    if(!strcmp(word,"mcr")){
        return success;
    }
    else{
        //free(lineOut);
        return noMcr;
    }
}

/*Checks if it is a definition of an end of macro "endmcr" and if so return success
 * the function receives pointer of string line */
error is_mcrEnd(char *line){
    char* linecpy;
    linecpy= removeWhiteSpace(line);
    if(!strcmp(linecpy,"endmcr\n")){
        return success;
    }
    else return noMcr;
}

/* function checks if it is a name of macro
 * the function receives string of line, list of macros , and string of code
 * if found name of macro, the string of code receive the code of the matching macro*/
error is_name_of_mcr(char* line,ListMcr * mcrList,char* code){
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


error preAssembler(FILE* fileSrc,char* fileName){
    FILE *fpAm;
    char *line,*name=NULL,*linecpy,*code="";
    int i;
    char *newFileName;
    openFile(&fileSrc,fileName, ".as");
    insertSuffix(fileName,&newFileName,".am");

    line= (char *) malloc(sizeof (char)*LINE_MAX_LENGTH);

    /*Building a linked list of macros*/
    ListMcr * mcrList = calloc(1,sizeof (ListMcr));
    if(!mcrList){
        return noMemory;
    }
    NodeMcr * newNode = (NodeMcr *) malloc(sizeof (NodeMcr));
    if(!newNode){
        return noMemory;
    }

    fpAm= fopen(newFileName,"w+");
    while (!feof(fileSrc)){
        if( (getOneLine(&line,fileSrc))== success)
        {
            linecpy=(char *) malloc(sizeof (char)*LINE_MAX_LENGTH);
            strcpy(linecpy,line);

            /*checks name of macro */
            if(!is_name_of_mcr(linecpy,mcrList,code)){
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
            else if(!is_mcr_def(linecpy)){
                name= malloc(sizeof (char )* strlen(linecpy)+1);
                strcpy(name, linecpy);
                freeString(&line);
                free(linecpy);
                linecpy=NULL;
                getOneLine(&line,fileSrc);
                char * codemcr="";

                /*copying code to the node of the list until "endmcr"*/
                while ((is_mcrEnd(line))!= success){
                    codemcr = concatenateStrings(codemcr, line);
                    getOneLine(&line,fileSrc);
                }
                newNode= createNode(name,codemcr);
                addToList(newNode,mcrList);
                codemcr=NULL;
                freeString(&line);
            }
            else /*not macro definition or macro name*/
            {
                fputs(line, fpAm);
                fflush(fpAm);
                freeString(&line);
                freeString(&linecpy);
            }
        }
        else break;
    }
    fputs(line, fpAm);
    fflush(fpAm);
    fclose(fileSrc);
    fclose(fpAm);
    clearList(mcrList,NULL);
    freeString(&line);
    freeString(&linecpy);
    freeString(&name);
    free(mcrList);
    return success;
}


char* concatenateStrings(char* str1, char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char* result = malloc(len1 + len2 + 2);
    if (result == NULL) {
        printf("Error: Out of memory.\n");
        exit(1);
    }
    str2=removeWhiteSpace(str2);
    strcpy(result, str1);
    strcat(result,"\t\t");
    strcat(result, str2);

    return result;
}
