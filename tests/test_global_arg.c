#include "ArgParse.h"
#include "initArgParse.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    ArgParse *argparse = initArgParse();

    argParseParse(argparse, argc, argv);

    char *val = argParseGetGlobalArg(argparse, "-v");

    if (argParseCheckGlobalTriggered(argparse, "-v")) {
        printf("Global arg -v triggered\n");
    }

    if (val != NULL) {
        printf("Global arg -v: %s\n", val);
    } else {
        printf("Global arg -v is NULL\n");
    }

    char *val_q = argParseGetGlobalArg(argparse, "-q");

    if (argParseCheckGlobalTriggered(argparse, "-q")) {
        printf("Global arg -q triggered\n");
    }

    if (val != NULL) {
        printf("Global arg -q: %s\n", val);
    } else {
        printf("Global arg -q is NULL\n");
    }

    argParseFree(argparse);

    return 0;
}