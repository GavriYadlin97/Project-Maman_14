/* Created by Gavri.y & Adi.s 28/01/2023. */
#include "mainHeader.h"

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");
    char* line = (char*) malloc(sizeof (char )*LINE_MAX_LENGTH);
    line= "hello,world!";
    char** word= "";

    getToken(&line,word,",");

    char* fileName= argv[1];
    FILE* stream;
    openFile(&stream,argv[1]);
    strcat(fileName,".am");
    preAssembler(stream,fileName);
    return 0;
}
