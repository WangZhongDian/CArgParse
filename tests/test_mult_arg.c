#include "ArgParse.h"
#include "initArgParse.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ArgParse *argparse = initArgParse();

    argParseParse(argparse, argc, argv);

    const char *testv[3] = {"testpackge1", "testpackge2", "testpackge3"};

    // Test -p
    char *val            = argParseGetCurArg(argparse, "-p");

    int    len           = 0;
    char **vals          = argParseGetCurArgList(argparse, "-p", &len);

    for (int i = 0; i < len; i++) {
        printf("-p Value: %s\n", vals[i]);
        assert(strcmp(vals[i], testv[i]) == 0);
    }

    // Test -i
    char *val_i = argParseGetCurArg(argparse, "-i");
    printf("-i Value: %s\n", val_i);
    assert(strcmp(val_i, "www.test.com") == 0);

    // Test -f
    len                  = 0;
    const char *testf[2] = {"file1.txt", "file2.txt"};
    char      **val_f    = argParseGetCurArgList(argparse, "-f", &len);
    for (int i = 0; i < len; i++) {
        printf("-f Value: %s\n", val_f[i]);
        assert(strcmp(val_f[i], testf[i]) == 0);
    }

    argParseFree(argparse);

    return 0;
}