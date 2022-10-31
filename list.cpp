#include "list.h"
#include <cstdlib>
#include <cstdarg>

//TODO: new List with struct
//      define Head and Tail
//      PhysIns and PhysErase
//      Better GraphViz

#define TAIL list -> prev[0]
#define HEAD list -> next[0]

const size_t   DUMP_SIZE  = 20;
const size_t   FictElem   =  0;
const size_t   DataFree   = -1;
const size_t MAX_CMD_SIZE = 40;

static FILE *LogGraph = fopen("logs/HtmlLog.html", "w");
static FILE *LogFile  = LogGraph;


int _listCtor(List *list, const char * name, const char *file, const char *function, size_t line) {
    catchNullptr(list);
    if (list -> status == Active) return ListDoubleConstruction;
    
    list -> data = (Elem_t *) calloc(MAX_LIST_SIZE, sizeof(Elem_t));
    catchNullptr(list -> data);
    
    list -> next = (size_t *) calloc(MAX_LIST_SIZE, sizeof(size_t));
    catchNullptr(list -> next);

    list -> prev = (size_t *) calloc(MAX_LIST_SIZE, sizeof(size_t));
    catchNullptr(list -> data);

    for (size_t curElem = 0; curElem < MAX_LIST_SIZE; ++curElem) {
        list -> data[curElem] =   POISON   ;
        list -> next[curElem] = curElem + 1;
        list -> prev[curElem] =  DataFree  ;
    }
    list -> next[MAX_LIST_SIZE - 1] = 0;

    list ->  next[0] = 0;
    list ->  prev[0] = 0;

    list ->   head   = 0;
    list -> nextFree = 1;

    list -> info. pointer = (char *)   list  ;
    list -> info.  name   = (char *)   name  ;
    list -> info.  file   = (char *)   file  ;
    list -> info.function = (char *) function;
    list -> info.  line   =        line      ;

    list ->     status    =      Active      ;
    list ->    sweetLife  =      Sweet       ;

    return ListIsOk;
}

int listDtor(List * list) {
    catchNullptr(list);
    if (list -> status == InActive) return ListDoubleDestruction;

    free(list ->   data  );
    free(list ->   next  );
    free(list ->   prev  );

    list ->   head   =  -1  ;
    list -> nextFree =  -1  ;
    list ->  status  = Sweet;

    return ListIsOk;

}

int listLogicInsert(List *list, size_t ind, Elem_t val, int *err) {
    if (list == nullptr) return POISON;

    *err = listVerify(list);
    if (*err) return POISON;

    if (list -> nextFree == list -> head) { 
        *err = ListIsEmpty;
        return POISON;
    }

    size_t prevPos = listGetPos(list, ind);
    if (prevPos == POISON) return POISON;

    printf("______________\n");
    
    size_t   newPos  = list ->   nextFree  ; 

    list -> nextFree = list -> next[newPos];  
    
    list -> next[newPos] =        list -> next[prevPos]      ;
    list -> prev[newPos] = list -> prev[list -> next[newPos]];

    list -> prev[list -> next[newPos]] = newPos;
    list -> next[list -> prev[newPos]] = newPos;

    list -> data[newPos] = val; 

    if (newPos - list -> next[list -> head] + 1 != ind)
        list -> sweetLife = Bitter;

     printf("%d\n", newPos);

    *err = listVerify(list);
    if (*err) return POISON;

    return newPos;
}

int listPushBack(List *list, Elem_t val, int *err = nullptr) {
    catchNullptr(list);

    *err = listVerify(list);
    if (*err) return POISON;

    if (list -> nextFree == FictElem) return ListIsEmpty;
    
    size_t prevPos = list -> prev[list -> head];
    if (prevPos == POISON) return POISON;

    fprintf(stderr, "-----\n");

    
    size_t   newPos  = list ->   nextFree  ; 

    list -> nextFree = list -> next[newPos];  
    
    list -> next[newPos] =        list -> next[prevPos]      ;
    list -> prev[newPos] = list -> prev[list -> next[newPos]];

    list -> prev[list -> next[newPos]] = newPos;
    list -> next[list -> prev[newPos]] = newPos;

    list -> data[newPos] = val; 

    *err = listVerify(list);
    if (*err) return POISON;

    return newPos;
}

