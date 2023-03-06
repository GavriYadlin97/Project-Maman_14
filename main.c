/* Created by Gavri.y & Adi.s 28/01/2023. */
#include "mainHeader.h"
#include "asmFirstRun.h"
#include "preAsm.h"
int main(int argc, char *argv[]) {
    FILE* pf;
    openFile(&pf,argv[1],".as");
    preAssembler(pf,argv[1]);
//    firstRun(argv[1]);


    return 0;
}
