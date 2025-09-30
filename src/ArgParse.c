#include "ArgParse.h"
#include "ArgParseTools.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool _AutoHelp = true; // 是否自动添加帮助信息
static bool _COLOR    = true; // 是否启用颜色

void argParseDisableAutoHelp() { _AutoHelp = false; }

ArgParse *argParseInit(char *documentation, ArgParseValueType value_type) {
    ArgParse *argParse = malloc(sizeof(ArgParse));
    if (argParse == NULL) {
        return NULL;
    }

    argParse->commands        = NULL;
    argParse->commands_len    = 0;
    argParse->current_command = NULL;
    argParse->global_args     = NULL;
    argParse->global_args_len = 0;
    argParse->argc            = 0;
    argParse->argv            = NULL;
    argParse->documentation   = stringNewCopy(documentation);
    argParse->value_type      = value_type;
    argParse->val             = NULL;
    argParse->val_len         = 0;

    argParseAutoHelp(argParse);

    return argParse;
}

/**
 * @brief 自动帮助信息回调函数
 */
int __helpCallback(ArgParse *argParse, char **val, int val_len) {
    if (argParse == NULL) {
        return -1;
    }
    char *help_doc = argParseGenerateHelp(argParse);
    printf("%s\n", help_doc);
    free(help_doc);
    return 0;
}

/**
 * @brief 自动添加帮助信息
 * @param argParse ArgParse结构体指针
 */
void argParseAutoHelp(ArgParse *argParse) {
    if (!_AutoHelp) {
        return;
    }
    if (argParse == NULL) {
        return;
    }

    argParseAddGlobalArg(argParse,
                         "-h",
                         "--help",
                         "show help",
                         NULL,
                         __helpCallback,
                         false,
                         NOVALUE);
}

int __commandHelpCallback(ArgParse *argParse, char **val, int val_len) {
    if (argParse == NULL) {
        return -1;
    }
    char *help_doc = argParseGenerateHelpForCommand(argParse->current_command);
    printf("%s", help_doc);
    free(help_doc);
    exit(0);
    return 0;
}

void argParseCommandAutoHelp(Command *command) {
    if (!_AutoHelp) {
        return;
    }
    if (command == NULL) {
        return;
    }

    argParseAddArg(command,
                   "-h",
                   "--help",
                   "show help",
                   NULL,
                   __commandHelpCallback,
                   false,
                   NOVALUE);
}

Command *argParseAddCommand(ArgParse         *argParse,
                            char             *name,
                            char             *help,
                            char             *default_val,
                            ArgParseCallback  callback,
                            CommandGroup     *group,
                            ArgParseValueType value_type) {

    if (argParse == NULL || name == NULL) {
        return NULL;
    }
    // 构造命令结构
    Command *command =
        createCommand(name, help, default_val, callback, group, value_type);

    // 将命令结构添加到argParse中
    argParse->commands = realloc(
        argParse->commands, sizeof(Command *) * (argParse->commands_len + 1));
    if (argParse->commands == NULL) {
        return NULL;
    }
    argParse->commands[argParse->commands_len] = command;
    argParse->commands_len++;

    argParseCommandAutoHelp(command);

    return command;
}

/**
 * @brief 添加子命令
 * @param Parent 父命令
 * @param name 子命令名称
 * @param help 子命令帮助信息
 * @param callback 子命令回调函数
 * @param group 子命令组
 * @param value_type 子命令值类型
 * @return
 */
Command *argParseAddSubCommand(Command          *Parent,
                               char             *name,
                               char             *help,
                               char             *default_val,
                               ArgParseCallback  callback,
                               CommandGroup     *group,
                               ArgParseValueType value_type) {

    if (Parent == NULL || name == NULL) {
        return NULL;
    }
    // 构造命令结构
    Command *command =
        createCommand(name, help, default_val, callback, group, value_type);
    if (command == NULL) {
        return NULL;
    }
    // 将命令结构添加到Parent中
    Parent->sub_commands =
        realloc(Parent->sub_commands,
                sizeof(Command *) * (Parent->sub_commands_len + 1));
    if (Parent->sub_commands == NULL)
        return NULL;
    Parent->sub_commands[Parent->sub_commands_len] = command;
    Parent->sub_commands_len++;

    argParseCommandAutoHelp(command);

    return command;
}

