# CArgParse

C语言易用的命令参数分析


# 特性
- 自动生成帮助信息
- 默认值支持
- 子命令支持


# 安装

## cmake安装
```bash
cmake -S . -B build
cmake --build build
cmake --install build
```



## conan安装
将库构建为`conan`包
```bash
conan create .
```



## 示例程序
构建简单的命令行程序
```c
#include "ArgParse.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

ArgParse *Init() {
    ArgParse *ap = argParseInit("简单的命令行工具示例");

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
```

```bash
example -h
```
![帮助信息](docs/images/help.png)