int listPushFront(List *list, Elem_t val, int *err = nullptr) {
    catchNullptr(list);

    *err = listVerify(list);
    if (*err) return POISON;
    // TODO: enum for 0

    if (list -> nextFree == FictElem) return ListIsEmpty;
    
    size_t  prevPos  =       FictElem      ;
    size_t   newPos  = list ->   nextFree  ; 

    list -> nextFree = list -> next[newPos];  
    
    list -> next[newPos] =        list -> next[prevPos]      ;
    list -> prev[newPos] = list -> prev[list -> next[newPos]];

    list -> prev[list -> next[newPos]] = newPos;
    list -> next[list -> prev[newPos]] = newPos;

    list -> data[newPos] = val; 

    *err = listVerify(list);
    if (*err) return POISON;

    return newPos;
}

int listLogicErase (List *list, size_t ind) {
    catchNullptr(list);

    if (ind == list -> head) return ListHeadErase;

    int err = listVerify(list);
    if (err) return err;

    if (HEAD == FictElem) return ListIsEmpty;

    size_t curPos = listGetPos(list, ind);
    if (curPos == POISON) return ListWrongIndex;

    if (list -> prev[curPos] != 0 && list -> next[curPos] != 0)
        list -> sweetLife = Bitter;
    
    list -> next[list -> prev[curPos]] = list -> next[curPos];
    list -> prev[list -> next[curPos]] = list -> prev[curPos];

    list -> prev[curPos] =      DataFree    ;
    list -> data[curPos] =       POISON     ;
    list -> next[curPos] =  list -> nextFree;
    list ->   nextFree   =       curPos     ;

    err = listVerify(list);
    if (err) return err;

    return ListIsOk;
}



int listPhysInsert(List *list, size_t ind, Elem_t val, int *err) {
    if (list == nullptr) return POISON;

    *err = listVerify(list);
    if (*err) return POISON;
    
    if (list -> nextFree == FictElem) {
        *err = ListIsFull;
        return POISON;
    }

    if (list -> data[ind] == DataFree) {
        *err = ListWrongIndex;
        return POISON;
    }

    size_t  prevPos  =        ind          ;
    size_t   newPos  = list ->   nextFree  ;
    list -> nextFree = list -> next[newPos];  
    
    list -> next[newPos] =        list -> next[prevPos]      ;
    list -> prev[newPos] = list -> prev[list -> next[newPos]];

    list -> prev[list -> next[newPos]] = newPos;
    list -> next[list -> prev[newPos]] = newPos;

    list -> data[newPos] = val;

    *err = listVerify(list);
    if (*err) return 0;

    return newPos;
}

int listPhysErase (List *list, size_t ind) {
    catchNullptr(list);

    int err = listVerify(list);
    if (err) return err;

    if (TAIL == HEAD) return ListIsEmpty;

    size_t curPos = ind;
    if (list -> prev[curPos] == DataFree) return ListWrongIndex;
    
    list -> next[list -> prev[curPos]] = list -> next[curPos];
    list -> prev[list -> next[curPos]] = list -> prev[curPos];

    list -> prev[curPos] =      DataFree    ;
    list -> next[curPos] =  list -> nextFree;
    list -> data[curPos] =       POISON     ;

    list -> nextFree = curPos;

    err = listVerify(list);
    if (err) return err;

    return ListIsOk;
}


int listGetPos(List *list, size_t index) {
    if (list == nullptr) return POISON;

    if (list -> sweetLife) {
        size_t pos = index + HEAD;
        if (list -> prev[pos] == DataFree)
            return POISON;
        return pos;
    }

    size_t pos = list -> head;
    for (size_t cur = 0; cur < index; ++cur) {
        printf("Анжумання\n\a");
        pos = list -> next[pos];
        if (!pos)
            return POISON;
    }

    return pos;
}