CommandArgs *argParseAddArg(Command          *command,
                            char             *short_opt,
                            char             *long_opt,
                            char             *help,
                            char             *default_val,
                            ArgParseCallback  callback,
                            bool              required,
                            ArgParseValueType value_type) {

    if (command == NULL) {
        return NULL;
    }
    // 构造参数结构
    CommandArgs *arg = createCommandArgs(
        short_opt, long_opt, default_val, help, callback, required, value_type);
    if (arg == NULL) {
        return NULL;
    }
    // 将参数结构添加到command中
    command->args =
        realloc(command->args, sizeof(CommandArgs *) * (command->args_len + 1));
    if (command->args == NULL) {
        return NULL;
    }
    command->args[command->args_len] = arg;
    command->args_len++;

    return arg;
}

CommandArgs *argParseAddGlobalArg(ArgParse         *argParse,
                                  char             *short_opt,
                                  char             *long_opt,
                                  char             *help,
                                  char             *default_val,
                                  ArgParseCallback  callback,
                                  bool              required,
                                  ArgParseValueType value_type) {

    if (argParse == NULL) {
        return NULL;
    }
    // 构造参数结构
    CommandArgs *arg = createCommandArgs(
        short_opt, long_opt, default_val, help, callback, required, value_type);
    if (arg == NULL) {
        return NULL;
    }
    // 将参数结构添加到argParse中
    argParse->global_args =
        realloc(argParse->global_args,
                sizeof(CommandArgs *) * (argParse->global_args_len + 1));
    if (argParse->global_args == NULL) {
        return NULL;
    }
    argParse->global_args[argParse->global_args_len] = arg;
    argParse->global_args_len++;

    return arg;
}

/** Start----------------内存释放API---------------- */
void __freeCommandArgs(CommandArgs *arg) {
    if (arg == NULL) {
        return;
    }

    free(arg->short_opt);
    free(arg->long_opt);
    free(arg->help);
    free(arg->default_val);
    for (size_t i = 0; i < arg->val_len; i++) {
        free(arg->val[i]);
    }
    free(arg->val);
    free(arg);
}

void __freeCommand(Command *command) {
    if (command == NULL) {
        return;
    }

    for (size_t i = 0; i < command->args_len; i++) {
        __freeCommandArgs(command->args[i]);
    }

    for (size_t i = 0; i < command->sub_commands_len; i++) {
        __freeCommand(command->sub_commands[i]);
    }
    free(command->sub_commands);
    free(command->name);
    free(command->help);
    free(command->args);
    free(command->default_val);

    if (command->val_len > 0) {
        for (size_t i = 0; i < command->val_len; i++) {
            free(command->val[i]);
        }
        free(command->val);
    }

    free(command);
}

void argParseFree(ArgParse *argParse) {
    // 释放命令及其子命令与参数
    for (size_t i = 0; i < argParse->commands_len; i++) {
        __freeCommand(argParse->commands[i]);
    }
    free(argParse->commands);

    // 释放全局参数
    for (size_t i = 0; i < argParse->global_args_len; i++) {
        __freeCommandArgs(argParse->global_args[i]);
    }
    if (argParse->val_len > 0) {
        for (size_t i = 0; i < argParse->val_len; i++) {
            free(argParse->val[i]);
        }
        free(argParse->val);
    }

    free(argParse->global_args);
    free(argParse->documentation);
    free(argParse);
}
/** End----------------内存释放API---------------- */

/** Start----------------解析API---------------- */

/**
 * @brief 解析命令行参数
 * @param argParse ArgParse结构体指针
 * @param command 命令结构体指针
 * @param arg 参数结构体指针
 * @param arg_index 参数索引
 * @return int 返回解析到的参数索引
 */
int __processArgs(ArgParse *argParse, CommandArgs *arg, int arg_index) {
    arg->is_trigged   = true; // 标记参数被触发

    int current_index = arg_index;

    if (arg->value_type == MULTIVALUE) {
        for (int i = arg_index + 1; i < argParse->argc; i++) {
            if (checkArgType(argParse->argv[i]) ==
                COMMAND) { // COMMAND是无--或-开头的字符串，也可认定为参数值
                argParseSetArgVal(arg, argParse->argv[i]);
                current_index = i;
            } else {
                current_index = i - 1;
                break;
            }
        }
    } else if (arg->value_type == SINGLEVALUE) {
        if (arg_index + 1 < argParse->argc) {
            argParseSetArgVal(arg, argParse->argv[arg_index + 1]);
            current_index = arg_index + 1;
        }
    } else if (arg->value_type == NOVALUE) {
        current_index = arg_index;
    }

    if (arg->callback != NULL) {
        arg->callback(argParse, arg->val, arg->val_len);
    }

    return current_index;
}

