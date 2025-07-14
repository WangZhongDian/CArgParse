#include "ArgParse.h"
#include "initArgParse.h"
#include <assert.h>

int main(int argc, char *argv[]) {
    ArgParse *argparse = initArgParse();

    argParseParse(argparse, argc, argv);


    argParseFree(argparse);

    return 0;
}