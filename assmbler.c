/* Created by Adi.s & Gavri.y 22/02/2023. */

#include "mainHeader.h"
#include "assmbler.h"

void binaryOf(char binaryForm[], unsigned num, int size) {
    int i = 0;
    while (num > 0 && i < size) {
        if (num % 2 == 0)
            binaryForm[i] = '.';
        else
            binaryForm[i] = '/';
        num /= 2;
        i++;
    }
    while (i < size)
        binaryForm[i++] = '.';
    reverseSTR(binaryForm, size);
}

void reverseSTR(char str[],int size) {
    int a = 0, b = 0;
    char temp;
    while (b < size - 1 && str[b + 1] != '\0')
        b++;
    while (b >= a) {
        temp = str[a];
        str[a] = str[b];
        str[b] = temp;
        a++;
        b--;
    }
}

/*Function receives char *
 * the functions checks if the string is alpha or number if so return success
 * if not returns an error code represented as an error enum*/
error strIsAlphaDigit(char* str){
    int i;

    for (i=0; str[i]!= '\0' ; i++) {
        if(!isalnum(str[i])){
            return wrongDefLabel;
        }
    }
    return success;
}

/* function takes a pointer to a string (char*) which is dynamically allocated, and five unallocated pointers to string (char*)
 * function divides the given string to Label, command and two arguments (depending on each existence) */
