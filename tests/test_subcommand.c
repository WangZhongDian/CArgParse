#include "ArgParse.h"
#include "initArgParse.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ArgParse *argparse = initArgParse();

    argParseParse(argparse, argc, argv);

    
    char *command_name = argParseGetCurCommandName(argparse);
    printf("command name: %s\n", command_name);
    assert(strcmp(command_name, "tools") == 0);

    char * val = argParseGetCurArg(argparse, "-t");
    printf("tools -t: %s\n", val);

    argParseFree(argparse);

    return 0;
}