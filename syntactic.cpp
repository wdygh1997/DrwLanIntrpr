#include "syntactic.h"
#include "semantic.h"

double Origin_x = 0, Origin_y = 0, Scale_x = 1, Scale_y = 1, Rot_angle = 0;
Token token;

// 打印错误信息
void ErrMsg(unsigned int line, const char *desc, const char *str)
{
    char msg[256];
    memset(msg, 0, 256);
    sprintf(msg, "[Line %04d] %s: %s!", line, desc, str);

    MessageBox(nullptr, msg, "Error", MB_OK); // 错误弹窗，有错误消息和确认按钮

    CloseScanner();
    exit(1);
}

// 分发语法错误
void SyntaxError(int case_of)
{
    if (case_of == 1)
        ErrMsg(LineNo, "wrong token", token.lexeme);
    else if (case_of == 2)
        ErrMsg(LineNo, "unexpected token", token.lexeme);
}

// 获取一个记号，并判断是否是非法记号
void FetchToken()
{
    token = GetToken();
    if (token.type == ERR_TOKEN)
        SyntaxError(1);
}

// 匹配记号类型，若成功再获取一个记号
void MatchToken(Token_Type The_Token)
{
    if (token.type != The_Token)
        SyntaxError(2);
    FetchToken();
}

double Parameter = 0; // 参数T

// 生成语法树节点
ExprNode *MakeExprNode(Token_Type opcode, ...) // 可变参数
{
    auto ExprPtr = new(ExprNode);
    ExprPtr->OpCode = opcode;

    va_list ArgPtr;
    va_start(ArgPtr, opcode);
    switch (opcode)
    {
        case CONSTANT: // 常数
            ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
            break;
        case T: // 参数T
            ExprPtr->Content.CaseParmPtr = &Parameter;
            break;
        case FUNC: // 函数运算式
            ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
            ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
            break;
        default: // 二元运算式（加减、乘除）
            ExprPtr->Content.CaseOpera.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
            ExprPtr->Content.CaseOpera.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
            break;
    }
    va_end(ArgPtr);

    return ExprPtr;
}

// 对各级递归子程序的集中声明
void Program(); // 程序
void Statement(); // 语句
void OriginStatement(); // Origin语句
void RotStatement(); // Rot语句
void ScaleStatement(); // Scale语句
void ForStatement(); // For语句
ExprNode *Expression(); // 表达式、二元加减运算表达式
ExprNode *Term(); // 乘除运算表达式
ExprNode *Factor(); // 一元加减运算表达式
ExprNode *Component(); // 幂运算表达式
ExprNode *Atom(); // 原子表达式

// 函数，用以构造语法树
void Parser(char *SrcFilePtr)
{
    if (!InitScanner(SrcFilePtr))
    {
        printf("打开绘图语言源文件失败!\n");
        return;
    }
    FetchToken(); // 获取一个记号
    Program(); // 进入Program程序
    CloseScanner();
}

// Program程序，下一级为Statement程序
void Program()
{
    while (token.type != NON_TOKEN) // 循环获取语句，直到文件结束
    {
        Statement(); // 进入Statement程序
        MatchToken(SEMI); // 语句结尾必须是分号，其他记号是非法的
    }
}

// Statement程序，用以分发不同类型语句
void Statement()
{
    switch (token.type)
    {
        case ORIGIN:
            OriginStatement();
            break;
        case SCALE:
            ScaleStatement();
            break;
        case ROT:
            RotStatement();
            break;
        case FOR:
            ForStatement();
            break;
        default:
            SyntaxError(2); // 记号不是四种语句起始记号中的一种，记号非法
    }
}

// OriginStatement程序
void OriginStatement()
{
    ExprNode *tmp;
    MatchToken(ORIGIN); // 匹配ORIGIN，否则记号非法
    MatchToken(IS); // 匹配IS，否则记号非法
    MatchToken(L_BRACKET); // 匹配左括号，否则记号非法

    tmp = Expression(); // 创建表达式子树
    Origin_x = GetExprValue(tmp); // 计算表达式的值，为绘图原点x坐标
    DelExprTree(tmp); // 删除表达式子树

    MatchToken(COMMA); // 匹配逗号，否则记号非法

    tmp = Expression();
    Origin_y = GetExprValue(tmp); // 计算表达式的值，为绘图原点y坐标
    DelExprTree(tmp);

    MatchToken(R_BRACKET); // 匹配右括号，否则记号非法
}