error clearWhiteSpace(char **line) {
    int i = 0;
    if (!line || !(*line))
        return emptyArg;

    while ((*line)[i] != '\0') {
        if (isspace((*line)[i])) {
            (*line)[i] = '\0';
            strcat(*line, *line + i + 1);
        } else
            i++;
    }
    *line = (char *) realloc(*line, sizeof(char) * (strlen(*line) + 1));
    checkAlloc(*line);
    if (!strcmp(*line, "")) {
        free(*line);
        *line = NULL;
    }
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


/* Function to create a new node*/
struct Node* createNodeFirstRun(char * name, opcode opcode, int place, char* instructionCode) {
    /* Allocate memory for the new node*/
    struct Node *newNode = (struct Node *) calloc(1, sizeof(struct Node));
    checkAlloc(newNode);
    strcpy(newNode->data.InstructionCode, instructionCode);
    strcpy(newNode->data.name, name);
    newNode->data.type = opcode;
    newNode->data.place = place;
    newNode->next = NULL;
    return newNode;
}

static error idLabel (char* arg) {
    if (strlen(arg) >= LABEL_MAX_SIZE) {
        fprintf(stderr, "\tLabel too long\n");
        return unknownArg;
    }
    if (strpbrk(arg, " \t") != NULL) {
        fprintf(stderr, "\tLabel cannot contain white spaces\n");
        return unknownArg;
    }
    if (!isalpha(arg[0])) {
        fprintf(stderr, "\tLabel must start with latter\n");
        return unknownArg;
    }
    if(strchr(arg,',')) {
        fprintf(stderr, "\tLabel or arg cannot contain \',\',\n");
        return unknownArg;
    }
    return success;
}

static error addToList(list *lst, Node **nodeToAdd) {
    Node *ptr = lst->head;
    (*nodeToAdd)->next = NULL;
    (*nodeToAdd)->next = NULL;
    if (ptr) {
        while (ptr->next)
            ptr = ptr->next;
        ptr->next = *nodeToAdd;
    } else lst->head = *nodeToAdd;
    lst->count++;
    *nodeToAdd = NULL;
    return success;
}

/*search node by name
 * checks only if the names are the same
 * received pointer to list and name (char*)*/
error searchNode(list* list, char* name,Node **nodeOut) {
    Node *currentNode = list->head;
    int i;
    if (list->count == 0)
        return success;
    else {
        for (i = 0; i < list->count; i++) {
            if (!strcmp(name, currentNode->data.name)) {
                *nodeOut = currentNode;
                return labelExists;
            }
            currentNode = currentNode->next;
        }
    }
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
    char *word = NULL, *endPTR = NULL;
    int num = 0, i;
    Node *newNode = NULL;
    getToken(&line, &word, ",");
    if (!word)
        getToken(&line, &word, NULL);
    while (word) {
        if (!word || !strcmp(word, "")) {
            fprintf(stderr, "\tConsecutive commas in .data command\n");
            return consecutiveCommas;
        }
        num = strtol(word, &endPTR, 10);
        if (strcmp(endPTR, "")) {
            for (i = 0; i < strlen(endPTR); ++i) {
                if (!isspace(endPTR[i])) {
                    fprintf(stderr, "\tAn unrecognized argument in .data, or missing comma(s)\n");
                    freeMulti(&word, &line, NULL);
                    return unknownArg;
                }
            }
        }
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        binaryOf(newNode->data.InstructionCode, num, sizeof(newNode->data.InstructionCode) - 1);
        newNode->data.place = (*counter)++;
        addToList(dataList, &newNode);
        free(word);
        getToken(&line, &word, ",");
        if (!word)
            getToken(&line, &word, NULL);
    }
    freeMulti(&word, &line, NULL);
    return success;
}

error codeString(char *line, list *dataList, int *counter) {
    Node *newNode, *OGNode = dataList->head;
    int i = 0;
    char ch;
    if (line == NULL)
        return emptyArg;
    ch = line[i];
    while (OGNode)
        OGNode = OGNode->next;
    while (ch != '\0' && ch != '\"') {
        if(isalpha(ch)){
            free(line);
            fprintf(stderr, "\tA .string command is missing an argument\n");
            return tooManyArg;
        }
        ch = line[i++];
    }
    if (ch == '\0') {
        free(line);
        fprintf(stderr, "\tA .string command is missing an argument\n");
        return missingParentheses;
    }
    ch = line[++i];
    while (ch != '\0' && ch != '\"') {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        binaryOf(newNode->data.InstructionCode, line[i], DATA_SIZE);
        newNode->data.place = (*counter)++;
        addToList(dataList, &newNode);
        ch = line[++i];
    }
    newNode = (Node *) calloc(1, sizeof(Node));
    checkAlloc(newNode);
    binaryOf(newNode->data.InstructionCode, '\0', DATA_SIZE);
    newNode->data.place = (*counter)++;
    addToList(dataList, &newNode);
    if (ch != '\"') {
        (*counter) -= dataList->count;
        clearList(dataList, &OGNode);
        (*counter) += dataList->count;
        fprintf(stderr, "\tA .string command is missing a closing parentheses\n");
        free(line);
        return missingClosingParentheses;
    }
    while ((ch = line[++i]) != '\0') {
        if (!isalpha(ch) && !isspace(ch)) {
            (*counter) -= dataList->count;
            clearList(dataList, &OGNode);
            (*counter) += dataList->count;
            fprintf(stderr, "\tA .string command has too many args\n");
            free(line);
            return tooManyArg;
        }
    }
    free(line);
    return success;
}

error codeCommand (char *line, list *instructionList, opcode command, int *count) {
    addressMethod am1 = 0, am2 = 0, prm1 = 0, prm2 = 0;
    char *label = NULL, *arg1 = NULL, *arg2 = NULL, *endPTR;
    int argVal;
    Node *newNode = (Node *) calloc(1, sizeof(Node));
    checkAlloc(newNode);
    switch (command) {
        case jmp:
        case bne:
        case jsr:
            getToken(&line, &label, "(");
            am1 = 0;
            if (!label) {
                getToken(&line, &arg2, NULL);
                if (arg2 && strpbrk(arg2, ",\t ")) {
                    fprintf(stderr, "\tCommand has too many arguments\n");
                    return tooManyArg;
                }
                idArg(&arg2, &am2);
                arg1 = NULL;
            } else {
                getToken(&line, &arg1, ",");
                idArg(&arg1, &prm1);
                getToken(&line, &arg2, ")");
                idArg(&arg2, &prm2);
                clearWhiteSpace(&line);
                if (line) {
                    fprintf(stderr, "\tCommand has too many arguments\n");
                    return tooManyArg;
                }
                am2 = jumpWP;
            }
            if (arg2 == NULL) {
                fprintf(stderr, "\tCommand is missing arguments\n");
                return missingArg;
            }
            if (am2 != direct && am2 != jumpWP) {
                fprintf(stderr, "\tCommand has a wrong type of argument\n");
                return wrongArg;
            }
            break;
        case mov:
        case add:
        case lea:
        case cmp:
        case sub:
            prm1 = prm2 = 0;
            getToken(&line, &arg1, ",");
            getToken(&line, &arg2, NULL);
            if (!arg2 || !arg1 || !strcmp(arg1, "")) {
                fprintf(stderr, "\tCommand is missing arguments\n");
                return missingArg;
            }
            if (strpbrk(arg2, ", \t") != NULL) {
                fprintf(stderr, "\tConsecutive commas or Too many args\n");
                return consecutiveCommas;
            }
            idArg(&arg1, &am1);
            idArg(&arg2, &am2);
            if ((command == lea && am1 != direct) || am2 == immediate) {
                fprintf(stderr, "\tCommand has a wrong type of argument\n");
                return wrongArg;
            }
            break;
        case not:
        case clr:
        case inc:
        case dec:
        case red:
        case prn:
            prm1 = prm2 = 0;
            label = arg1 = NULL;
            am1 = 0;
            getToken(&line, &arg2, NULL);
            if (!arg2) {
                fprintf(stderr, "\tCommand is missing arguments\n");
                return missingArg;
            }
            if (strpbrk(arg2, ", \t")) {
                fprintf(stderr, "\tCommand has too many arguments\n");
                return tooManyArg;
            }
            if (!(command == prn || command == not) && idArg(&arg2, &am2) == success && am2 == immediate) {
                fprintf(stderr, "\tCommand has a wrong type of argument\n");
                return wrongArg;
            }
            break;
        case stop:
        case rts:
            am1 = am2 = 0;
            label = arg1 = arg2 = NULL;
            clearWhiteSpace(&line);
            if (line) {
                fprintf(stderr, "\tCommand has too many arguments\n");
                return tooManyArg;
            }
            break;
        default:
            break;
    }
    binaryOf(newNode->data.InstructionCode + PRM_1_START, prm1, PRM_SIZE);
    binaryOf(newNode->data.InstructionCode + PRM_2_START, prm2, PRM_SIZE);
    binaryOf(newNode->data.InstructionCode + OPCODE_START, command, OPCODE_SIZE);
    binaryOf(newNode->data.InstructionCode + SRC_AM_START, am1, AM_SIZE);
    binaryOf(newNode->data.InstructionCode + DEST_AM_START, am2, AM_SIZE);
    binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
    newNode->data.place = (*count)++;
    addToList(instructionList, &newNode);
    if (label) {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        newNode->data.place = (*count)++;
        clearWhiteSpace(&label);
        strcpy(newNode->data.name, label);
        addToList(instructionList, &newNode);
        am1 = prm1;
        am2 = prm2;
    }
    if (arg1) {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        newNode->data.place = (*count)++;
        switch (am1) {
            case immediate:
                argVal = strtol(arg1 + 1, &endPTR, 10);
                if (strcmp(endPTR, "")) {
                    fprintf(stderr, "\tCommand has an unknown argument\n");
                    return unknownArg;
                }
                binaryOf(newNode->data.InstructionCode, argVal, IMMEDIATE_SIZE);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
                addToList(instructionList, &newNode);
                break;
            case directRegister:
                binaryOf(newNode->data.InstructionCode + REGI_1_START, arg1[1] - '0', REGI_SIZE);
                binaryOf(newNode->data.InstructionCode + REGI_2_START, 0, REGI_SIZE);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
                if (am2 != directRegister)
                    addToList(instructionList, &newNode);
                break;
            default:
                strcpy(newNode->data.name, arg1);
                addToList(instructionList, &newNode);
                break;
        }
    }
    if (arg2) {
        switch (am2) {
            case immediate:
                newNode = (Node *) calloc(1, sizeof(Node));
                checkAlloc(newNode);
                newNode->data.place = (*count)++;
                argVal = strtol(arg2 + 1, &endPTR, 10);
                if (strcmp(endPTR, "")) {
                    fprintf(stderr, "\tCommand has an unknown argument\n");
                    return unknownArg;
                }
                binaryOf(newNode->data.InstructionCode, argVal, 12);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, 2);
                break;
            case directRegister:
                if (!newNode) {
                    newNode = (Node *) calloc(1, sizeof(Node));
                    checkAlloc(newNode);
                    newNode->data.place = (*count)++;
                    binaryOf(newNode->data.InstructionCode + REGI_1_START, 0, REGI_SIZE);
                }
                binaryOf(newNode->data.InstructionCode + REGI_2_START, arg2[1] - '0', REGI_SIZE);
                binaryOf(newNode->data.InstructionCode + ARE_START, ARE_Immediate, ARE_SIZE);
                break;
            default:
                newNode = (Node *) calloc(1, sizeof(Node));
                checkAlloc(newNode);
                newNode->data.place = (*count)++;
                strcpy(newNode->data.name, arg2);
                break;
        }
        addToList(instructionList, &newNode);
    }
    freeMulti(&line, &label, &arg1, &arg2, NULL);
    return success;
}

