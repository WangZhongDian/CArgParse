#include "ArgParseTools.h"
#include "ArgParse.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 分配新内存复制字符串
 * @param str 字符串
 * @return 复制后的字符串内存指针
 */
char *stringNewCopy(char *str) {
    if (str == NULL) {
        return NULL;
    }
    char *mem = malloc(strlen(str) + 1);
    if (mem == NULL) {
        return NULL;
    }
    strcpy(mem, str);
    return mem;
}

// 构造命令
Command *createCommand(char             *name,
                       char             *help,
                       char             *default_val,
                       ArgParseCallback  callback,
                       CommandGroup     *group,
                       ArgParseValueType value_typ) {
    // 构造命令结构
    Command *command = malloc(sizeof(Command));
    if (command == NULL || name == NULL) {
        return NULL;
    }

    command->name = stringNewCopy(name);
    if (command->name == NULL) {
        return NULL;
    }

    command->help             = stringNewCopy(help);
    command->default_val      = stringNewCopy(default_val);
    command->callback         = callback;

    command->sub_commands     = NULL;
    command->sub_commands_len = 0;
    command->args             = NULL;
    command->args_len         = 0;
    command->value_type       = value_typ;

    if (group != NULL) {
        command->group = group;
    } else {
        command->group = NULL;
    }

    command->val        = NULL;
    command->val_len    = 0;
    command->is_trigged = false; // 是否被触发
    return command;
}

ArgType checkArgType(char *arg) {
    size_t len = strlen(arg);
    if (len == 0) {
        return BAD;
    }
    if (arg[0] == '-') {
        if (len >= 2) {
            if (arg[1] == '-') {
                return LONG_ARG;
            } else {
                return SHORT_ARG;
            }
        }
    } else {
        return COMMAND;
    }
    return BAD;
}

// 寻找命令
Command *argParseFindCommand(ArgParse *argParse, char *name) {
    for (size_t i = 0; i < argParse->commands_len; i++) {
        if (strcmp(argParse->commands[i]->name, name) == 0) {
            return argParse->commands[i];
        }
    }
    return NULL;
}

Command *argParseFindSubCommand(Command *command, char *name) {
    for (size_t i = 0; i < command->sub_commands_len; i++) {
        if (strcmp(command->sub_commands[i]->name, name) == 0) {
            return command->sub_commands[i];
        }
    }
    return NULL;
}

CommandArgs *createCommandArgs(char             *short_opt,
                               char             *long_opt,
                               char             *default_val,
                               char             *help,
                               ArgParseCallback  callback,
                               bool              required,
                               ArgParseValueType value_type) {
    CommandArgs *args = malloc(sizeof(CommandArgs));

    if (short_opt == NULL && long_opt == NULL) {
        return NULL;
    }

    // 分配内存
    args->short_opt   = stringNewCopy(short_opt);
    args->long_opt    = stringNewCopy(long_opt);
    args->default_val = stringNewCopy(default_val);
    args->help        = stringNewCopy(help);

    args->callback    = callback;
    args->required    = required;
    args->value_type  = value_type;

    // 解析属性初始化
    args->val         = NULL;
    args->val_len     = 0;
    args->is_trigged  = false; // 是否被触发

    return args;
}

CommandArgs *
argParseFindCommandArgs(Command *command, char *name, bool short_flag) {
    if (command == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < command->args_len; i++) {
        if (short_flag) {
            if (command->args[i]->short_opt != NULL &&
                strcmp(command->args[i]->short_opt, name) == 0) {
                return command->args[i];
            }
        } else {
            if (command->args[i]->long_opt != NULL &&
                strcmp(command->args[i]->long_opt, name) == 0) {
                return command->args[i];
            }
        }
    }
    return NULL;
}

CommandArgs *
argParseFindGlobalArgs(ArgParse *argParse, char *name, bool short_flag) {
    for (size_t i = 0; i < argParse->global_args_len; i++) {
        if (short_flag) {
            if (argParse->global_args[i]->short_opt != NULL &&
                strcmp(argParse->global_args[i]->short_opt, name) == 0) {
                return argParse->global_args[i];
            }
        } else {
            if (argParse->global_args[i]->long_opt != NULL &&
                strcmp(argParse->global_args[i]->long_opt, name) == 0) {
                return argParse->global_args[i];
            }
        }
    }
    return NULL;
}

/**
 * @brief 设置命令参数值
 * @param args 参数
 * @param val 值
 * @return 成功返回true，失败返回false
 */
bool argParseSetArgVal(CommandArgs *args, char *val) {
    if (args->value_type == MULTIVALUE) { // 多值
        args->val = realloc(args->val, (args->val_len + 1) * sizeof(char *));
        if (args->val == NULL) {
            return false;
        }
        args->val[args->val_len] = stringNewCopy(val);
        if (args->val[args->val_len] == NULL) {
            return false;
        }
        args->val_len++;
        return true;
    } else if (args->value_type == SINGLEVALUE) { // 单值
        if (args->val != NULL) {
            free(args->val);
        }
        args->val = malloc(sizeof(char *));

        if (args->val == NULL) {
            return false;
        }
        args->val[0] = stringNewCopy(val); // 分配内存
        if (args->val[0] == NULL) {
            return false;
        }
        args->val_len = 1;
        return true;
    } else if (args->value_type == NOVALUE) { // 无值
        return true;
    }

    return false;
}

bool argParseSetCommandVal(Command *command, char *val) {
    if (command->value_type == MULTIVALUE) { // 多值
        command->val =
            realloc(command->val, (command->val_len + 1) * sizeof(char *));
        if (command->val == NULL) {
            return false;
        }
        command->val[command->val_len] = stringNewCopy(val);
        if (command->val[command->val_len] == NULL) {
            return false;
        }
        command->val_len++;
        return true;
    } else if (command->value_type == SINGLEVALUE) { // 单值
        if (command->val != NULL) {
            free(command->val);
        }
        command->val = malloc(sizeof(char *));

        if (command->val == NULL) {
            return false;
        }
        command->val[0] = stringNewCopy(val); // 分配内存
        if (command->val[0] == NULL) {
            return false;
        }
        command->val_len = 1;
        return true;
    } else if (command->value_type == NOVALUE) { // 无值
        return true;
    }
    return false;
}