// 处理子命令参数
int __processSubCommand(ArgParse *argParse,
                        Command  *ParentCommand,
                        char     *name,
                        int       command_index) {

    Command *sub_command = argParseFindSubCommand(ParentCommand, name);
    if (sub_command == NULL) {
        argParseError(argParse, argParse->current_command, NULL, NULL);
        return -1;
    }
    CommandArgs *arg          = NULL;

    argParse->current_command = sub_command;

    for (int i = command_index + 1; i < argParse->argc; i++) {
        ArgType argType = checkArgType(argParse->argv[i]);
        switch (argType) {
        case COMMAND:
            return __processSubCommand(
                argParse, sub_command, argParse->argv[i], i); // 递归处理子命令
        case LONG_ARG:
            arg =
                argParseFindCommandArgs(sub_command, argParse->argv[i], false);
            if (arg != NULL) {
                i = __processArgs(argParse, arg, i);
            }
            return i;
        case SHORT_ARG:
            arg = argParseFindCommandArgs(sub_command, argParse->argv[i], true);
            if (arg != NULL) {
                i = __processArgs(argParse,
                                  arg,
                                  i); // 解析参数值并返回以解析到的索引位置
                return i;
            } else {
                argParseError(argParse, argParse->current_command, NULL, NULL);
                return -1;
            }
        default:
            argParseError(argParse, argParse->current_command, NULL, NULL);
            return -1;
        }
    }
    argParseError(argParse, argParse->current_command, NULL, NULL);
    return -1;
}

int __processVal(ArgParse *argParse, int index) {
    CommandArgs *arg = NULL;

    for (int i = index; i < argParse->argc; i++) {
        ArgType argType = checkArgType(argParse->argv[i]);
        switch (argType) {
        case COMMAND:
            argParseSetVal(argParse, argParse->argv[i]);
            break;
        case LONG_ARG:
            // 处理全局命令长选项
            arg = argParseFindGlobalArgs(argParse, argParse->argv[i], false);
            if (arg == NULL) {
                char *msg = argParseGenerateArgErrorMsg(
                    argParse, argParse->argv[i], false);
                argParseError(argParse, argParse->current_command, msg, NULL);
                return -1;
            }
            i = __processArgs(argParse, arg, i);
            break;
        case SHORT_ARG:
            // 处理全局命令短选项
            arg = argParseFindGlobalArgs(argParse, argParse->argv[i], true);
            if (arg == NULL) {
                char *msg = argParseGenerateArgErrorMsg(
                    argParse, argParse->argv[i], true);
                argParseError(argParse, argParse->current_command, msg, NULL);
                return -1; // 错误处理
            }
            i = __processArgs(
                argParse, arg, i); // 解析参数值并返回以解析到的索引位置
            break;
        default:
            argParseError(
                argParse, argParse->current_command, "unknown error", NULL);
            return -1;
        }
    }
    return argParse->argc - 1;
}