error printList (list *lst, Node **node) {
    if (node && *node) {
        printf("%04d\t", (*node)->data.place);
        if (strcmp((*node)->data.InstructionCode, ""))
            printf("%s\n", (*node)->data.InstructionCode);
        else
            printf("%s\n", (*node)->data.name);
        if ((*node)->next)
            printList(lst, &((*node)->next));
    } else if (lst->head)
        printList(lst, &(lst->head));
    return success;
}

error codeLabel (opcode type, char* line , list* labelList) {
    error err;
    Node *node = NULL, *newNode = NULL;
    char *label = NULL;
    getToken(&line, &label, ",");
    if (!line) {
        fprintf(stderr, "\tError: Missing label \n");
        free(label);
        return missingArg;
    }
    if(idLabel(line)!=success) {
        freeMulti(&label, &line, NULL);
        return tooManyArg;
    }
    if (strlen(line) > LABEL_MAX_SIZE) {
        fprintf(stderr, "\tError: The definition of the label %s is too long\n", line);
        freeMulti(&label, &line, NULL);
        return labelTooLong;
    }
    if (label == NULL) {
        clearWhiteSpace(&line);
        if ((err = strIsAlphaDigit(line)) != success) {
            fprintf(stderr, "\tError: The definition of label %s is incorrect\n", line);
            freeMulti(&label, &line, NULL);
            return wrongDefLabel;
        }
        getToken(&line, &label, NULL);
        clearWhiteSpace(&label);
        if (line == NULL) {
            if ((err = searchNode(labelList, label, &node)) == success) {
                newNode = createNodeFirstRun(label, type, -1, "");
                addToList(labelList, &newNode);
                freeMulti(&label, &line, &newNode, NULL);
                return success;
            } else if (err == labelExists) {
                if (node->data.type != type) {
                    /*fixes label "place" problem:*/
                    if (node->data.type == none)
                        node->data.type = type;
                    else {
                        newNode = createNodeFirstRun(label, type, -1, "");
                        addToList(labelList, &newNode);
                    }
                    freeMulti(&label, &line, &newNode, NULL);
                    return success;
                } else {
                    fprintf(stderr, "\tError: The definition of label %s is already exists\n", node->data.name);
                    freeMulti(&label, &line, &newNode, NULL);
                    return labelExists;
                }
            } else if (line == NULL) {
                fprintf(stderr, "\tError: The definition of label %s is already exists\n", node->data.name);
                freeMulti(&label, &line, &newNode, NULL);
                return labelExists;
            }
        }
    } else if ((err = strIsAlphaDigit(line)) != success) {
        fprintf(stderr, "\tError: Definition of two labels at once\n");
        freeMulti(&label, &line, &newNode, NULL);
        return wrongDefLabel;
    } else {
        fprintf(stderr, "\tError: The definition of the label %s is incorrect\n", line);
        freeMulti(&label, &line, &newNode, NULL);
        return wrongDefLabel;
    }
    freeMulti(&label, &line, &newNode, &node, NULL);
    return success;
}