// ScaleStatement程序
void ScaleStatement()
{
    ExprNode *tmp;
    MatchToken(SCALE);
    MatchToken(IS);
    MatchToken(L_BRACKET);

    tmp = Expression();
    Scale_x = GetExprValue(tmp); // 计算表达式的值，为绘图比例的x值
    DelExprTree(tmp);

    MatchToken(COMMA);

    tmp = Expression();
    Scale_y = GetExprValue(tmp); // 计算表达式的值，为绘图比例的y值
    DelExprTree(tmp);

    MatchToken(R_BRACKET);
}

// RotStatement程序
void RotStatement()
{
    ExprNode *tmp;
    MatchToken(ROT);
    MatchToken(IS);

    tmp = Expression();
    Rot_angle = GetExprValue(tmp); // 计算表达式的值，为绘图旋转角度的值
    DelExprTree(tmp);
}

// ForStatement程序
void ForStatement()
{
    ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;
    double Start, End, Step;

    MatchToken(FOR);
    MatchToken(T);
    MatchToken(FROM);

    start_ptr = Expression();
    Start = GetExprValue(start_ptr); // 计算表达式的值，为绘图T参数的起点值
    DelExprTree(start_ptr);

    MatchToken(TO);

    end_ptr = Expression();
    End = GetExprValue(end_ptr); // 计算表达式的值，为绘图T参数的终点值
    DelExprTree(end_ptr);

    MatchToken(STEP);

    step_ptr = Expression();
    Step = GetExprValue(step_ptr); // 计算表达式的值，为绘图T参数的步长值
    DelExprTree(step_ptr);

    MatchToken(DRAW);
    MatchToken(L_BRACKET);

    x_ptr = Expression(); // 计算表达式的值，为绘图点横坐标的值

    MatchToken(COMMA);

    y_ptr = Expression(); // 计算表达式的值，为绘图点纵坐标的值

    MatchToken(R_BRACKET);

    DrawLoop(Start, End, Step, x_ptr, y_ptr); // 循环描点绘图

    DelExprTree(x_ptr);
    DelExprTree(y_ptr);
}

// Expression程序，一个加减运算式
ExprNode *Expression()
{
    ExprNode *left, *right; // 左、右子语法树
    Token_Type token_tmp; // 当前节点类型
    left = Term();
    while (token.type == PLUS || token.type == MINUS)
    {
        token_tmp = token.type;
        MatchToken(token_tmp);
        right = Term();
        left = MakeExprNode(token_tmp, left, right);
    }
    return left;
}

// Term程序，一个乘除运算式
ExprNode *Term()
{
    ExprNode *left, *right; // 左、右子语法树
    Token_Type token_tmp; // 当前节点类型
    left = Factor();
    while (token.type == MUL || token.type == DIV)
    {
        token_tmp = token.type;
        MatchToken(token_tmp);
        right = Factor();
        left = MakeExprNode(token_tmp, left, right);
    }
    return left;
}

// Factor程序，一个正负运算式
ExprNode *Factor()
{
    ExprNode *left, *right; // 左、右子语法树
    if (token.type == PLUS)
    {
        MatchToken(PLUS);
        right = Factor();
    }
    else if (token.type == MINUS)
    {
        MatchToken(MINUS);
        right = Factor();
        left = new ExprNode;
        left->OpCode = CONSTANT;
        left->Content.CaseConst = 0.0;
        right = MakeExprNode(MINUS, left, right);
    }
    else
        right = Component();
    return right;
}

// Component程序，一个幂运算式
ExprNode *Component()
{
    ExprNode *left, *right; // 左、右子语法树
    left = Atom();
    if (token.type == POWER)
    {
        MatchToken(POWER);
        right = Component();
        left = MakeExprNode(POWER, left, right);
    }
    return left;
}

// Atom程序，一个常数、参数T、函数运算式、括号运算式
ExprNode *Atom()
{
    Token t = token;
    ExprNode *address, *tmp; // address指向语法树叶节点
    switch (token.type)
    {
        case CONSTANT:
            MatchToken(CONSTANT);
            address = MakeExprNode(CONSTANT, t.value);
            break;
        case T:
            MatchToken(T);
            address = MakeExprNode(T);
            break;
        case FUNC:
            MatchToken(FUNC);
            MatchToken(L_BRACKET);
            tmp = Expression();
            address = MakeExprNode(FUNC, t.FuncPtr, tmp);
            MatchToken(R_BRACKET);
            break;
        case L_BRACKET:
            MatchToken(L_BRACKET);
            address = Expression();
            MatchToken(R_BRACKET);
            break;
        default:
            SyntaxError(2);
    }
    return address;
}