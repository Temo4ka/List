#include "list.h"
#include <cstdlib>

const Elem_t  POISON  = 1e9 + 7;
const size_t DumpSize = 20;
const  int   DataFree = -1,

FILE *LogFile = fopen("logs.txt", "w");

#define listDump(LIST) {                                             \
    listDump_(&(LIST), __PRETTY_FUNCTION__, FILENAME_, __LINE__);    \
}

int _listCtor(List *list, const char * name, const char *file, const char *function, const size_t line) {
    catchNullptr(list);
    if (list -> status == Active) return ListDoubleConstruction;
    
    list -> data = (Elem_t *) calloc(MAX_LIST_SIZE, sizeof(Elem_t));
    catchNullptr(list -> data);
    for (size_t curElem = 1; curElem < MAX_LIST_SIZE; ++curElem) {
        list -> data[curElem] = POISON;
    }

    list -> next = (size_t *) calloc(MAX_LIST_SIZE, sizeof(size_t));
    catchNullptr(list -> next);
    for (size_t curElem = 0; curElem < MAX_LIST_SIZE; ++curElem) {
        list -> next[curElem] = curElem + 1;
    }

    list -> prev = (size_t *) calloc(MAX_LIST_SIZE, sizeof(size_t));
    catchNullptr(list -> data);
    for (size_t curElem = 1; curElem < MAX_LIST_SIZE; ++curElem) {
        list -> next[curElem] = DataFree;
    }

    list ->   head   = 0;
    list -> nextFree = 1;

    list -> info.  name   =   name  ;
    list -> info.  line   =   line  ;
    list -> info.  file   =   file  ;
    list -> info. pointer =   list  ;
    list -> info.function = function;

    list ->     status    =  Active ;

    return ListIsOk;
}

int listDtor(List * list) {
    catchNullptr(list);
    if (list -> status == InActive) return ListDoubleDestruction;

    free(list ->   data  );
    free(list ->   next  );
    free(list ->   file  );
    free(list -> function);

    list ->   head   =    -1   ;
    list -> nextFree =    -1   ;
    list ->  status  = InActive;

    return ListIsOk;

}

int listLogicInsert(List *list, size_t ind, Elem_t val, size_t *err) {
    if (list == nullptr) return 0;

    *err = listVerify(list);
    if (*err) return 0;
    // TODO: enum for 0
    if (list -> nextFree == list -> head) return ListIsEmpty;
    
    size_t prevPos = listGetPos(list, ind);
    if (prevPos == -1) return 0;
    
    size_t   newPos  = list ->   nextFree  ;

    list -> nextFree = list -> next[newPos];  
    
    next[newPos] =    next[prevPos]  ;
    prev[newPos] = prev[next[newPos]];

    prev[next[newPos]] = newPos;
    next[prev[newPos]] = newPos;

    data[newPos] = val;

    *err = listVerify(list);
    if (*err) return 0;

    return newPos;
}

int listPhysInsert(List *list, size_t ind, Elem_t val, size_t *err) {
    if (list == nullptr) return 0;

    *err = listVerify(list);
    if (*err) return 0;
    // TODO: enum for 0
    
    if (list -> nextFree == list -> head) return ListIsEmpty;


    if (list -> data[ind] == -1) return 0;
    
    size_t  prevPos  =        ind          ;
    size_t   newPos  = list ->   nextFree  ;
    list -> nextFree = list -> next[newPos];  
    
    next[newPos] =    next[prevPos]  ;
    prev[newPos] = prev[next[newPos]];

    prev[next[newPos]] = newPos;
    next[prev[newPos]] = newPos;

    data[newPos] = val;

    *err = listVerify(list);
    if (*err) return 0;

    return newPos;
}

int listLogicErase (List *list, size_t ind) {
    catchNullptr(list);

    *err = listVerify(list);
    if (*err) return *err;

    if (list -> next[list -> head] == list -> head) return ListIsEmpty;

    size_t curPos = listGetPos(ind);
    if (curPos == -1) return EXIT_FAILURE;
    
    next[prev[curPos]] = next[curPos];
    prev[next[curPos]] = prev[curPos];

    prev[curPos] =     DataFree    ;
    next[curPos] = list -> nextFree;
    data[curPos] =      POISON     ;

    list -> nextFree = curPos;

    return ListIsOk;
}

