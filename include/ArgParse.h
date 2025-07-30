#ifndef CARGPARSE_H
#define CARGPARSE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _MSC_VER
    #define NORETURN __declspec(noreturn)
#else
    #define NORETURN _Noreturn
#endif


#define ARG_DEFAULT_HELP_FLAG "--help"

typedef struct ArgParse ArgParse; // 解析器

typedef int (*ArgParseCallback)(ArgParse *argParse,
                                char    **val,
                                int       val_len); // 回调函数

typedef enum {
    NOVALUE = 0,     // 无值
    SINGLEVALUE,     // 单值 例如： -i https://www.baidu.com
    MULTIVALUE,      // 多值 例如: -s a b c 或 -s a -s b -s c等
} ArgParseValueType; // 值类型

typedef struct CommandArgs {
    /* 构造属性 */
    char             *short_opt;   // 短选项名， 例如： -h
    char             *long_opt;    // 长选项    例如： --help
    char             *default_val; // 默认值
    char             *help;        // 选项帮助信息
    ArgParseCallback  callback;    // 回调函数
    bool              required;    // 是否为必选参数
    ArgParseValueType value_type;  // 值类型
    /* 解析所用到的属性*/
    char **val;        // 解析到的值
    int    val_len;    // 解析到的值个数
    bool   is_trigged; // 是否被触发
} CommandArgs;

typedef struct CommandGroup {
    char            *name;         // 命令组名
    char            *help;         // 命令组帮助信息
    struct Command **commands;     // 命令
    int              commands_len; // 命令个数
} CommandGroup;

typedef struct Command {
    /* 构造属性 */
    char                *name;             // 命令名
    char                *help;             // 命令帮助信息
    CommandGroup        *group;            // 命令组
    char                *default_val;      // 默认值
    struct CommandArgs **args;             // 命令参数
    int                  args_len;         // 命令参数个数
    struct Command     **sub_commands;     // 子命令
    int                  sub_commands_len; // 子命令个数
    ArgParseCallback     callback;         // 回调函数
    ArgParseValueType    value_type;       // 值类型

    /* 解析所用到的属性*/
    char **val;        // 解析到的值
    int    val_len;    // 解析到的值个数
    bool   is_trigged; // 是否被触发
} Command;

typedef struct ArgParse {
    /* 构造属性 */
    struct Command     **commands;        // 命令
    int                  commands_len;    // 命令个数
    struct CommandArgs **global_args;     // 全局参数
    int                  global_args_len; // 全局参数个数
    char                *documentation;   // 帮助文档
    ArgParseValueType value_type; // 值类型 程序默认需要的值例如 gcc main.c

    /* 解析所用到的属性*/
    struct Command *current_command; // 当前解析到的命令
    char          **val;
    int             val_len;
    int             argc; // 参数个数
    char          **argv; // 参数列表
} ArgParse;

/** Start---------------构造API---------------- */

/**
 * @brief 初始化解析器
 * @param documentation 帮助文档
 * @param value_type 值类型,程序默认需要的值例如 gcc main.c
 * @return ArgParse* 解析器指针
 */
ArgParse *argParseInit(char *documentation, ArgParseValueType value_type);

/**
 * @brief 释放解析器
 * @param argParse 解析器指针
 * @return void
 */
void argParseFree(ArgParse *argParse);

/**
 * @brief 自动生成帮助信息
 * @param argParse 解析器指针
 * @return void
 */
void argParseAutoHelp(ArgParse *argParse);

/**
 * @brief 添加命令
 * @param argParse 解析器指针
 * @param name 命令名
 * @param help 命令帮助信息
 * @param group_name 命令组名
 * @return Command* 命令指针
 */
Command *argParseAddCommand(ArgParse         *argParse,
                            char             *name,
                            char             *help,
                            char             *default_val, // 默认值
                            ArgParseCallback  callback,
                            CommandGroup     *group,
                            ArgParseValueType value_type);

/**
 * @brief 添加子命令
 * @param Parent 父命令指针
 * @param name 命令名
 * @param help 命令帮助信息
 * @param callback 回调函数
 * @param value_type 值类型
 * @return Command* 子命令指针
 */
Command *argParseAddSubCommand(Command          *Parent,
                               char             *name,
                               char             *help,
                               char             *default_val,
                               ArgParseCallback  callback,
                               CommandGroup     *group,
                               ArgParseValueType value_type);

/**
 * @brief 添加命令参数
 * @param command 命令指针
 * @param long_opt 长选项名
 * @param short_opt 短选项名
 * @param default_val 默认值
 * @param help 选项帮助信息
 * @param required 是否为必选参数
 * @param value_type 值类型
 * @return CommandArgs* 参数指针
 */
