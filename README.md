# CArgParse

C语言易用的命令参数分析


# 特性
- 自动生成帮助信息
- 命令自动帮助信息
- 默认值支持
- 子命令支持
- 命令参数支持
- 全局参数
- 互斥参数(即将)



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


# API
该API将分为两类，一类是用于构造解析器，另一类将用于解析参数

## 构造解析器

1. 初始化解析器
```c
ArgParse *argParseInit(char *documentation，ArgParseValueType value_type);
```
- `documentation` 解析器的文档信息
- `valueType` 程序本身需要的值类型

2. 添加命令
```c
Command *argParseAddCommand(ArgParse         *argParse,
                            char             *name,
                            char             *help,
                            char             *default_val, 
                            ArgParseCallback  callback,
                            CommandGroup     *group,
                            ArgParseValueType value_type);
```
- `argParse` 解析器
- `name` 命令名称
- `help` 命令帮助信息
- `defaultVal` 默认值
- `callback` 命令回调函数(可选)
- `group` 命令组(可选)
- `valueType` 命令参数类型

3. 添加命令参数
```c
CommandArgs *argParseAddArg(Command          *command,
                            char             *short_opt,
                            char             *long_opt,
                            char             *help,
                            char             *default_val,
                            ArgParseCallback  callback,
                            bool              required,
                            ArgParseValueType value_type);
```
- `command` 命令
- `shortOpt` 短选项
- `longOpt` 长选项
- `help` 帮助信息
- `defaultVal` 默认值
- `callback` 回调函数(可选)
- `required` 是否必须
- `valueType` 参数类型

4. 添加子命令
```c
Command *argParseAddSubCommand(Command          *Parent,
                               char             *name,
                               char             *help,
                               char             *default_val,
                               ArgParseCallback  callback,
                               CommandGroup     *group,
                               ArgParseValueType value_type);
```
- `Parent` 父命令
- `name` 命令名称
- `help` 命令帮助信息
- `defaultVal` 默认值
- `callback` 回调函数(可选)
- `group` 命令组(可选)
- `valueType` 命令参数类型

5. 添加全局参数
```c
CommandArgs *argParseAddGlobalArg(ArgParse         *argParse,
                                  char             *short_opt,
                                  char             *long_opt,
                                  char             *help,
                                  char             *default_val,
                                  ArgParseCallback  callback,
                                  bool              required,
                                  ArgParseValueType value_type);
```
- `argParse` 解析器
- `shortOpt` 短选项
- `longOpt` 长选项
- `help` 帮助信息
- `defaultVal` 默认值
- `callback` 回调函数(可选)
- `required` 是否必须
- `valueType` 参数类型

## 解析参数API
1. 解析参数
```c
void argParseParse(ArgParse *argParse, int argc, char *argv[]);
```
- `argParse` 解析器
- `argc` 参数个数
- `argv` 参数列表

2. 获取当前解析到的命令名字
```c
char *argParseGetCurCommandName(ArgParse *argParse);
```
- `argParse` 解析器
> 注意，解析到的命令为最后一次出现的命令，也可能是子命令


3. 获取当前命令的值
```c
char *argParseGetCurCommandValue(ArgParse *argParse);
```
- `argParse` 解析器
> 注意，解析到的命令为最后一次出现的命令，也可能是子命令

4. 获取当前命令的某个参数的值
```c
char *argParseGetCurArg(ArgParse *argParse, char *opt);
```
- `argParse` 解析器
- `opt` 参数选项

5. 获取当前命令的某个选项的值列表(当为多值类型时使用该函数)
```c
char **argParseGetCurArgList(ArgParse *argParse, char *opt, int *len);
```
- `argParse` 解析器
- `opt` 参数选项
- `len` 参数个数

6. 获取全局选项的值
```c
char *argParseGetGlobalArg(ArgParse *argParse, char *opt);
```
- `argParse` 解析器
- `opt` 参数选项

7. 获取全局选项的值列表
```c
char **argParseGetGlobalArgList(ArgParse *argParse, char *opt, int *len);
```
- `argParse` 解析器
- `opt` 参数选项
- `len` 参数个数

8. 获取程序值
该值为非命令的值，类似`gcc main.c`，该类程序没有使用命令，因此该值就是`main.c`
```c
char  *argParseGetVal(ArgParse *argParse);
char **argParseGetValList(ArgParse *argParse, int *len);
```
- `argParse` 解析器
- `len` 参数个数


### 触发检测API
1. 检测当前检测的命令的某个选项是否触发
```c
bool argParseCheckCurArgTriggered(ArgParse *argParse, char *opt);
```
- `argParse` 解析器
- `opt` 参数选项


2. 检测某个命令是否被触发
```c
bool argParseCheckCommandTriggered(ArgParse *argParse, char *command_name);
```
- `argParse` 解析器
- `command_name` 命令名称

3. 检测某个全局选项是否被触发
```c
bool argParseCheckGlobalTriggered(ArgParse *argParse, char *opt);
```
- `argParse` 解析器
- `opt` 参数选项