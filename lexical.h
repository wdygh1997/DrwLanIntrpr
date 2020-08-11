#ifndef DRWLANINTRPR_LEXICAL_H
#define DRWLANINTRPR_LEXICAL_H

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdarg>
#include <cstdlib>

// 记号类别，为一个枚举
enum Token_Type
{
    // 保留字
    ORIGIN/* 0 */, SCALE/* 1 */, ROT/* 2 */, IS/* 3 */,
    FOR/* 4 */, FROM/* 5 */, TO/* 6 */, STEP/* 7 */, DRAW/* 8 */,

    // 参数T
    T/* 9 */,

    // 分隔符
    SEMI/* 10 */, // 分号
    L_BRACKET/* 11 */, // 左括号
    R_BRACKET/* 12 */, // 右括号
    COMMA/* 13 */, // 逗号

    // 运算符
    PLUS/* 14 */, // 加运算符
    MINUS/* 15 */, // 减运算符
    MUL/* 16 */, // 乘运算符
    DIV/* 17 */, // 除运算符
    POWER/* 18 */, // 幂运算符

    // 函数
    FUNC/* 19 */,

    // 常数
    CONSTANT/* 20 */,

    // 空记号
    NON_TOKEN/* 21 */, // 标识文件末尾
    // 错记号
    ERR_TOKEN/* 22 */ // 标识非法记号
};

// 记号定义，为一个结构体
struct Token
{
    Token_Type type; // 类别
    const char *lexeme; // 属性，记号字面字符串
    double value; // 属性，若记号为常数，记录其值
    double (*FuncPtr)(double); // 属性，若记号为函数，记录指针
};

// 符号表，为一个Token数组，用以匹配字面量第一个字符为字母时的记号
static Token TokenTab[] =
        {
                // 常用常数
                { CONSTANT, "PI",       3.1415926,  nullptr },
                { CONSTANT, "E",        2.71828,    nullptr },
                // 参数T
                { T,        "T",        0.0,        nullptr },
                // 常用函数
                { FUNC,     "SIN",      0.0,                sin },
                { FUNC,     "COS",      0.0,                cos },
                { FUNC,     "TAN",      0.0,                tan },
                { FUNC,     "LN",       0.0,                log },
                { FUNC,     "EXP",      0.0,                exp },
                { FUNC,     "SQRT",     0.0,                sqrt },
                // 保留字
                { ORIGIN,   "ORIGIN",   0.0,        nullptr },
                { SCALE,    "SCALE",    0.0,        nullptr },
                { ROT,      "ROT",      0.0,        nullptr },
                { IS,       "IS",       0.0,        nullptr },
                { FOR,      "FOR",      0.0,        nullptr },
                { FROM,     "FROM",     0.0,        nullptr },
                { TO,       "TO",       0.0,        nullptr },
                { STEP,     "STEP",     0.0,        nullptr },
                { DRAW,     "DRAW",     0.0,        nullptr },
        };

extern unsigned int LineNo; // 绘图语言源文件的行号
extern int InitScanner(const char *); // 打开词法分析器
extern Token GetToken(); // 获得一个记号
extern void CloseScanner(); // 关闭词法分析器

#endif
