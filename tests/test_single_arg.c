#include "ArgParse.h"
#include "initArgParse.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ArgParse *argparse = initArgParse();

    argParseParse(argparse, argc, argv);

    const char *testv[3] = {"testpackge1", "testpackge2", "testpackge3"};

    char *val = argParseGetCurArg(argparse, "-p");

    int len = 0;
    char **vals = argParseGetCurArgList(argparse, "-p", &len);

    for (int i = 0; i < len; i++) {
        printf("-p Value: %s\n", vals[i]);
        assert(strcmp(vals[i], testv[i]) == 0);
    }

    argParseFree(argparse);

    return 0;
}