static int reCalloc(List *list, int newSize) {
    catchNullptr(list);

    Elem_t *newData = (Elem_t *) calloc(newSize, sizeof(Elem_t));
    size_t *newNext = (size_t *) calloc(newSize, sizeof(size_t));
    size_t *newPrev = (size_t *) calloc(newSize, sizeof(size_t));

    catchNullptr(newData);
    catchNullptr(newNext);
    catchNullptr(newPrev);

    size_t   cur   = list -> next[list -> head];
    size_t newFree =            0              ;
    for (size_t copied = 0; copied < newSize; ++copied) {
        if (cur) {
            newData[copied] = list -> data[cur];
            newNext[copied] = copied + 1;
            newNext[copied] = copied - 1;

            cur = list -> next[cur];

            if (cur == 0) {
                newNext[copied] =     0     ;
                    newFree     = copied + 1;
            }

        } else {
            newData[copied] =   POISON  ;
            newNext[copied] = copied + 1;
            newPrev[copied] =  DataFree ;
        }
    }
    newData[0] = POISON;
    newPrev[0] =   0   ;

    free(list -> data);
    free(list -> next);
    free(list -> prev);

    list -> data = newData;
    list -> next = newNext;
    list -> prev = newPrev;

    list -> sweetLife = Sweet;

    return ListIsOk;
}

int listVerifyData(List *list);

int listVerifyFree(List *list);

int listVerify(List *list) {
    int err = ListIsOk;

    if (list         == nullptr)   return ListIsNull;

    if (list -> data == nullptr) err |= ListDataIsNull;
    if (list -> next == nullptr) err |= ListNextIsNull;
    if (list -> prev == nullptr) err |= ListPrevIsNull;

    if (err) return err;

    if (list -> status == InActive) err |= ListIsInActive;

    err |= listVerifyData(list);
    err |= listVerifyFree(list);

    return err;
}

int listPrint(List *list) {
    catchNullptr(list);

    // fprintf(stderr, "-----\n");
    for (size_t curElem = list -> next[list -> head]; curElem != list -> head; curElem = list -> next[curElem]) {
        printf("%zu -> %d\n", curElem, list -> data[curElem]);
    }

    return ListIsOk;
}

int listVerifyData(List *list) {
    catchNullptr(list);

    for (size_t curPos = list -> next[list -> head]; curPos != list -> head; curPos = list -> next[curPos])
        if (list -> data[curPos] == POISON) return ListPoisonInData;

    return ListIsOk;
}

int listVerifyFree(List *list) {
    catchNullptr(list);

    for (size_t curPos = list -> nextFree; curPos; curPos = list -> next[curPos])
        if (list -> prev[curPos] != DataFree) { printf("%zu\n", list -> prev[curPos]); return ListFreeBlocksErr;}

    return ListIsOk;
}

void myfPrintf(FILE *stream = nullptr, const char *format = nullptr, ...) {
    if (format == nullptr)
        return;

    va_list arguments;
    va_start (arguments, format);
    vfprintf(stderr, format, arguments);
    if (stream != nullptr)
        vfprintf(stream, format, arguments);
    va_end(arguments);
}

void listDump_(List *list, const char* functionName, const char *fileName, size_t line) {
    myfPrintf(LogFile, "\n%s at %s(%zu);<br>\n", functionName, fileName, line);
    myfPrintf(LogFile, "List[%08X] '%s' at '%s' at %s(%zu);\n{<br>\n", list->info.pointer, list->info.name, list->info.function, list->info.file, list->info.line);

    for (size_t cur = 0; cur < DUMP_SIZE; ++cur) {
        myfPrintf(LogFile, "%03zu ", cur);
    }
    myfPrintf(LogFile, "\n<br>");

    for (size_t cur = 0; cur < DUMP_SIZE; ++cur) {
        myfPrintf(LogFile, "%03d ", list -> data[cur]);
    }
    myfPrintf(LogFile, "\n<br>");

    for (size_t cur = 0; cur < DUMP_SIZE; ++cur) {
        myfPrintf(LogFile, "%03d ", list -> next[cur]);
    }
    myfPrintf(LogFile, "\n<br>");

    for (size_t cur = 0; cur < DUMP_SIZE; ++cur) {
        myfPrintf(LogFile, "%03d ", list -> prev[cur]);
    }
    myfPrintf(LogFile, "\n<br>}<br>\n");
}

