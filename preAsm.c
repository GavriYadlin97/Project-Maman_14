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
    free(code);
    return newNode;
}

/*Checks if it is a definition of a macro "mcr" and if so return success
 * the function receives two pointers of string, line and copy of line*/
error is_mcr_def( char* lineOut) {
    char *word;
    getToken(&lineOut, &word, " ");
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


error preAssembler(char* fileName){
    FILE *fpAm, *fileSrc;
    char *line,*name=NULL,*linecpy,*code="";
    int i;
    char *newFileName, *codemcr;


    /*Building a linked list of macros*/
    ListMcr * mcrList = calloc(1,sizeof (ListMcr));
    checkAlloc(mcrList);
    NodeMcr * newNode = (NodeMcr *) malloc(sizeof (NodeMcr));
    checkAlloc(newNode);

    openFile(&fileSrc,fileName, ".as");
    createFile(&fpAm,fileName,".am");
    while (!feof(fileSrc)){
        if( (getOneLine(&line,fileSrc))== success)
        {
            linecpy=(char *) malloc(sizeof (char)*LINE_MAX_LENGTH);
            checkAlloc(linecpy);
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
                //fputs("\n",fpAm);
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
                freeString(&linecpy);
                getOneLine(&line,fileSrc);
                codemcr = calloc(1, sizeof(char ));


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
                fputs("\n",fpAm);
                fflush(fpAm);
                freeString(&line);
                /*freeString(&linecpy);*/
            }
        }
        else break;
    }
    fputs(line, fpAm);
    fputs("\n",fpAm);
    fclose(fileSrc);
    fclose(fpAm);
    clearList(mcrList,NULL);
    freeString(&line);
    /*freeString(&linecpy);*/
    freeString(&name);
    free(mcrList);
    return success;
}


char* concatenateStrings(char* str1, char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char *result = malloc(len1 + len2 + 4);
    /*strlen("\t\t")+strlen("\n")+strlen("\0") = 4*/
    checkAlloc(result);
    str2 = removeWhiteSpace(str2);
    strcpy(result, str1);
    strcat(result, "\t\t");
    strcat(result, str2);
    strcat(result,"\n");
    free(str1);
    free(str2);
    return result;
}