CommandArgs *argParseAddArg(Command          *command,
                            char             *short_opt,
                            char             *long_opt,
                            char             *help,
                            char             *default_val,
                            ArgParseCallback  callback,
                            bool              required,
                            ArgParseValueType value_type);

/**
 * @brief 添加全局参数
 * @param argParse 解析器指针
 * @param short_opt 短选项名
 * @param long_opt 长选项名
 * @param default_val 默认值
 * @param help 选项帮助信息
 * @param required 是否为必选参数
 * @param value_type 值类型
 * @return
 */
CommandArgs *argParseAddGlobalArg(ArgParse         *argParse,
                                  char             *short_opt,
                                  char             *long_opt,
                                  char             *help,
                                  char             *default_val,
                                  ArgParseCallback  callback,
                                  bool              required,
                                  ArgParseValueType value_type);

/**
 * @brief 禁用自动生成帮助信息
 */
void argParseDisableAutoHelp();

/** End---------------构造API---------------- */

/** Start----------------解析API---------------- */

/**
 * @brief 解析命令行参数
 * @param argParse 解析器指针
 * @param argc 参数个数
 * @param argv 参数列表
 */
void argParseParse(ArgParse *argParse, int argc, char *argv[]);

/**
 * @brief 获取当前解析到的命令名
 * @param argParse 解析器指针
 * @return char* 命令名
 */
char *argParseGetCurCommandName(ArgParse *argParse);

/**
 * @brief 获取当前解析到的命令的值,针对带值参数
 * @param argParse 解析器指针
 * @return char* 命令值
 */
char *argParseGetCurCommandValue(ArgParse *argParse);
/**
 * @brief 获取当前解析到的命令的值列表,针对带值参数
 * @param argParse 解析器指针
 * @param len 值个数buffer
 * @return char** 命令值列表
 */
char **argParseGetCurCommandValues(ArgParse *argParse, int *len);

/**
 * @brief
 * 获取当前解析到的命令的参数,当前命令为解析到的最后一个命令或者命令的子命令
 * @param argParse 解析器指针
 * @param opt 选项名
 * @return char* 选项值
 * @return
 */
char *argParseGetCurArg(ArgParse *argParse, char *opt);

/**
 * @brief 获取当前解析到的命令参数列表,仅适用于多值参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @param len 参数个数buffer
 * @return char** 参数列表
 */
char **argParseGetCurArgList(ArgParse *argParse, char *opt, int *len);

/**
 * @brief 获取全局参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @return char* 选项值
 */
char *argParseGetGlobalArg(ArgParse *argParse, char *opt);

/**
 * @brief 获取全局参数列表,仅适用于多值参数
 * @param argParse 解析器指针
 * @param opt 选项名
 * @param len 参数个数buffer
 * @return char** 参数列表
 */
char **argParseGetGlobalArgList(ArgParse *argParse, char *opt, int *len);

/**
 * @brief 检查当前解析到的命令的某个参数是否被触发
 * @param argParse 解析器指针
 * @param opt 选项名
 * @return bool 是否被触发
 */
bool argParseCheckCurArgTriggered(ArgParse *argParse, char *opt);

/**
 * @brief 检查当前解析到的命令是否被触发
 * @param argParse 解析器指针
 * @param command_name 命令名
 * @return bool 是否被触发
 */
bool argParseCheckCommandTriggered(ArgParse *argParse, char *command_name);

/**
 * @brief 检查全局参数是否被触发
 * @param  argParse 解析器指针
 * @param   opt 选项名
 * @return bool 是否被触发
 */
bool argParseCheckGlobalTriggered(ArgParse *argParse, char *opt);

char  *argParseGetVal(ArgParse *argParse);
char **argParseGetValList(ArgParse *argParse, int *len);

/** End----------------解析API---------------- */

/**
 * @brief 生成命令帮助信息
 * @param   command 命令指针
 * @return  char* 命令帮助信息
 */
char *argParseGenerateHelpForCommand(Command *command);

/**
 * @brief 生成全局参数帮助信息
 * @param argParse 解析器指针
 * @return  char* 全局参数帮助信息
 */
char *argParseGenerateHelp(ArgParse *argParse);

/**
 * @brief 生成选项错误信息
 * @param argParse 解析器指针
 * @param name 选项名
 * @param short_flag 是否为短选项
 * @return char* 选项错误信息
 */
char *
argParseGenerateArgErrorMsg(ArgParse *argParse, char *name, bool short_flag);

NORETURN void argParseError(ArgParse   *argParse,
                             Command    *lastCommand,
                             const char *prefix,
                             const char *suffix);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CARGPARSE_H