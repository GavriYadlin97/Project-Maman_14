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

static error addToList(list *lst, Node **nodeToAdd) {
    (*nodeToAdd)->next = NULL;
    Node *ptr = lst->head;
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
    int num = 0;
    Node *newNode = NULL;
    getToken(&line, &word, ",");
    if (!word)
        getToken(&line, &word, NULL);
    while (word) {
        num = (int) strlen(word);
        clearWhiteSpace(&word);
        if (num != strlen(word) && line)
            return missingComma;
        num = strtol(word, &endPTR, 10);
        if (strcmp(endPTR, ""))
            return unknownArg;
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
    free(word);
    free(line);
    return success;
}

error codeString(char *line, list *dataList, int *counter) {
    Node *newNode, *OGNode = dataList->head;
    int i = 0;
    char ch = line[i];
    if (line == NULL)
        return emptyArg;
    if (OGNode)
        while (OGNode->next)
            OGNode = OGNode->next;
    while (ch != '\0' && ch != '\"')
        ch = line[i++];
    if (ch == '\0') {
        free(line);
        return missingParentheses;
    }
    ch = line[++i];
    while (ch != '\0' && ch != '\"') {
        newNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(newNode);
        binaryOf(newNode->data.InstructionCode, line[i], sizeof(newNode->data.InstructionCode));
        newNode->data.place = (*counter)++;
        addToList(dataList, &newNode);
        ch = line[++i];
    }
    newNode = (Node *) calloc(1, sizeof(Node));
    checkAlloc(newNode);
    binaryOf(newNode->data.InstructionCode, '\0', sizeof(newNode->data.InstructionCode));
    newNode->data.place = (*counter)++;
    addToList(dataList, &newNode);
    if (ch != '\"') {
        (*counter) -= dataList->count;
        clearList(dataList, &OGNode);
        (*counter) += dataList->count;
        return missingClosingParentheses;
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
                getToken(&line, &label, NULL);
                arg1 = arg2 = NULL;
            } else {
                getToken(&line, &arg1, ",");
                idArg(&arg1, &prm1);
                getToken(&line, &arg2, ")");
                idArg(&arg2, &prm2);
                if (arg1 && !arg2)
                    return missingArg;
                clearWhiteSpace(&line);
                if (line)
                    return tooManyArg;
                am2 = jumpWP;
            }
            break;
        case mov:
        case add:
        case lea:
        case cmp:
        case sub:
            prm1 = prm2 = 0;
            getToken(&line, &arg1, ",");
            idArg(&arg1, &am1);
            getToken(&line, &arg2, NULL);
            idArg(&arg2, &am2);
            if (!arg2 || !arg1)
                return missingArg;
            if (am2 == immediate)
                return wrongArg;
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
            idArg(&arg2, &am2);
            if (!arg2)
                return missingArg;
            break;
        case stop:
        case rts:
            am1 = am2 = 0;
            label = arg1 = arg2 = NULL;
            clearWhiteSpace(&line);
            if (line)
                return tooManyArg;
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
                if (strcmp(endPTR, ""))
                    return unknownArg;
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
                if (strcmp(endPTR, ""))
                    return unknownArg;
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
    free(line);
    free(label);
    free(arg1);
    free(arg2);
    line = label = arg1 = arg2 = NULL;
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

error codeLabel (opcode type, char* line ,char* label, list* labelList) {
    error err;
    getToken(&line, &label, ",");
    if (strlen(line) > LABEL_MAX_SIZE) {
        fprintf(stderr, "Error: The definition of the label %s is too long\n", line);
        return labelTooLong;
    }
    if (label == NULL) {
        clearWhiteSpace(&line);
        if ((err = strIsAlphaDigit(line)) != success) {
            fprintf(stderr, "Error: The definition of label %s is incorrect\n", line);
            return wrongDefLabel;
        }
        getToken(&line, &label, NULL);
        clearWhiteSpace(&label);
        if (line == NULL) {
            Node *node = calloc(1, sizeof(Node));
            checkAlloc(node);
            if ((err=searchNode(labelList, label, &node))==success) {
                Node *newNode = createNodeFirstRun(label, type, -1, "");
                addToList(labelList, &newNode);
                return success;
            }else if(err== labelExists){
                if (node->data.type!= type){
                    Node *newNode = createNodeFirstRun(label, type, -1, "");
                    addToList(labelList, &newNode);
                    return success;
                }
                else{
                    fprintf(stderr, "Error: The definition of label %s is already exists\n", node->data.name);
                    return labelExists;
                }
            }
            else if (line == NULL) {
                fprintf(stderr, "Error: The definition of label %s is already exists\n", node->data.name);
                return labelExists;
            }
        }
    } else if ((err = strIsAlphaDigit(line)) != success) {
        fprintf(stderr, "Error: Definition of two labels at once\n");
        return wrongDefLabel;
    } else {
        fprintf(stderr, "Error: The definition of thr label %s is incorrect\n", line);
        return wrongDefLabel;
    }
    return success;
}

error setLabelAddress ( list *lst, char* name,int *address) {
    Node *relevantNode, *temp;
    if (searchNode(lst, name, &relevantNode) != labelExists) {
        relevantNode = (Node *) calloc(1, sizeof(Node));
        checkAlloc(relevantNode);
        strcpy(relevantNode->data.name, name);
        relevantNode->data.type = none;
        temp = relevantNode;
        addToList(lst, &relevantNode);
        relevantNode = temp;
    }
    relevantNode->data.place = (*address);
    return success;
}

error idArg(char **arg, addressMethod *amArg) {
    if (!arg) {
        *amArg = 0;
        return emptyArg;
    }
    clearWhiteSpace(arg);
    if ((*arg)[0] == '#') {
        *amArg = immediate;
        return success;
    } else if (strlen(*arg) == REGI_NAME_SIZE && (*arg)[0] == 'r' && '0' <= (*arg)[1] && (*arg)[1] <= '7') {
        *amArg = directRegister;
        return success;
    } else
        *amArg = direct;
    return unknownArg;
}

error firstRun (char *path) {
    error errFlag;
    opcode commandOP;
    list dataList = {0};
    list labelList = {0};
    list instructionList = {0};
    FILE *stream;
    int IC = 100;
    int DC = 0;
    char *label, *word;
    char *line;
    openFile(&stream, path, ".am");
    while (!feof(stream)) {
        getOneLine(&line, stream);
        removeComments(&line);
        printf("%s", line);
        getToken(&line, &label, ":"); /*Get label if exists*/
        getToken(&line, &word, " \t\n"); /*Get first word of line*/
        if (!line) {
            free(word);
            continue;
        }
        idCommand(word, &commandOP); /*Identify the command and assign an enum value */
        if (label && (commandOP == data || commandOP == string))
            setLabelAddress(&labelList, label, &DC);
        switch (commandOP) {
            case none:
                errFlag = undefinedCommand;
                free(line);
                break;
            case data:
                codeData(line, &dataList, &DC);
                break;
            case string:
                codeString(line, &dataList, &DC);
                break;
            case external:
            case entry:
                if (!label) {
                    errFlag = meaninglessLabel;
                    free(label);
                    label = NULL;
                    errFlag = codeLabel(commandOP, line, label, &labelList);
                } else
                    errFlag = codeLabel(commandOP, line, label, &labelList);
                break;
            default:
                if (label)
                    setLabelAddress(&labelList, label, &IC);
                codeCommand(line, &instructionList, commandOP, &IC);
                break;
        }
        free(label);
        free(word);
    }
    closeFile(stream);
    printList(&instructionList, NULL);
    printList(&dataList, NULL);
    secondRun(&dataList, &labelList, &instructionList, path, errFlag);
    clearList(&instructionList, NULL);
    clearList(&labelList, NULL);
    clearList(&dataList, NULL);
    return success;
}

error secondRun(list* dataList, list* labelList, list* instructionList,char* fileName, error errFlag) {
    Node *currentNode ,*nodeOut ;
    int i, cntEnt=0, cntExt = 0;
    char *newFileName;
    FILE *fpObj, *fpEnt, *fpExt;
    currentNode = instructionList->head;
    for (i = 0; i < instructionList->count; i++) {
        /*If it's a label and it's not an encoded line*/
        if (isalpha(currentNode->data.name[0])) {
            if (searchNode(labelList, currentNode->data.name, &nodeOut) == labelExists) {
                nodeOut->data.place=currentNode->data.place;

                if (nodeOut->data.type == external) {
                    cntExt++;
                    binaryOf(currentNode->data.InstructionCode, 0, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "./");
                } else if (nodeOut->data.type == entry) {
                    cntEnt++;
                    binaryOf(currentNode->data.InstructionCode, nodeOut->data.place, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "/.");
                }
                else{
                    binaryOf(currentNode->data.InstructionCode, nodeOut->data.place, ADDRESS_SIZE);
                    strcpy(currentNode->data.InstructionCode + ARE_START, "/.");
                }
            } else {
                errFlag = missingLabel;
                fprintf(stderr,"missing label %s \n",currentNode->data.name);
            }
            currentNode = currentNode->next;
        } else
            currentNode = currentNode->next;
    }
    /*There were no errors*/
    if (errFlag == success) {
        insertSuffix(fileName, &newFileName, ".obj");
        fpObj = fopen(newFileName, "w");
        /*write the instruction codes into new file ".obj"*/
        currentNode = instructionList->head;
        for (i = 0; i < instructionList->count; i++) {
            fprintf(fpObj,"%04d\t",currentNode->data.place);
            fputs(currentNode->data.InstructionCode, fpObj);
            fputs("\n", fpObj);
            fflush(fpObj);
            currentNode = currentNode->next;
        }
        fclose(fpObj);

        currentNode = dataList->head;
        for (i = 0; i < dataList->count; i++) {
            fputs(currentNode->data.InstructionCode, fpObj);
            fputs("\n", fpObj);
            fflush(fpObj);
            currentNode = currentNode->next;
        }

        /*there is entry*/
        if (cntEnt) {
            insertSuffix(fileName, &newFileName, ".ent");
            fpEnt = fopen(newFileName, "w");
            currentNode = labelList->head;
            for (i = 0; i < labelList->count; i++) {
                if (currentNode->data.type == entry) {
                    fputs(currentNode->data.name, fpEnt);
                    fputs("\t", fpEnt);
                    fprintf(fpEnt, "%d" , currentNode->data.place);
                    fputs("\n", fpEnt);
                    fflush(fpEnt);
                }
                currentNode = currentNode->next;
            }
            fclose(fpEnt);
        }
        /*there is extern*/
        if (cntExt) {
            insertSuffix(fileName, &newFileName, ".ext");
            fpExt = fopen(newFileName, "w");
            currentNode = instructionList->head;
            for (i = 0; i < instructionList->count; i++) {
                if (isalpha(currentNode->data.name[0])) {
                    searchNode(labelList,currentNode->data.name,&nodeOut);
                    if(nodeOut->data.type==external){
                        fputs(currentNode->data.name, fpExt);
                        fputs("\t", fpExt);
                        fprintf(fpExt, "%d" , currentNode->data.place);
                        fputs("\n", fpExt);
                        fflush(fpExt);
                    }
                }
                currentNode = currentNode->next;
            }
            fclose(fpExt);
        }
    }
}