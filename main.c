/* Created by Gavri.y & Adi.s 28/01/2023. */
#include "mainHeader.h"
#include "assmbler.h"
#include "preAsm.h"

int main(int argc, char *argv[]) {
    int i;
    error errFlag = success;
    for (i = 1; i < argc; i++) {
        if (preAssembler(argv[i]) == success)
            firstRun(argv[i]);
    }
    return 0;
}