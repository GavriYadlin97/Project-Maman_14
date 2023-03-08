#include "asmSecondRun.h"
#include "mainHeader.h"
#include "asmFirstRun.h"

error secondRun(list* dataList, list* labelList, list* instructionList, FILE* fileSrc,char* fileName){
    Node * currentNode= calloc(1,sizeof (Node));
    Node * nodeOut= calloc(1,sizeof (Node));
    int i;
    char * newFileName;
    FILE * fpObj,*fpEnt,*fpExt;
    error errFlag=success;
    currentNode=instructionList->head;
    for (i=0;i<=instructionList->count;i++){
        /*If it's a label and it's not an encoded line*/
        if(strcmp(currentNode->data.name,"")){
            if(searchNode(labelList, currentNode->data.name, nodeOut)==labelExists){
                strcmp(currentNode->data.InstructionCode , nodeOut->data.InstructionCode);
                currentNode= currentNode->next;
            }
            else
                errFlag =missingLabel;
                currentNode= currentNode->next;
        }
        else
            currentNode= currentNode->next;
    }
    /*There were no errors*/
    if(errFlag==success) {
        insertSuffix(fileName, &newFileName, ".obj");
        fpObj = fopen(newFileName, "w");
        /*write the instruction codes into new file ".obj"*/
        currentNode = instructionList->head;
        for (i = 0; i <= instructionList->count; i++) {
            fputs(currentNode->data.InstructionCode, fpObj);
            fflush(fpObj);
            currentNode = currentNode->next;
        }

        currentNode = dataList->head;
        for (i = 0; i <= dataList->count; i++) {
            fputs(currentNode->data.InstructionCode, fpObj);
            fflush(fpObj);
            currentNode = currentNode->next;
        }


        insertSuffix(fileName, &newFileName, ".ent");
        fpEnt = fopen(newFileName, "w");

        insertSuffix(fileName, &newFileName, ".ext");
        fpExt = fopen(newFileName, "w");

        currentNode = labelList->head;
        for (i = 0; i <= labelList->count; i++) {
            if (currentNode->data.type == entry) {
                fputs(currentNode->data.InstructionCode, fpEnt);
                fflush(fpEnt);
                currentNode = currentNode->next;
            } else if (currentNode->data.type == external) {
                fputs(currentNode->data.InstructionCode, fpExt);
                fflush(fpExt);
                currentNode = currentNode->next;
            }
        }
    }
}