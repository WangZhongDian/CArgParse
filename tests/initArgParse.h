#include "ArgParse.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

ArgParse *initArgParse() {
    ArgParse *argparse    = argParseInit("测试程序");
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