int listGraphVizLogicDump(List *list, const char *outFileName) {
    catchNullptr(outFileName);
    catchNullptr(   list    );

    FILE *stream = fopen(outFileName, "w");

    fprintf(stream, "digraph List {\n"
                    "   rankdir = LR;\n");

    fprintf(stream, "   subgraph varriables {\n"
                    "       node[shape = component, style = filled, fillcolor = orange];\n"

                    "         Head   [label = \"Head = %d\"];\n"
                    "        Status  [label = \"Status = %d\"];\n"
                    "       NextFree [label = \"NextFree = %d\"];\n"
                    "       SweetLife[label = \"SweetLife = %d\"];\n"
                    "   }\n",
            list -> head, list -> status, list -> nextFree, list -> sweetLife);

    fprintf(stream, "   subgraph UsedNodes {\n"
                    "       node[shape = Mrecord, style = filled, fillcolor = lightgreen];\n");
    for (size_t curElem = list -> next[list -> head]; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "       l%zu[label = \" %zu "    "|"        "data: %d "    "|"    " next: %zu "   "|"    " prev: %d \"];\n",
                                curElem,     curElem,        list -> data[curElem], list -> next[curElem], list -> prev[curElem]);
    }
    fprintf(stream, "   }\n");

    fprintf(stream, "   subgraph FreeNodes {\n"
                    "       node[shape = Mrecord, style = filled, fillcolor = lightblue];\n");
    for (size_t curElem = list -> nextFree; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "       l%zu[label = \" %zu "    "|"        "data: %d "    "|"    " next: %zu "   "|"    " prev: %d \"];\n",
                                curElem,     curElem,        list -> data[curElem], list -> next[curElem], list -> prev[curElem]);
    }
    fprintf(stream, "   }\n");

    fprintf(stream, "   Head");  // Active edges [next]
    for (size_t curElem = HEAD; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "->Head[style = \"bold\", color = \"darkgreen\"];\n");

    fprintf(stream, "   Head");  // Active edges [prev]
    for (size_t curElem = TAIL; curElem != list -> head; curElem = list -> prev[curElem]) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "->Head[style = \"dashed\", color = \"grey\"];\n");

    fprintf(stream, "   NextFree");  // Inactive(free) edges [next]
    for (size_t curElem = list -> nextFree; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "->NextFree[style = \"bold\", color = \"darkblue\"];\n"
                    "}\n");


    fclose(stream);

    return ListIsOk;
}

