#include "list.h"
#include <stdlib.h>

int main (int argc, const char* argv[]) {
    int  n  = 0;
    int cmd = 0;
    int pos = 0;
    int val = 0;
    int err = 0;

    List list = {};
    listCtor(&list);

    // listDump(&list);

    FILE* f = (argc > 1? fopen (argv[1], "r") : stdin);

    fscanf(f, "%d", &n);
    for (size_t i = 0; i < n; i++) {
        fscanf(f, "%d %d", &cmd, &pos);
        if (cmd) {
            fscanf(f, "%d", &val);
            listLogicInsert(&list, pos, val, &err);
            if (err) {
                printErrorMessage(err);
                return EXIT_FAILURE;
            }
        } else {
            printErrorMessage(listLogicErase(&list, pos));
        }
        //fprintf(stderr, "--- %d ---\n", list.prev[list.head]);
        // fprintf(logStream, "Command number %zu\n", i);
        listGraphVizPhysDump (&list, "logs/listImage.dot", i + 1);
        listPrint(&list);
        listDump(&list);
    }

    listGraphVizLogicDump(&list, "forGrVzL.dot");

    fprintf(stderr, "\nEverything is Ok, Dude, RELAX!\n");

    listDtor(&list);
    listLogClose();
    vizLogClose();

    if (f != stdin) fclose (f);

    return EXIT_SUCCESS;
}
