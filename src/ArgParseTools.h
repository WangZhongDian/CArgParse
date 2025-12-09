#ifndef ARGPARSETOOLS_H
#define ARGPARSETOOLS_H

#include "ArgParse.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LONG_ARG,  // 长选项
    SHORT_ARG, // 短选项
    COMMAND,   // 命令
    BAD,       // 错误
} ArgType;

Command *createCommand(const char       *name,
                       const char       *help,
                       const char       *default_val,
                       ArgParseCallback  callback,
                       CommandGroup     *group,
                       ArgParseValueType value_type); // 创建命令

CommandGroup *createCommandGroup(const char *name,
                                 const char *help); // 创建命令组

CommandGroup *addCommandToGroup(CommandGroup *group,
                                Command      *command); // 添加命令到命令组

CommandArgs *createCommandArgs(const char       *short_opt,
                               const char       *long_opt,
                               const char       *default_val,
                               const char       *help,
                               ArgParseCallback  callback,
                               bool              required,
                               ArgParseValueType value_type); // 创建命令参数

/**
 * @brief 设置命令参数值
 * @param args 参数
 * @param val 值
 * @return 成功返回true，失败返回false
 */
bool argParseSetArgVal(CommandArgs *args, const char *val); // 设置命令参数值

bool argParseSetCommandVal(Command *command, const char *val); // 设置命令值

bool argParseSetVal(ArgParse *argParse, const char *val); // 设置值

ArgType checkArgType(char *arg); // 检查参数类型

Command *argParseFindCommand(ArgParse *argParse, const char *name); // 查找命令

Command *argParseFindSubCommand(Command    *command,
                                const char *name); // 查找子命令

CommandArgs *argParseFindCommandArgs(Command    *command,
                                     const char *name,
                                     bool        short_flag); // 查找命令参数

CommandArgs *argParseFindGlobalArgs(ArgParse   *argParse,
                                    const char *name,
                                    bool        short_flag); // 查找全局参数

char  *stringNewCopy(const char *str);       // 创建字符串副本
void   __catStr(char **dst, int count, ...); // 字符串拼接
size_t __getStrlen(const char *str);         // 获取字符串长度

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // ARGPARSETOOLS_H