int listGraphVizPhysDump(List *list, const char *fileName, int cmd) {
    catchNullptr(fileName);
    catchNullptr(  list  );

    FILE *stream = fopen(fileName, "w");
    catchNullptr(stream);

    fprintf(stream, "digraph List {\n"
                    "   rankdir = LR;\n");

    fprintf(stream, "   subgraph varriables {\n"
                    "       node[shape = component, style = filled, fillcolor = orange];\n"

                    "         Head   [label = \"Head = %d\"];\n"
                    "        Status  [label = \"Status = %d\"];\n"
                    "       NextFree [label = \"NextFree = %d\"];\n"
                    "       SweetLife[label = \"SweetLife = %d\"];\n"
                    "   }\n",
            list -> head, list -> status, list -> nextFree, list -> sweetLife);

    fprintf(stream, "   subgraph UsedNodes {\n"
                    "       node[shape = Mrecord, style = filled, fillcolor = lightgreen];\n");
    for (size_t curElem = list -> next[list -> head]; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "       l%zu[label = \" %zu "    "|"        "data: %d "    "|"    " next: %zu "   "|"    " prev: %d \"];\n",
                                curElem,     curElem,        list -> data[curElem], list -> next[curElem], list -> prev[curElem]);
    }
    fprintf(stream, "   }\n");

    fprintf(stream, "   subgraph FreeNodes {\n"
                    "       node[shape = Mrecord, style = filled, fillcolor = lightblue];\n");
    for (size_t curElem = list -> nextFree; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "       l%zu[label = \" %zu "    "|"        "data: %d "    "|"    " next: %zu "   "|"    " prev: %d \"];\n",
                                curElem,     curElem,        list -> data[curElem], list -> next[curElem], list -> prev[curElem]);
    }
    fprintf(stream, "   }\n");

    fprintf(stream, "   Head->NextFree");
    for (size_t curElem = 1; curElem < MAX_LIST_SIZE; ++curElem) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "[style = invis, weigth = 100];\n");
    
    fprintf(stream, "   Head");  // Active edges [next]
    for (size_t curElem = HEAD; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "->Head[weigth = 0, style = \"bold\", color = \"darkgreen\"];\n");

    fprintf(stream, "   Head");  // Active edges [prev]
    for (size_t curElem = TAIL; curElem != list -> head; curElem = list -> prev[curElem]) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "->Head[weigth = 0, style = \"dashed\", color = \"green\"];\n");

    fprintf(stream, "   NextFree");  // Inactive(free) edges [next]
    for (size_t curElem = list -> nextFree; curElem != list -> head; curElem = list -> next[curElem]) {
        fprintf(stream, "->l%zu", curElem);
    }
    fprintf(stream, "->NextFree[weight = 0, style = \"bold\", color = \"purple\"];\n"
                    "}\n");

    fclose(stream);

    char CmdBuffer[MAX_CMD_SIZE] = {0};
    sprintf(CmdBuffer, "dot -Tpng %s -o logs/logPic%zu.png", fileName, cmd);

    if (system(CmdBuffer)) return ListGraphVizExecutionErr;

    fprintf(LogGraph, "<center>\n<h1>\nCommand[%zu]\n</h1>\n</center>\n", cmd);
    fprintf(LogGraph, "<img src= logPic%zu.png />\n", cmd);

    return ListIsOk;
}

void printErrorMessage(int error) {
    if (error == 0) {
        myfPrintf(LogFile, "No Errors occured :)\n");
        return;
    }

    size_t numberOfErrors = 0;
    for (size_t currentError = 0; currentError < 20; currentError++)
        if (error & (1 << currentError))
            numberOfErrors++;
    myfPrintf(LogFile, "Program stopped with %d errors: \n", numberOfErrors);

    size_t currentError = 1;
    if (error & (1 <<  0))
        myfPrintf(LogFile, "%zu)  Struct List was nullptr!\n", currentError++);
    if (error & (1 <<  1))
        myfPrintf(LogFile, "%zu)  List is already full!\n", currentError++);
    if (error & (1 <<  2))
        myfPrintf(LogFile, "%zu)  List is already empty!\n", currentError++);
    if (error & (1 <<  3))
        myfPrintf(LogFile, "%zu)  list -> head cannot be erased!\n", currentError++);
    if (error & (1 <<  4))
        myfPrintf(LogFile, "%zu)  Data in struct List was Null!\n", currentError++);
    if (error & (1 <<  5))
        myfPrintf(LogFile, "%zu)  Next in struct List was Null!\n", currentError++);
    if (error & (1 <<  6))
        myfPrintf(LogFile, "%zu)  There is no element with this index!\n", currentError++);
    if (error & (1 <<  7))
        myfPrintf(LogFile, "%zu)  Prev in struct List was Null!\n", currentError++);
    if (error & (1 <<  8))
        myfPrintf(LogFile, "%zu)  Struct List was inactive!\n", currentError++);
    if (error & (1 <<  9))
        myfPrintf(LogFile, "%zu)  Data in struct List was POISONED!\n", currentError++);
    if (error & (1 << 10))
        myfPrintf(LogFile, "%zu)  The System of free blocks is damaged in struct List\n", currentError++);
    if (error & (1 << 11))
        myfPrintf(LogFile, "%zu)  Struct List was Destructed two times!\n", currentError++);
    if (error & (1 << 12))
        myfPrintf(LogFile, "%zu)  Struct List was Constructed two times!\n", currentError++);
    if (error & (1 << 13))
        myfPrintf(LogFile, "%zu)  GraphViz execution error!\n", currentError++);
}

void listLogClose() {
    if (LogFile != nullptr)
        fclose(LogFile);
}

void vizLogClose() {
    fclose(LogGraph);
}