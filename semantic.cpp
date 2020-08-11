#include "semantic.h"

extern double Parameter, Origin_x, Origin_y, Scale_x, Scale_y, Rot_angle;

// 函数，用以计算表达式值
double GetExprValue(ExprNode *root)
{
    if (root == nullptr)
        return 0.0;
    switch (root->OpCode)
    {
        case PLUS:
            return GetExprValue(root->Content.CaseOpera.Left) + GetExprValue(root->Content.CaseOpera.Right);
        case MINUS:
            return GetExprValue(root->Content.CaseOpera.Left) - GetExprValue(root->Content.CaseOpera.Right);
        case MUL:
            return GetExprValue(root->Content.CaseOpera.Left) * GetExprValue(root->Content.CaseOpera.Right);
        case DIV:
            return GetExprValue(root->Content.CaseOpera.Left) / GetExprValue(root->Content.CaseOpera.Right);
        case POWER:
            return pow(GetExprValue(root->Content.CaseOpera.Left), GetExprValue(root->Content.CaseOpera.Right));
        case FUNC:
            return (*root->Content.CaseFunc.MathFuncPtr) // 得到函数名
                    (GetExprValue(root->Content.CaseFunc.Child)); // 得到参数T值
        case CONSTANT:
            return root->Content.CaseConst;
        case T:
            return *(root->Content.CaseParmPtr);
        default:
            return 0.0;
    }
}

// 函数，用以计算点坐标
void CalcCoord(ExprNode *Hor_Exp, ExprNode *Ver_Exp, double &Hor_x, double &Ver_y)
{
    double HorCord, VerCord, Hor_tmp;

    HorCord = GetExprValue(Hor_Exp); // 计算表达式值得到横坐标
    VerCord = GetExprValue(Ver_Exp); // 计算表达式值得到纵坐标

    // 进行比例变换
    HorCord *= Scale_x;
    VerCord *= Scale_y;

    // 进行旋转变换
    Hor_tmp = HorCord * cos(Rot_angle) + VerCord *sin(Rot_angle);
    VerCord = VerCord * cos(Rot_angle) - HorCord *sin(Rot_angle);
    HorCord = Hor_tmp;

    // 进行平移变换
    HorCord += Origin_x;
    VerCord += Origin_y;

    // 返回变换后的坐标
    Hor_x = HorCord;
    Ver_y = VerCord;
}

// 函数，用以绘制一个点
void DrawPixel(unsigned long x, unsigned long y)
{
    SetPixel(hDC, x, y, RGB(69, 139, 116));
}

// 函数，用以循环绘制点
void DrawLoop(double Start, double End, double Step, ExprNode *HorPtr, ExprNode *VerPtr)
{
    double x, y;
    for (Parameter = Start; Parameter <= End; Parameter += Step)
    {
        CalcCoord(HorPtr, VerPtr, x, y);
        DrawPixel((unsigned long)x, (unsigned long)y);
    }
}

// 函数，用以删除语法树
void DelExprTree(ExprNode *root)
{
    if (root == nullptr)
        return;
    switch (root->OpCode)
    {
        case PLUS:
        case MINUS:
        case MUL:
        case DIV:
        case POWER:
            DelExprTree(root->Content.CaseOpera.Left);
            DelExprTree(root->Content.CaseOpera.Right);
            break;
        case FUNC:
            DelExprTree(root->Content.CaseFunc.Child);
            break;
        default:
            break;
    }
    delete(root);
}