// 处理命令参数
int __processCommand(ArgParse *argParse, char *name, int command_index) {
    CommandArgs *arg     = NULL;
    Command     *command = NULL;

    command              = argParseFindCommand(argParse, name); // 查找命令

    if (command == NULL && argParse->value_type == NOVALUE) {
        char *msg = NULL;
        if (name != NULL) {
            msg = stringNewCopy("\033[1;31mERROR\033[0m:");
            __catStr(&msg, 1, name);
            __catStr(&msg, 1, " is not a valid command");
        }

        argParseError(argParse, command, msg, NULL);
        return -1;
    }

    if (command == NULL && argParse->value_type != NOVALUE) {
        return __processVal(argParse, command_index);
    }

    if (command != NULL) {
        command->is_trigged       = true; // 标记命令被触发
        argParse->current_command = command;
    }

    for (int i = command_index + 1; i < argParse->argc; i++) {
        ArgType argType = checkArgType(argParse->argv[i]);
        switch (argType) {
        case COMMAND: {
            // 命令无值则处理子命令
            if (command->value_type == NOVALUE) {
                __processSubCommand(argParse, command, argParse->argv[i], i);
                return argParse->argc - 1;
            } else {
                // 命令有值，则认为该值是命令值
                argParseSetCommandVal(command, argParse->argv[i]);
            }
            break;
        }
        case LONG_ARG:
            // 处理命令长选项
            arg = argParseFindCommandArgs(command, argParse->argv[i], false);
            if (arg == NULL) {
                // 可能全局参数被放在了命令参数之前
                arg =
                    argParseFindGlobalArgs(argParse, argParse->argv[i], false);
                if (arg == NULL) {
                    char *msg = argParseGenerateArgErrorMsg(
                        argParse, argParse->argv[i], false);
                    argParseError(
                        argParse, argParse->current_command, msg, NULL);
                    return -1;
                }
                i = __processArgs(argParse, arg, i);
                break;
            }
            i = __processArgs(argParse, arg, i);
            break;
        case SHORT_ARG:
            // 处理命令短选项
            arg = argParseFindCommandArgs(command, argParse->argv[i], true);
            if (arg == NULL) {
                // 可能全局参数被放在了命令参数之前
                arg = argParseFindGlobalArgs(argParse, argParse->argv[i], true);
                if (arg == NULL) {
                    char *msg = argParseGenerateArgErrorMsg(
                        argParse, argParse->argv[i], true);
                    argParseError(
                        argParse, argParse->current_command, msg, NULL);
                    return -1;
                }
                i = __processArgs(
                    argParse, arg, i); // 解析参数值并返回以解析到的索引位置
                break;
            }
            i = __processArgs(argParse,
                              arg,
                              i); // 解析参数值并返回以解析到的索引位置
            break;
        default:
            argParseError(argParse, argParse->current_command, NULL, NULL);
            return -1;
        }
    }

    return argParse->argc - 1;
}

/**
 * @brief 解析命令行参数
 * @param argParse 解析器指针
 * @param argc 参数个数
 * @param argv 参数列表
 */
void argParseParse(ArgParse *argParse, int argc, char *argv[]) {
    argParse->argc   = argc;
    argParse->argv   = argv;

    CommandArgs *arg = NULL;

    for (int i = 1; i < argc; i++) {
        ArgType argType = checkArgType(argv[i]);
        switch (argType) {
        case COMMAND:
            // 处理命令
            i = __processCommand(argParse, argv[i], i);
            break;
        case LONG_ARG: // 处理全局长选项
            arg = argParseFindGlobalArgs(argParse, argv[i], false);
            if (arg == NULL) {
                char *msg =
                    argParseGenerateArgErrorMsg(argParse, argv[i], false);
                argParseError(argParse, argParse->current_command, msg, NULL);
                return; // 错误处理
            }
            i = __processArgs(argParse, arg, i);
            break;
        case SHORT_ARG: // 处理全局短选项
            arg = argParseFindGlobalArgs(argParse, argv[i], true);
            if (arg == NULL) {
                char *msg =
                    argParseGenerateArgErrorMsg(argParse, argv[i], true);
                argParseError(argParse, argParse->current_command, msg, NULL);
                return; // 错误处理
            }
            i = __processArgs(
                argParse, arg, i); // 解析参数值并返回以解析到的索引位置
            break;
        default:
            break;
        }
    }

    // 执行当前命令的回调函数
    if (argParse->current_command != NULL &&
        argParse->current_command->is_trigged &&
        argParse->current_command->callback != NULL) {
        argParse->current_command->callback(argParse,
                                            argParse->current_command->val,
                                            argParse->current_command->val_len);
    }
}

/**
 * @brief 获取当前解析到的命令名,会返回最后一个解析到的命令名(子命令)
 * @param argParse 解析器指针
 * @return char* 命令名
 */
char *argParseGetCurCommandName(ArgParse *argParse) {
    if (argParse == NULL)
        return NULL;

    if (argParse->current_command == NULL) {
        return NULL;
    }
    return argParse->current_command->name;
}

/**
 * @brief 获取当前解析到的命令参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @return char* 选项值
 * @return
 */
char *argParseGetCurArg(ArgParse *argParse, char *opt) {
    CommandArgs *arg = NULL;
    // 尝试获取当前命令的长选项参数
    arg = argParseFindCommandArgs(argParse->current_command, opt, false);

    if (arg == NULL) {
        // 尝试获取当前命令的短选项参数
        arg = argParseFindCommandArgs(argParse->current_command, opt, true);
    }

    if (arg == NULL) {
        return NULL;
    }
    if (arg->val_len >= 1) {
        return arg->val[0];
    }
    return NULL;
}

