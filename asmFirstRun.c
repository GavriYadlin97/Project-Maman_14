/* Created by Adi.s & Gavri.y 22/02/2023. */

#include "mainHeader.h"
#include "asmFirstRun.h"

/* function takes a pointer to a string (char*) which is dynamically allocated, and five unallocated pointers to string (char*)
 * function divides the given string to Label, command and two arguments (depending on each existence) */
error clearWhiteSpace(char **line) {
    int i = 0;
    while ((*line)[i] != '\0') {
        if (isspace((*line)[i])) {
            (*line)[i] = '\0';
            strcat(*line, *line + i + 1);
        } else
            i++;
    }
    *line = (char *) realloc(*line, strlen(*line) + 1);
    if (!(*line))
        return memoryAllocErr;
    return success;
}

error divideLine(char **line, char **label, char **command, char **arg1, char **arg2) {
    error err;
    if ((err = getToken(line, label, ":")) != success) {
        command = NULL;
        arg1 = NULL;
        arg2 = NULL;
        return err;
    }
    if ((err = getToken(line, command, " \t\n")) != success) {
        arg1 = NULL;
        arg2 = NULL;
        return err;
    }
    /*in case the command is like: jmp LOOP(r3,#-6)*/
    if ((err = getToken(line, label, "(")) != success) {
        arg1 = NULL;
        arg2 = NULL;
        return err;
    }
    if ((err = getToken(line, arg1, ",\"\n")) != success) {
        arg2 = NULL;
        return err;
    }
    if ((err = getToken(line, arg2, ";)\"\n")) != success)
        return err;
    return success;
}

error idCommand(char *command, opcode *op) {
    if (!command)
        return emptyArg;
    *op = none;
    if (!strcmp(command, "mov"))
        *op = mov;
    if (!strcmp(command, "cmp"))
        *op = cmp;
    if (!strcmp(command, "add"))
        *op = add;
    if (!strcmp(command, "sub"))
        *op = sub;
    if (!strcmp(command, "not"))
        *op = not;
    if (!strcmp(command, "clr"))
        *op = clr;
    if (!strcmp(command, "lea"))
        *op = lea;
    if (!strcmp(command, "inc"))
        *op = inc;
    if (!strcmp(command, "dec"))
        *op = dec;
    if (!strcmp(command, "jmp"))
        *op = jmp;
    if (!strcmp(command, "bne"))
        *op = bne;
    if (!strcmp(command, "red"))
        *op = red;
    if (!strcmp(command, "prn"))
        *op = prn;
    if (!strcmp(command, "jsr"))
        *op = jsr;
    if (!strcmp(command, "rts"))
        *op = rts;
    if (!strcmp(command, "stop"))
        *op = stop;
    if (!strcmp(command, ".data"))
        *op = data;
    if (!strcmp(command, ".string"))
        *op = string;
    if (!strcmp(command, ".entry"))
        *op = entry;
    if (!strcmp(command, ".external"))
        *op = external;
    if (*op == none)
        return undefinedCommand;
    return success;
}

error idArg(char **arg, addressMethod *AMarg) {
    clearWhiteSpace(arg);
    int num = strcmp(*arg, "r0") * strcmp(*arg, "r1") * strcmp(*arg, "r2") * strcmp(*arg, "r3") * strcmp(*arg, "r4")
              * strcmp(*arg, "r5") * strcmp(*arg, "r6") * strcmp(*arg, "r7");
    if (num == 0) {
        *AMarg = directRegister;
        return success;
    }
    if ((*arg)[0] == '#') {
        *AMarg = direct;
        return success;
    }
    return undefinedAddressing;
}

error codeCommand(opcode op, addressMethod AMarg1, addressMethod AMarg2, node *newNode) {
    char code[WORD] = "..............";
    newNode = (node *) calloc(1, sizeof(node));
    if (!newNode)
        return memoryAllocErr;
    switch (op) {
        case none:
            return undefinedCommand;
        case mov:
            strncpy(code + 4, "....", strlen("...."));
            break;
        case cmp:
            strncpy(code + 4, ".../", strlen(".../"));
            break;
        case add:
            strncpy(code + 4, "../.", strlen("../."));
            break;
        case sub:
            strncpy(code + 4, "..//", strlen("..//"));
            break;
        case not:
            strncpy(code + 4, "./..", strlen("./.."));
            break;
        case clr:
            strncpy(code + 4, "././", strlen("././"));
            break;
        case lea:
            strncpy(code + 4, ".//.", strlen(".//."));
            break;
        case inc:
            strncpy(code + 4, ".///", strlen(".///"));
            break;
        case dec:
            strncpy(code + 4, "/...", strlen("/..."));
            break;
        case jmp:
            strncpy(code + 4, "/../", strlen("/../"));
            break;
        case bne:
            strncpy(code + 4, "/./.", strlen("/./."));
            break;
        case red:
            strncpy(code + 4, "/.//", strlen("/.//"));
            break;
        case prn:
            strncpy(code + 4, "//..", strlen("//.."));
            break;
        case jsr:
            strncpy(code + 4, "//./", strlen("//./"));
            break;
        case rts:
            strncpy(code + 4, "///.", strlen("///."));
            break;
        case stop:
            strncpy(code + 4, "////", strlen("////"));
            break;
        case data:
        case string:
        case entry:
        case external:
            break;
    }
    switch (AMarg1) {
        case immediate:
            strncpy(code + SRC_OP_START, "..", strlen(".."));
            break;
        case direct:
            strncpy(code + SRC_OP_START, "./", strlen("./"));
            break;
        case directRegister:
            strncpy(code + SRC_OP_START, "//", strlen("//"));/*break isn't needed*/
        default:
            break;
    }
    switch (AMarg2) {
        case immediate:
            strncpy(code + DEST_OP_START, "..", strlen(".."));
            break;
        case direct:
            strncpy(code + DEST_OP_START, "./", strlen("./"));
            break;
        case jumpWP:
            strncpy(code + DEST_OP_START, "/.", strlen("/."));
            break;
        case directRegister:
            strncpy(code + DEST_OP_START, "//", strlen("//"));
            break;
    }
}
