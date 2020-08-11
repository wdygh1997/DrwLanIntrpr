#ifndef DRWLANINTRPR_SEMANTIC_H
#define DRWLANINTRPR_SEMANTIC_H

#include <windows.h>
#include <wingdi.h>
#include "syntactic.h"

extern HDC hDC; // 句柄

extern double GetExprValue(ExprNode *root); // 获得表达式值
extern void DrawLoop(double Start, double End, double Step, ExprNode *HorPtr, ExprNode *VerPtr); // 循环绘制点
extern void DelExprTree(ExprNode *root); // 删除语法树

#endif