/**
 * @brief 获取当前解析到的命令参数列表,仅适用于多值参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @param len 参数个数buffer
 * @return char** 参数列表
 */
char **argParseGetCurArgList(ArgParse *argParse, char *opt, int *len) {
    CommandArgs *arg = NULL;
    arg = argParseFindCommandArgs(argParse->current_command, opt, false);

    if (arg == NULL) {
        arg = argParseFindCommandArgs(argParse->current_command, opt, true);
    }

    if (arg == NULL) {
        return NULL;
    }
    *len = arg->val_len;
    return arg->val;
}

/**
 * @brief 获取全局参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @return char* 选项值
 */
char *argParseGetGlobalArg(ArgParse *argParse, char *opt) {
    CommandArgs *arg = NULL;
    arg              = argParseFindGlobalArgs(argParse, opt, false);

    if (arg == NULL) {
        arg = argParseFindGlobalArgs(argParse, opt, true);
    }

    if (arg == NULL) {
        return NULL;
    }
    if (arg->val_len >= 1) {
        return arg->val[0];
    }
    return NULL;
}

/**
 * @brief 获取当前解析到的命令参数值
 * @return  char* 参数值
 */
char *argParseGetCurCommandValue(ArgParse *argParse) {
    Command *command = argParse->current_command;
    if (command == NULL) {
        return NULL;
    }
    if (command->val_len >= 1) {
        return command->val[0];
    }
    return NULL;
}
/**
 * @brief 获取当前解析到的命令参数列表,仅适用于多值参数
 * @return char** 参数列表
 */
char **argParseGetCurCommandValues(ArgParse *argParse, int *len) {
    Command *command = argParse->current_command;
    if (command == NULL) {
        return NULL;
    }
    if (command->val_len >= 1) {
        *len = command->val_len;
        return command->val;
    }
    return NULL;
}

/**
 * @brief 获取全局参数列表,仅适用于多值参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @param len 参数个数buffer
 * @return char** 参数列表
 */
char **argParseGetGlobalArgList(ArgParse *argParse, char *opt, int *len) {
    CommandArgs *arg = NULL;
    arg              = argParseFindGlobalArgs(argParse, opt, false);

    if (arg == NULL) {
        arg = argParseFindGlobalArgs(argParse, opt, true);
    }

    if (arg == NULL) {
        return NULL;
    }
    *len = arg->val_len;
    return arg->val;
}

char **argParseGetValList(ArgParse *argParse, int *len) {
    if (argParse == NULL) {
        return NULL;
    }

    if (argParse->val_len == 0) {
        return NULL;
    }
    *len = argParse->val_len;
    return argParse->val;
}

char *argParseGetVal(ArgParse *argParse) {
    if (argParse == NULL) {
        return NULL;
    }

    if (argParse->val_len == 0) {
        return NULL;
    }
    return argParse->val[0];
}

char *argParseGenerateHelpForCommand(Command *command) {
    if (command == NULL) {
        return NULL;
    }

    char *help_msg = stringNewCopy(command->help);
    if (help_msg == NULL) {
        return NULL;
    }

    __catStr(&help_msg, 1, "\n\n");
    __catStr(&help_msg, 2, "\033[1;33mUsage\033[0m: ", command->name);

    switch (command->value_type) {
    case NOVALUE:
        break;
    case SINGLEVALUE:
        __catStr(&help_msg, 1, " <value>");
        break;
    case MULTIVALUE:
        __catStr(&help_msg, 1, " <value>...");
    }

    if (command->args != NULL) {
        __catStr(&help_msg, 1, " [Options]");
        __catStr(&help_msg, 1, "\n\n\033[1;34mOptions\033[0m:");
        for (int i = 0; i < command->args_len; i++) {
            CommandArgs *arg = command->args[i];

            __catStr(&help_msg,
                     6,
                     "\n  \033[1;32m",
                     arg->short_opt,
                     "\033[0m , \033[1;32m",
                     arg->long_opt,
                     "\033[0m  ",
                     arg->help);
        }
    }
    __catStr(&help_msg, 1, "\n");

    return help_msg;
}