int listPhysErase (List *list, size_t ind) {
    catchNullptr(list);

    *err = listVerify(list);
    if (*err) return *err;

    size_t curPos = ind;
    if (curPos == -1) return EXIT_FAILURE;
    
    next[prev[curPos]] = next[curPos];
    prev[next[curPos]] = prev[curPos];

    prev[curPos] =     DataFree    ;
    next[curPos] = list -> nextFree;
    data[curPos] =      POISON     ;

    list -> nextFree = curPos;

    return ListIsOk;
}

int listGetPos(List *list, size_t index) {
    if (list == nullptr) return -1;

    size_t pos = list -> head;
    for (size_t cur = 0; cur < index; ++cur) {
        pos = list -> next[pos];
    }

    return pos;
}

int listVerifyData(List *list);

int listVerifyData(List *list);

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

int listVerifyData(List *list) {
    catchNullptr(list);

    for (size_t curPos = next[list -> head]; curPos != list -> head; curPos = next[curPos])
        if (list -> data[curPos] == POISON) return ListPoisonInData;

    return ListIsOk;
}

int listVerifyData(List *list) {
    catchNullptr(list);

    for (size_t curPos = list -> Free; curPos; curPos = next[curPos])
        if (list -> prev[curPos] != -1 || list -> data[curPos]) return ListPoisonInData;

    return ListIsOk;
}

void myfPrintf(FILE *stream = nullptr, const char *format = nullptr, ...) {
    if (format == nullptr)
        return;

    va_list arguments;
    va_start (arguments, format);
    if (stream != nullptr)
        vfprintf(stream, format, arguments);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
}

void listDump_(List *list, const char* functionName, const char *fileName, unsigned line) {
    myfPrintf(LogFile, "%s at %s(%u);\n", functionName, fileName, line);
    myfPrintf(LogFile, "List[%08X] '%s' at '%s' at %s(%u); ", list->info.pointer, list->info.name, list->info.function, list->info.file, list->info.line);
    
    for (size_t cur = 0; cur < MAX_LIST_SIZE; ++cur) {
        myPrintf(LogFile, "%03zu", cur);
    }
    myPrintf(LogFile, "\n", cur);

    for (size_t cur = 0; cur < MAX_LIST_SIZE; ++cur) {
        myPrintf(LogFile, "%03d", list -> data[cur]);
    }
    myPrintf(LogFile, "\n", cur);

    for (size_t cur = 0; cur < MAX_LIST_SIZE; ++cur) {
        myPrintf(LogFile, "%03d", list -> next[cur]);
    }
    myPrintf(LogFile, "\n", cur);

    for (size_t cur = 0; cur < MAX_LIST_SIZE; ++cur) {
        myPrintf(LogFile, "%03d", list -> prev[cur]);
    }
    myPrintf(LogFile, "\n", cur);
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
        myfPrintf(LogFile, "%zu)  Data in struct List was Null!\n", currentError++);
    if (error & (1 <<  4))
        myfPrintf(LogFile, "%zu)  Next in struct List was Null!\n", currentError++);
    if (error & (1 <<  5))
        myfPrintf(LogFile, "%zu)  Prev in struct List was Null!\n", currentError++);
    if (error & (1 <<  6))
        myfPrintf(LogFile, "%zu)  Struct List was inactive!\n", currentError++);
    if (error & (1 <<  7))
        myfPrintf(LogFile, "%zu)  Data in struct List was POISONED!\n", currentError++);
    if (error & (1 <<  8))
        myfPrintf(LogFile, "%zu)  The System of free blocks is damaged in struct List\n", currentError++);
    if (error & (1 <<  9))
        myfPrintf(LogFile, "%zu)  Struct List was Destructed two times!\n", currentError++);
    if (error & (1 << 10))
        myfPrintf(LogFile, "%zu)  Struct List was Constructed two times!\n", currentError++);
}

void listLogClose()  {
    if (LogFile != nullptr)
        fclose(LogFile);
}
