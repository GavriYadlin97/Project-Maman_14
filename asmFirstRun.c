/* Created by Adi.s & Gavri.y 22/02/2023. */

#include "mainHeader.h"
#include "asmFirstRun.h"

void binaryOf(char binaryForm[], unsigned num) {
    int i = 0;
    while (num > 0) {
        if (num % 2 == 0)
            binaryForm[i] = '.';
        else
            binaryForm[i] = '/';
        num /= 2;
        i++;
    }
    while (i<WORD)
        binaryForm[i++] = '.';
    binaryForm[WORD - 1] = '\0';
    reverseSTR(binaryForm);
}

void reverseSTR(char str[]) {
    int a = 0, b = 0;
    char temp;
    while (b < WORD && str[b + 1] != '\0')
        b++;
    while (b >= a) {
        temp = str[a];
        str[a] = str[b];
        str[b] = temp;
        a++;
        b--;
    }
}

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
    if (!strcmp(command, ".extern"))
        *op = external;
    if (*op == none)
        return undefinedCommand;
    return success;
}

static error addToList(list *lst, Node *nodeToAdd) {
    nodeToAdd->next = NULL;
    Node *ptr = lst->head;
    if (ptr) {
        while (ptr->next)
            ptr = ptr->next;
        ptr->next = nodeToAdd;
    } else lst->head = nodeToAdd;
    lst->count++;
    return success;
}

static error clearList (list *lst, Node **node) {
    if (node && *node) {
        if ((*node)->next)
            clearList(lst, &((*node)->next));
        free(*node);
        *node = NULL;
        lst->count--;
    } else if (lst->head)
        clearList(lst, &(lst->head));
    return success;
}

error codeData(char *line, list *dataList, int *counter) {
    char *word, *endPTR;
    int num = 0;
    Node *newNode;
    getToken(&line, &word, ",\n");
    while (word != NULL) {
        num = strlen(word);
        clearWhiteSpace(&word);
        if (num != strlen(word))
            return missingComma;
        num = strtol(word, &endPTR, 10);
        if (strcmp(endPTR, ""))
            return unknownArg;
        newNode = (Node *) malloc(sizeof(Node));
        binaryOf(newNode->data.InstructionCode, num);
        addToList(dataList, newNode);
        (*counter)++;
        free(word);
        getToken(&line, &word, ",\n\0");
        if (!word) {
            word = line;
            line = NULL;
        }
    }
    return success;
}

error codeString(char *line, list *dataList, int *counter) {
    Node *newNode, *OGNode = dataList->head;
    int i = 0;
    char code[WORD], ch = line[i];
    if (line == NULL)
        return emptyArg;
    while (OGNode->next)
        OGNode = OGNode->next;
    while (ch != '\0' && ch != '\"')
        ch = line[i++];
    if (ch == '\0')
        return missingParentheses;
    ch = line[++i];
    while (ch != '\0' && ch != '\"') {
        newNode = (Node *) malloc(sizeof(Node));
        binaryOf(newNode->data.InstructionCode, line[i]);
        addToList(dataList, newNode);
        (*counter)++;
        ch = line[++i];
    }
    newNode = (Node *) malloc(sizeof(Node));
    binaryOf(newNode->data.InstructionCode, '\0');
    addToList(dataList, newNode);
    counter++;
    if (ch != '\"') {
        counter -= dataList->count;
        clearList(dataList, &OGNode);
        counter += dataList->count;
        return missingClosingParentheses;
    }
    return success;
}

error codeLabel(char *label,list *labelList, int *count, opcode op) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (!label)
        return emptyArg;
    if (!newNode)
        return memoryAllocErr;
    newNode->data.type = op;
    newNode->data.place = (*count)++;
    strcpy(newNode->data.name, label);
    addToList(labelList, newNode);
    return success;
}

error codeCommand (char *line, list instructionList, int *count) {
    addressMethod am1, am2;
    char *label, *arg1, *arg2;
    getToken(&line, &label, "(");
    getToken(&line, &arg1, ",");
    if (label)
        getToken(&line, &arg2, ")");
    else
        getToken(&line, &arg2, NULL);
    idArg(arg1,&am1);
    idArg(arg2,&am2);
}

error idArg(char *arg, addressMethod *amArg) {

    clearWhiteSpace(&arg);
    if (arg[0] == '#')
        *amArg = immediate;
    else if (strlen(arg) == 2 && arg[0] == 'r' && '0' <= arg[1] && arg[1] <= '7')
        *amArg=directRegister;
    else
        return success;

}

error firstRun (char *path) {
    error errFlag;
    opcode commandCode;
    list dataList = {0};
    list labelList = {0};
    list instructionList = {0};
    addressMethod AMarg;
    FILE *stream;
    int IC = 0;
    int DC = 0;
    char *label, *word, *filepath;
    char *line = (char *) malloc(LINE_MAX_LENGTH);
    if (!line)
        return memoryAllocErr;
    if (!filepath)
        return memoryAllocErr;
    insertSuffix(path, &filepath, ".am");
    openFile(&stream, filepath);
    getOneLine(&line, stream);
    removeComments(&line);
    getToken(&line, &label, ":"); /*Get label if exists*/
    getToken(&line, &word, " \t\n"); /*Get first word of line*/
    idCommand(word, &commandCode); /*Identify the command and assign an enum value */
    switch (commandCode) {
        case none:
            errFlag = undefinedCommand;
            break;
        case data:
            codeData(line, &dataList, &DC);
            codeLabel(label, &labelList, &DC, commandCode);
            break;
        case string:
            codeString(line, &dataList, &DC);
            codeLabel(label, &labelList, &DC, commandCode);
            break;
        case external:
        case entry:
            errFlag = meaninglessLabel;
            free(label);
            getToken(&line, &label, ",\n");
            codeLabel(label, &labelList, &DC, commandCode);
            DC--;
            break;
        case mov:
        case cmp:
        case add:
        case sub:
        case lea:

        case not:
        case clr:
        case inc:
        case dec:
        case jmp:
        case bne:
        case red:
        case prn:
        case jsr:

        case rts:
        case stop:
            codeCommand(line, instructionList, &IC);
            break;
    }
    while (line) {
        free(word);
        getToken(&line, &word, "(,);\n");
    }
}