//  检查全局参数是否被触发
bool argParseCheckGlobalTriggered(ArgParse *argParse, char *opt) {
    CommandArgs *arg = NULL;
    arg              = argParseFindGlobalArgs(argParse, opt, false);

    if (arg == NULL) {
        arg = argParseFindGlobalArgs(argParse, opt, true);
    }

    if (arg == NULL) {
        return false;
    }
    return arg->is_trigged;
}
// 检测当前命令的某项参数是否被触发
bool argParseCheckCurArgTriggered(ArgParse *argParse, char *opt) {
    CommandArgs *arg = NULL;
    arg = argParseFindCommandArgs(argParse->current_command, opt, false);

    if (arg == NULL) {
        arg = argParseFindCommandArgs(argParse->current_command, opt, true);
    }

    if (arg == NULL) {
        return false;
    }
    return arg->is_trigged;
}
// 检测指定命令是否被触发
bool argParseCheckCommandTriggered(ArgParse *argParse, char *command_name) {
    Command *command = argParseFindCommand(argParse, command_name);
    if (command == NULL) {
        return false;
    }
    return command->is_trigged;
}

NORETURN void argParseError(ArgParse   *argParse,
                             Command    *lastCommand,
                             const char *prefix,
                             const char *suffix) {
    if (argParse == NULL) {
        printf("ERROR: Parse is NULL\n");
        exit(1);
    }
    if (lastCommand == NULL) {
        char *mgs = stringNewCopy("");
        if (prefix != NULL) {
            __catStr(&mgs, 1, prefix);
        }
        char *help = argParseGenerateHelp(argParse);

        if (help != NULL) {
            __catStr(&mgs, 2, "\n", help);
        }

        if (suffix != NULL) {
            __catStr(&mgs, 2, "\n", suffix);
        }

        // printf("\033[1;31mERROR\033[0m: Last command is unknown\n");
        printf("%s\n", mgs);
        free(mgs);
        free(help);
        argParseFree(argParse);
        exit(1);
    }

    char *ErrorMsg = NULL;

    if (prefix != NULL) {
        ErrorMsg = stringNewCopy((char *)prefix);
    }

    char *command_help_msg     = argParseGenerateHelpForCommand(lastCommand);
    int   command_help_msg_len = strlen(command_help_msg);

    if (command_help_msg != NULL) {
        __catStr(&ErrorMsg, 2, "\n", command_help_msg);
    }

    if (suffix != NULL) {
        __catStr(&ErrorMsg, 2, "\n", suffix);
    }

    printf("%s\n", ErrorMsg);
    free(ErrorMsg);
    free(command_help_msg);
    argParseFree(argParse);
    exit(1);
}

char *
argParseGenerateArgErrorMsg(ArgParse *argParse, char *name, bool short_flag) {
    if (argParse == NULL) {
        return NULL;
    }
    if (name == NULL) {
        return NULL;
    }

    char *ErrorMsg = NULL;

    if (_COLOR) {
        ErrorMsg = stringNewCopy("\033[1;31mERROR\033[0m: Invalid argument ");
    } else {
        ErrorMsg = stringNewCopy("ERROR: Invalid argument ");
    }

    if (short_flag) {
        __catStr(&ErrorMsg, 1, name);
    } else {
        __catStr(&ErrorMsg, 1, name);
    }
    return ErrorMsg;
}

char *argParseGenerateHelp(ArgParse *argParse) {
    if (argParse == NULL) {
        return NULL;
    }

    char *help_msg = stringNewCopy(argParse->documentation);

    if (help_msg == NULL) {
        return NULL;
    }

    __catStr(&help_msg, 1, "\n\n\033[1;34mCommands\033[0m:");
    for (int i = 0; i < argParse->commands_len; i++) {
        Command *command = argParse->commands[i];

        __catStr(&help_msg,
                 4,
                 "\n  \033[1;32m",
                 command->name,
                 "\033[0m  ",
                 command->help);
    }

    if (argParse->global_args_len > 0) {
        __catStr(&help_msg, 1, "\n\n\033[1;34mGlobal Options\033[0m:");
        for (int i = 0; i < argParse->global_args_len; i++) {
            CommandArgs *arg = argParse->global_args[i];

            __catStr(&help_msg,
                     6,
                     "\n  \033[1;32m",
                     arg->short_opt,
                     "\033[0m , \033[1;32m",
                     arg->long_opt,
                     "\033[0m  ",
                     arg->help);
        }
    }

    return help_msg;
}

_Noreturn void argParseHelp(ArgParse *argParse) {
    char *help_msg = argParseGenerateHelp(argParse);
    printf("%s\n", help_msg);
    free(help_msg);
    exit(0);
}