error setLabelAddress ( list *lst, char* name,int address) {
    Node *relevantNode, *temp;
    if (idLabel(name) == success) {
        if (searchNode(lst, name, &relevantNode) != labelExists) {
            relevantNode = (Node *) calloc(1, sizeof(Node));
            checkAlloc(relevantNode);
            strcpy(relevantNode->data.name, name);
            relevantNode->data.type = none;
            temp = relevantNode;
            addToList(lst, &relevantNode);
            relevantNode = temp;
        } else if (relevantNode->data.place != -1)
            return labelExists;
        relevantNode->data.place = address;
        return success;
    }
    return unknownArg;
}

error idArg(char **arg, addressMethod *amArg) {
    clearWhiteSpace(arg);
    if (arg == NULL || (*arg) == NULL) {
        *amArg = 0;
        return emptyArg;
    }
    if ((*arg)[0] == '#') {
        *amArg = immediate;
        return success;
    } else if (strlen(*arg) == REGI_NAME_SIZE && (*arg)[0] == 'r' && '0' <= (*arg)[1] && (*arg)[1] <= '7') {
        *amArg = directRegister;
        return success;
    } else if (idLabel(*arg) == success) {
        *amArg = direct;
        return success;
    }
    return unknownArg;
}

error firstRun (char *path) {
    Node *node;
    FILE *stream;
    error errFlag = success;
    opcode commandOP;
    list dataList = {0};
    list labelList = {0};
    list instructionList = {0};
    int IC = 100, DC = 0;
    char *label, *word, *line;
    openFile(&stream, path, ".am");
    while (!feof(stream)) {
        getOneLine(&line, stream);
        errFlag = removeComments(&line);
        printf("%s\n", line);
        errFlag = getToken(&line, &label, ":"); /*Get label if exists*/
        errFlag = getToken(&line, &word, " \t\n"); /*Get first word of line*/
        if (!word)
            errFlag = getToken(&line, &word, NULL);
        if (!line && !label && !word) {
            freeMulti(&word, &line, &label, NULL);
            continue;
        }
        errFlag = idCommand(word, &commandOP); /*Identify the command and assign an enum value */
        if (label && (commandOP == data || commandOP == string))
            errFlag = setLabelAddress(&labelList, label, DC + IC);
        switch (commandOP) {
            case none:
                fprintf(stderr, "\tUndefined command encountered\n");
                free(line);
                break;
            case data:
                errFlag = codeData(line, &dataList, &DC);
                break;
            case string:
                errFlag = codeString(line, &dataList, &DC);
                break;
            case external:
            case entry:
                if (label) {
                    errFlag = meaninglessLabel;
                    free(label);
                    label = NULL;
                }
                errFlag = codeLabel(commandOP, line, &labelList);
                break;
            default:
                if (label)
                    errFlag = setLabelAddress(&labelList, label, IC);
                errFlag = codeCommand(line, &instructionList, commandOP, &IC);
                break;
        }
        freeMulti(&label, &word, NULL);
    }
    node = dataList.head;
    while (node) {
        node->data.place += IC;
        node = node->next;
    }

    errFlag = closeFile(stream);
    errFlag = printList(&instructionList, NULL);
    printf("\n");
    errFlag = printList(&dataList, NULL);
    printf("\n");
    errFlag = printList(&labelList, NULL);
    secondRun(&dataList, &labelList, &instructionList, path, errFlag);
    errFlag = clearList(&instructionList, NULL);
    errFlag = clearList(&labelList, NULL);
    errFlag = clearList(&dataList, NULL);
    return errFlag;
}

