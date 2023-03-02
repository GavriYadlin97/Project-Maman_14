/* Created by Adi.s & Gavri.y 22/02/2023. */

#include "mainHeader.h"
#include "asmFirstRun.h"

void binaryOf(char binaryForm[], unsigned num, int size) {
    int i = 0;
    while (num > 0) {
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
    while (b < size-1 && str[b + 1] != '\0')
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

/* Function to create a new node*/
struct Node* createNodeFirstRun(char * name, opcode opcode, int place, char* instructionCode) {
    // Allocate memory for the new node
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed");
        exit(1);
    }
    strcpy(newNode->data.name, name);
    newNode->data.type=opcode;
    newNode->data.place=place;
    strcpy(newNode->data.InstructionCode,instructionCode);
    newNode->next = NULL;
    return newNode;
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

/*search node by name
 * checks only if the names are the same
 * received pointer to list and name (char*)*/
static error searchNode(list* list, char* name,Node* nodeOut){
    Node * currentNode=list->head;
    int i;
    if (list->count==0)
        return success;
    else {

        for(i=0;i< list->count;i++){
            if(!strcmp(name,currentNode->data.name)){
                *nodeOut=*currentNode;
                return labelExists;
            }
            currentNode=currentNode->next;
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
        binaryOf(newNode->data.InstructionCode, num, sizeof(newNode->data.InstructionCode));
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
        binaryOf(newNode->data.InstructionCode, line[i], sizeof(newNode->data.InstructionCode));
        addToList(dataList, newNode);
        (*counter)++;
        ch = line[++i];
    }
    newNode = (Node *) malloc(sizeof(Node));
    binaryOf(newNode->data.InstructionCode, '\0', sizeof(newNode->data.InstructionCode));
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

error codeCommand (char *line, list *instructionList, opcode command, int *count) {
    addressMethod am1, am2;
    char *label, *arg1, *arg2, *endPTR;
    char instruction[WORD], immediateArg[12], regiCode[6] = {'.', '.', '.', '.', '.', '.'},
            opCode[4], dataCode[2];
    int argVal = 0,i=0;
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (!newNode)
        return memoryAllocErr;
    getToken(&line, &label, "(");
    getToken(&line, &arg1, ",");
    if (label)
        getToken(&line, &arg2, ")");
    else
        getToken(&line, &arg2, NULL);
    /*code the command*/
    binaryOf(opCode, command, sizeof(opCode));
    memcpy(instruction + OPCODE_START, opCode, sizeof(opCode));
    /*if only one arg: move it to destination arg*/
    if (arg2 == NULL) {
        arg2 = arg1;
        arg1 = NULL;
    }
    idArg(arg1, &am1);
    binaryOf(dataCode, am1, sizeof(dataCode));
    memcpy(instruction + SRC_AM_START, dataCode, sizeof(dataCode));
    idArg(arg2, &am2);
    binaryOf(dataCode, am2, sizeof(dataCode));
    memcpy(instruction + DEST_AM_START, dataCode, sizeof(dataCode));
    memcpy(instruction + ARE_START, "..", 1 + strlen("..")); /*command A.R.E is 0*/
    switch (command) {
        case jmp:
        case bne:
        case jsr:
            binaryOf(dataCode, am1, sizeof(dataCode));
            memcpy(instruction + PRM_1_START, dataCode, sizeof(dataCode));
            binaryOf(dataCode, am2, sizeof(dataCode));
            memcpy(instruction + PRM_2_START, dataCode, sizeof(dataCode));
            break;
        default:
            memcpy(instruction + PRM_1_START, "..", strlen(".."));
            memcpy(instruction + PRM_2_START, "..", strlen(".."));
            break;
    }
    memcpy(newNode->data.InstructionCode, instruction, strlen(instruction) + 1);
    newNode->data.place = (*count)++;
    addToList(instructionList, newNode);
    if (arg1) {
        switch (am1) {
            case immediate:
                newNode = (Node *) malloc(sizeof(Node));
                if (!newNode)
                    return memoryAllocErr;
                argVal = strtol(arg1 + 1, &endPTR, 10);
                if (strcmp(endPTR, ""))
                    return unknownArg;
                binaryOf(immediateArg, argVal, sizeof(immediateArg));
                memcpy(instruction, immediateArg, sizeof(immediateArg));
                memcpy(instruction + ARE_START, "..", 1 + strlen(".."));
                memcpy(newNode->data.InstructionCode, instruction, sizeof(instruction));
                newNode->data.place = (*count)++;
                addToList(instructionList, newNode);
                for (i = 0; i < sizeof(instruction)-1; ++i)
                    instruction[i] = '.';

                break;
            case directRegister:
                binaryOf(regiCode, arg1[1] - '0', sizeof(regiCode));
                memcpy(instruction, regiCode, sizeof(regiCode));
                memcpy(instruction + ARE_START, "..", 1 + strlen(".."));
                break;
            default:
                break;
        }
    }
    if (arg2) {
        newNode = (Node *) malloc(sizeof(Node));
        if (!newNode)
            return memoryAllocErr;
        switch (am2) {
            case immediate:
                argVal = strtol(arg2 + 1, &endPTR, 10);
                if (strcmp(endPTR, ""))
                    return unknownArg;
                binaryOf(immediateArg, argVal, sizeof(immediateArg));
                memcpy(instruction + REGI_1_START, immediateArg, sizeof(immediateArg));
                memcpy(instruction + ARE_START, "..", 1 + strlen(".."));
                memcpy(newNode->data.InstructionCode, instruction, sizeof(instruction));
                newNode->data.place = (*count)++;
                addToList(instructionList, newNode);
                break;
            case directRegister:
                binaryOf(regiCode, arg2[1] - '0', sizeof(regiCode));
                memcpy(instruction + REGI_2_START, regiCode, sizeof(regiCode));
                memcpy(instruction + ARE_START, "..", 1 + strlen(".."));
                memcpy(newNode->data.InstructionCode, instruction, sizeof(instruction));
                newNode->data.place = (*count)++;
                addToList(instructionList, newNode);
                break;
            default:
                break;
        }
    }
    return success;
}


error codeLabel (opcode type, char* line ,char* label, list* labelList){
    getToken(&line,&label,",");
    if(label==NULL) {
        getToken(&line, &label, NULL);
        clearWhiteSpace(&label);
        if (line == NULL) {
            Node* node= malloc(sizeof (Node));
            if(!node){
                return memoryAllocErr;
            }

            if(!searchNode(labelList,label,node)){
                Node * newNode= createNodeFirstRun(label,type,-1,"");
                addToList(labelList,newNode);
            }
            else{
                fprintf(stderr,"Error: The definition of label %s is already exists\n",node->data.name);
                return labelExists;
            }
        }
    }
    else {
        fprintf(stderr, "Error: definition of two labels at once\n");
    }
}

error idArg(char *arg, addressMethod *amArg) {
    if (!arg) {
        *amArg = 0;
        return emptyArg;
    }
    clearWhiteSpace(&arg);
    if (arg[0] == '#') {
        *amArg = immediate;
        return success;
    } else if (strlen(arg) == 2 && arg[0] == 'r' && '0' <= arg[1] && arg[1] <= '7') {
        *amArg = directRegister;
        return success;
    } else
        return success;
    return unknownArg;
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
   while(!feof(stream)) {
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
               break;
           case string:
               codeString(line, &dataList, &DC);
               break;
           case external:
               if(!label) {
                   errFlag = meaninglessLabel;
                   free(label);
                   codeLabel(external,line,label,&labelList);
               }
               else
                   codeLabel(external,line,label,&labelList);
               break;
           case entry:
               if(!label) {
                   errFlag = meaninglessLabel;
                   free(label);
                   codeLabel(entry,line,label,&labelList);
               }
               else
                   codeLabel(entry,line,label,&labelList);
               break;
           default:
               codeCommand(line, &instructionList, commandCode, &IC);
               break;
       }
   }
}
