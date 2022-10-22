#include "listType.h"
#include <cstdio>

const int MAX_LIST_SIZE = 1000;

enum ListErrors {
             ListIsOk        =       0,
            ListIsNull       = 1 <<  0,
            ListIsFull       = 1 <<  1   
            ListIsEmpty      = 1 <<  2,
          ListDataIsNull     = 1 <<  3,
          ListNextIsNull     = 1 <<  4,
          ListPrevIsNull     = 1 <<  5,
          ListIsInActive     = 1 <<  6,
         ListPoisonInData    = 1 <<  7,
         ListFreeBlocksErr   = 1 <<  8,
       ListDoubleDestruction = 1 <<  9,
      ListDoubleConstruction = 1 << 10,
};

enum Status {
     Active = 0,
    InActive = 1
}

struct listInfo
{
    const size_t  line   =    0   ;

    const char *  name   = nullptr;
    const char *  file   = nullptr;
    const char * pointer = nullptr;
    const char *function = nullptr;
};


struct List {
    Elem_t data[MAX_LIST_SIZE] =    {}   ;
    size_t next[MAX_LIST_SIZE] =    {}   ;
    size_t prev[MAX_LIST_SIZE] =    {}   ;

    listInfo       info        =    {}   ;

    size_t         head        =    0    ;
    bool          status       = InActive;
    size_t       nextFree      =    0    ;

};

#define FILENAME_ (strrchr(__FILE__, '/') + 1)

#define listCtor(LIST) {                                                    \
    _listCtor(&(LIST), #LIST, FILENAME_, __PRETTY_FUNCTION__, __LINE__);    \
}

#define catchNullptr(POINTER) {
    if ((POINTER) == nullptr) {
        fprintf(stderr, "%s pointer at %s at %s(%d) is NULL\n", #POINTER, __PRETTY_FUNCTION__, FILENAME, __LINE__);
        return EXIT_FAILURE;
    }
}

int _listCtor(List *list, const char * name, const char *file, const char *function, const size_t line);

int listDtor(List *list);

int listLogicInsert(List *list, size_t ind, Elem_t val, size_t *err);

int listPhysInsert (List *list, size_t ind, Elem_t val, size_t *err) ;

int listLogicErase (List *list, size_t ind);

int listPhysErase  (List *list, size_t ind);

int  listGetPos    (List *list, size_t ind);

int  listVerify    (List *list);