error secondRun(list* dataList, list* labelList, list* instructionList,char* fileName, error errFlag) {
    Node *currentNode = NULL, *nodeOut = NULL;
    int i, cntEnt = 0, cntExt = 0;
    FILE *fpObj = NULL, *fpEnt = NULL, *fpExt = NULL;
    char place[3];
    currentNode = instructionList->head;
    for (i = 0; i < instructionList->count; i++) {
        /*If it's a label and not an encoded line*/
        if (isalpha(currentNode->data.name[0])) {
            if (searchNode(labelList, currentNode->data.name, &nodeOut) == labelExists) {
                /*nodeOut->data.place = currentNode->data.place;*/
                if (nodeOut->data.type == external) {
                    cntExt++;
                    binaryOf(currentNode->data.InstructionCode, 0, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "./");
                } else if (nodeOut->data.type == entry) {
                    cntEnt++;
                    binaryOf(currentNode->data.InstructionCode, nodeOut->data.place, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "/.");
                } else {
                    binaryOf(currentNode->data.InstructionCode, nodeOut->data.place, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "/.");
                }
            } else {
                errFlag = missingLabel;
                fprintf(stderr, "\tmissing label %s \n", currentNode->data.name);
            }
        }
        currentNode = currentNode->next;
    }
    /*There were no errors*/
    if (errFlag == success) {
        createFile(&fpObj, fileName, ".obj");
        /*write the instruction codes into new file ".obj"*/
        currentNode = instructionList->head;
        for (i = 0; i < instructionList->count; i++) {
            fprintf(fpObj, "%04d\t", currentNode->data.place);
            fputs(currentNode->data.InstructionCode, fpObj);
            fputs("\n", fpObj);
            currentNode = currentNode->next;
        }

        currentNode = dataList->head;
        for (i = 0; i < dataList->count; i++) {
            fprintf(fpObj, "%04d\t", currentNode->data.place);
            fputs(currentNode->data.InstructionCode, fpObj);
            fputs("\n", fpObj);
            currentNode = currentNode->next;
        }
        fclose(fpObj);

        /*there is entry*/
        if (cntEnt) {
            createFile(&fpEnt, fileName, ".ent");
            currentNode = labelList->head;
            for (i = 0; i < labelList->count; i++) {
                if (currentNode->data.type == entry) {
                    fputs(currentNode->data.name, fpEnt);
                    fputs("\t", fpEnt);
                    binaryOf(currentNode->data.InstructionCode,currentNode->data.place,DATA_SIZE);
                    fputs(currentNode->data.InstructionCode,fpEnt);
                    fputs("\n", fpEnt);
                }
                currentNode = currentNode->next;
            }
            fclose(fpEnt);
        }
        /*there is extern*/
        if (cntExt) {
            createFile(&fpExt, fileName, ".ext");
            currentNode = instructionList->head;
            for (i = 0; i < instructionList->count; i++) {
                if (isalpha(currentNode->data.name[0])) {
                    searchNode(labelList, currentNode->data.name, &nodeOut);
                    if (nodeOut->data.type == external) {
                        fputs(currentNode->data.name, fpExt);
                        fputs("\t", fpExt);
                        binaryOf(currentNode->data.InstructionCode,currentNode->data.place,DATA_SIZE);
                        fputs(currentNode->data.InstructionCode,fpExt);
                        fputs("\n", fpExt);
                    }
                }
                currentNode = currentNode->next;
            }
            fclose(fpExt);
        }
    }
    return success;
}