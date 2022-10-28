#include "list.h"
#include <stdlib.h>

int main (int, const char* argv[]) {
    int  n  = 0;
    int cmd = 0;
    int pos = 0;
    int val = 0;
    int err = 0;

    List list = {};
    listCtor(&list);

    listDump(&list);

    FILE* f = (argv[1]? fopen (argv[1], "r") : stdout);

    fscanf(f, "%d", &n);
    for (int i = 0; i < n; i++) {
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
        fprintf(stderr, "--- %d ---\n", list.prev[list.head]);
        listPrint(&list);
        listDump(&list);
    }

    listGraphVizPhysDump (&list, "forGrVzP.dot");
    listGraphVizLogicDump(&list, "forGrVzL.dot");

    fprintf(stderr, "\nEverything is Ok, Dude, RELAX!\n");

    listDtor(&list);
    listLogClose();

    if (f != stdout) fclose (f);

    return EXIT_SUCCESS;
}
