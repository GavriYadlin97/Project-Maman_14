/* Created by Gavri.y & Adi.s 28/01/2023. */
#include "mainHeader.h"

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");
    char* line = (char*) malloc(sizeof (char )*LINE_MAX_LENGTH);
    //ברגע ששמת לו משפט כמו כאן, זה כבר string literal כלומר אסור לתוכנית לשנות אותו, צריך לעשות את זה עם קליטה של משתנה
    //line= "hello,world!";
    fgets(line,LINE_MAX_LENGTH,stdin);
    //חבל להתעסק עם dereferance אחר כך, כוכבית אחת זה מספיק (לשלוח לפונקצייה עם &) ולא לאתחל אותו לכלום
    // char ** word="";
    char* word;

    getToken(&line,&word,",");

    char* fileName= argv[1];
    FILE* stream;
    openFile(&stream,argv[1]);
    strcat(fileName,".am");
    preAssembler(stream,fileName);
    return 0;
}
