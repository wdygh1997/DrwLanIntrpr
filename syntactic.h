#ifndef DRWLANINTRPR_SYNTACTIC_H
#define DRWLANINTRPR_SYNTACTIC_H

#include "lexical.h"

typedef double (*FuncPtr)(double); // 函数指针，参数double类型，返回double类型

// 语法树节点定义
struct ExprNode
{
    Token_Type OpCode; // 记号类别
    union
    {
        struct{
            ExprNode *Left, *Right;
        }CaseOpera; // 如果是二元运算，两个子节点
        struct{
            ExprNode *Child;
            FuncPtr MathFuncPtr; // 函数指针
        }CaseFunc; // 如果是函数，一个子节点
        double CaseConst; // 如果是常数，double值
        double *CaseParmPtr; // 如果是参数T，double指针
    }Content; // 记号内容
};

extern void Parser(char *SrcFilePtr); // 构造语法树

#endif
