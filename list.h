#include "listType.h"
#include <cstdio>

const int MAX_LIST_SIZE = 1000;

enum ListErrors {
              ListOk         =       0,
            ListIsNull       = 1 <<  1,
          ListIsInActive     = 1 <<  2,
          ListNullInData     = 1 <<  3,
       ListDoubleDestruction = 1 <<  4,
      ListDoubleConstruction = 1 <<  5,

};

enum Status {
     Active = 0,
    InActive = 1
}

struct Error {
    char *  name   = nullptr;
    char *  file   = nullptr;
    char * pointer = nullptr;
    char * message = nullptr;
    char *function = nullptr;
    bool  status  =    0   ; 
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

int _errorCtor(Error *list, const char * name, const char *file, const char *function, const size_t line);

int _listDtor(List *list);

int addData(List *list, Elem_t val, Error *err);

int eraseData(List *list, size_t element);
