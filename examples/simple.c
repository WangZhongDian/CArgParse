#include "ArgParse.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

ArgParse *Init() {
    ArgParse *ap = argParseInit("简单的命令行工具示例",NOVALUE);

    // 添加第一个命令
    Command *cmd = argParseAddCommand(
        ap, "list", "列出文件列表", NULL, NULL, NULL, SINGLEVALUE);

    // 添加第一个命令的参数
    argParseAddArg(cmd,
                   "-a",
                   "--all",
                   "列出所有文件包括隐藏文件",
                   NULL,
                   NULL,
                   false,
                   NOVALUE);

    return ap;
}

int main(int argc, char *argv[]) {
    ArgParse *ap = Init();
    argParseParse(ap, argc, argv);

    char *dir = argParseGetCurCommandValue(ap);
    if (dir != NULL) {
        printf("列出目录: %s 的文件列表\n", dir);
    }

    if (argParseCheckCurArgTriggered(ap, "-a")) {
        printf("触发了 -a 参数,列出所有文件\n");
    }

    argParseFree(ap);
    return 0;
}