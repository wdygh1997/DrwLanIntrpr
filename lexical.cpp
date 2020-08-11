#include "lexical.h"

unsigned int LineNo; // 绘图语言源文件的行号
FILE *InFile; // 输入文件
char TokenBuffer[100]; // 记号缓冲区

// 函数，用以打开绘图语言源程序文件
int InitScanner(const char *FileName)
{
    LineNo = 1; // 行号初始化为1
    InFile = fopen(FileName, "r"); // 打开绘图语言源程序文件
    if (InFile != nullptr)
        return 1; // 打开成功
    else
        return 0; // 打开失败
}

// 函数，用以关闭绘图语言源程序文件
void CloseScanner()
{
    if (InFile != nullptr)
        fclose(InFile); // 关闭绘图语言源程序文件
}

// 从输入文件读出字符
static char GetChar()
{
    char ch = getc(InFile);
    return toupper(ch); // 绘图语言不区分大小写，所以需要全部转换为大写
}

// 将字符退回输入文件
static void BackChar(char ch)
{
    if (ch != EOF)
        ungetc(ch, InFile);
}

// 字符加入记号缓冲区
static void AddCharTokenString(char ch)
{
    int TokenLength = strlen(TokenBuffer);
    if (TokenLength + 1 >= sizeof(TokenBuffer))
        return;
    TokenBuffer[TokenLength] = ch;
    TokenBuffer[TokenLength + 1] = '\0';
}

// 清空记号缓冲区内容
static void EmptyTokenString()
{
    memset(TokenBuffer, 0, 100);
}

// 判断记号是否在记号表中
static Token JudgeKeyToken(const char *IDString)
{
    for (int loop = 0; loop < sizeof(TokenTab)/sizeof(TokenTab[0]); loop++)
    {
        if (strcmp(TokenTab[loop].lexeme, IDString) == 0)
            return TokenTab[loop]; // 判断在字符表中就返回该记号
    }
    // 循环执行完还没返回说明不在记号表中
    Token errortoken;
    memset(&errortoken, 0, sizeof(Token)); // 初始化
    errortoken.type = ERR_TOKEN; // 填入出错记号
    return errortoken; // 返回出错记号
}

// 函数，用以获得一个记号
Token GetToken()
{
    Token token;
    memset(&token, 0, sizeof(Token)); // 初始化
    EmptyTokenString(); // 清空记号缓冲区
    token.lexeme = TokenBuffer; // 记号字面字符串指针指向记号缓冲区

    char ch;
    while (true) // 过滤空白符，直到非空白符，文件结束则直接返回NON_TOKEN记号
    {
        ch = GetChar();
        if (ch == EOF) // 文件结束
        {
            token.type = NON_TOKEN;
            return token;
        }
        if (ch == '\n')
            LineNo++;
        if (!isspace(ch)) // 非空白符
            break;
    }

    AddCharTokenString(ch); // 如果是非空白符，加入记号缓冲区

    if (isalpha(ch)) // 如果是字母，则是符号表中定义的记号中的一种
    {
        while (true) // 继续读后面的字母或数字并加入记号缓冲区
        {
            ch = GetChar();
            if (isalnum(ch))
                AddCharTokenString(ch);
            else
                break;
        }
        BackChar(ch); // 将多读的一个字符退回输入文件
        token = JudgeKeyToken(TokenBuffer); // 判断是否在记号表中
        token.lexeme = TokenBuffer; // 记号字面字符串
        return token;
    }
    else if (isdigit(ch)) // 如果是数字，一定是常量
    {
        while (true) // 继续读后面的数字并加入记号缓冲区
        {
            ch = GetChar();
            if (isdigit(ch))
                AddCharTokenString(ch);
            else
                break;
        }
        if (ch == '.') // 遇到小数点
        {
            AddCharTokenString(ch); // 加入缓冲区
            while (true) // 继续读小数点后数字并加入记号缓冲区
            {
                ch = GetChar();
                if (isdigit(ch))
                    AddCharTokenString(ch);
                else
                    break;
            }
        }
        BackChar(ch); // 将多读的一个字符退回输入文件
        token.type = CONSTANT; // 记号类型是常数
        token.value = atof(TokenBuffer); // 常量记号的数值
        return token;
    }
    else // 如果是其他字符
    {
        switch (ch)
        {
            case ';':
                token.type = SEMI;
                break;
            case '(':
                token.type = L_BRACKET;
                break;
            case ')':
                token.type = R_BRACKET;
                break;
            case ',':
                token.type = COMMA;
                break;
            case '+':
                token.type = PLUS;
                break;
            case '-':
                ch = GetChar(); // 再读一个字符
                if (ch == '-') // 如果还是减号，表明是注释
                {
                    while (ch != '\n' && ch != EOF) // 忽略后续内容直到换行或者文件末尾
                        ch = GetChar();
                    BackChar(ch);
                    return GetToken();
                }
                else // 如果不是减号
                {
                    BackChar(ch); // 将多读的一个字符退回输入文件
                    token.type = MINUS; // 记号类型为减运算符
                    break;
                }
            case '/':
                ch = GetChar(); // 再读一个字符
                if (ch == '/') // 如果还是除号，表明是注释
                {
                    while (ch != '\n' && ch != EOF) // 忽略后续内容直到换行或者文件末尾
                        ch = GetChar();
                    BackChar(ch);
                    return GetToken();
                }
                else // 如果不是除号
                {
                    BackChar(ch); // 将多读的一个字符退回输入文件
                    token.type = DIV; // 记号类型为除运算符
                    break;
                }
            case '*':
                ch = GetChar();
                if (ch == '*') // 两个乘号表明是幂运算符
                {
                    token.type = POWER;
                    break;
                }
                else // 一个乘号才是乘运算符
                {
                    BackChar(ch);
                    token.type = MUL;
                    break;
                }
            default:
                token.type = ERR_TOKEN; // 非法记号
                break;
        }
    }
    return token;
}