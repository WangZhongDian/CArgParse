#include "ArgParse.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArgParse *initArgParse() {
    ArgParse *argparse    = argParseInit("测试程序", MULTIVALUE);
    Command  *command     = NULL;
    Command  *sub_command = NULL;

    // add global arguments
    argParseAddGlobalArg(argparse,
                         "-v",
                         "--version",
                         "Show version",
                         NULL,
                         NULL,
                         false,
                         NOVALUE);
    argParseAddGlobalArg(
        argparse, "-q", "--quiet", "Quiet mode", NULL, NULL, false, NOVALUE);

    // add arguments
    command = argParseAddCommand(
        argparse, "install", "Install the package", NULL, NULL, NULL, NOVALUE);
    argParseAddArg(command,
                   "-i",
                   "--index",
                   "Index URL",
                   "https://example.com",
                   NULL,
                   false,
                   SINGLEVALUE);
    argParseAddArg(command,
                   "-f",
                   "--file",
                   "Package file",
                   "package.json",
                   NULL,
                   false,
                   MULTIVALUE);
    argParseAddArg(command,
                   "-p",
                   "--package",
                   "Package file",
                   "package.json",
                   NULL,
                   false,
                   MULTIVALUE);

    sub_command = argParseAddSubCommand(
        command, "tools", "Install tools", NULL, NULL, NULL, MULTIVALUE);

    argParseAddArg(sub_command,
                   "-t",
                   "--tool",
                   "Tool name",
                   "Tool name",
                   NULL,
                   true,
                   MULTIVALUE);
    sub_command = argParseAddSubCommand(
        command, "tools_sub", "Install tools", NULL, NULL, NULL, MULTIVALUE);

    argParseAddArg(sub_command,
                   "-s",
                   "--source",
                   "test_source",
                   "tools subcommand test",
                   NULL,
                   true,
                   MULTIVALUE);

    command = argParseAddCommand(argparse,
                                 "uninstall",
                                 "Uninstall the package",
                                 NULL,
                                 NULL,
                                 NULL,
                                 SINGLEVALUE);
    argParseAddArg(command,
                   "-p",
                   "--package",
                   "Package name",
                   "Package name",
                   NULL,
                   true,
                   MULTIVALUE);

    return argparse;
}

int main(int argc, char *argv[]) {
    ArgParse *argparse = initArgParse();

    argParseParse(argparse, argc, argv);

    char *val = argParseGetVal(argparse);
    if (val) {
        printf("val: %s\n", val);
    }

    int    len  = 0;
    char **vals = argParseGetValList(argparse, &len);

    char *test_val[3] = {"file1.txt", "file2.txt", "file3.txt"};

    if (vals) {
        for (int i = 0; i < len; i++) {
            printf("vals: %s\n", vals[i]);
            assert(strcmp(vals[i], test_val[i]) == 0);
        }
    }

    assert(argParseCheckGlobalTriggered(argparse, "-v"));
    assert(argParseCheckGlobalTriggered(argparse, "-q"));

    argParseFree(argparse);

    return 0;
}