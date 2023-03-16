/* Created by Gavri.y & Adi.s 28/01/2023. */
#include "mainHeader.h"
#include "assmbler.h"
#include "preAsm.h"

int main(int argc, char *argv[]) {
    int i;
    for (i = 1; i < argc; i++) {
        preAssembler(argv[i]);
        firstRun(argv[i]);
    }
    return 0;
}