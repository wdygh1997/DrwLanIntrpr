#include "semantic.h"

HDC hDC;
char SrcFilePath[200];
char Name[] = "Drawing Language Interpreter -by wdy";

// 消息处理
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
        case WM_DESTROY:
            ReleaseDC(hWnd, hDC);
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            PAINTSTRUCT pt;
            BeginPaint(hWnd, &pt);
            Parser(SrcFilePath); // 构造语法树，其中已嵌入循环描点绘图函数
            EndPaint(hWnd, &pt);
        default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
}

// 窗口初始化
bool PrepareWindow(HINSTANCE hInst, int nCmdShow)
{
    WNDCLASS W;
    memset(&W, 0, sizeof(WNDCLASS));
    W.style = CS_HREDRAW | CS_VREDRAW; // 宽/高度更改，重绘窗口
    W.lpfnWndProc = WndProc; // 消息处理
    W.hInstance = hInst; // 句柄
    W.hCursor = LoadCursor(nullptr, IDC_ARROW); // 箭头光标
    W.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // 窗口背景
    W.lpszClassName = Name; // 窗口标题
    RegisterClass(&W);

    HWND hWnd;
    hWnd = CreateWindow(Name, Name, WS_TILEDWINDOW, 0, 0, 1280, 640, nullptr, nullptr, hInst, nullptr);
    if (hWnd == nullptr)
        return false;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetCursor(LoadCursor(hInst, IDC_ARROW));
    hDC = GetDC(hWnd);

    return true;
}

// 检查绘图语言源文件合法性
bool CheckSrcFile(LPSTR lpCmdParam)
{
    FILE *file = nullptr;
    if (strlen(lpCmdParam) == 0)
    {
        MessageBox(nullptr, "Please input drawing language source file!", "Error", MB_OK);
        return false;
    }
    if ((file = fopen(lpCmdParam, "r")) == nullptr)
    {
        MessageBox(nullptr, "Open drawing language source file failed!", "Error", MB_OK);
        MessageBox(nullptr, lpCmdParam, "Filename", MB_OK);
        return false;
    }
    else
        fclose(file);
    return true;
}

// 应用程序主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    strcpy(SrcFilePath, lpCmdLine); // 源文件路径
    if (!PrepareWindow(hInstance, nCmdShow)) // 初始化窗口
    {
        MessageBox(nullptr, "Create window failed!", "Error", MB_OK);
        return 1;
    }
    if (!CheckSrcFile(lpCmdLine)) // 检查绘图语言源文件合法性
        return 1;

    Parser(SrcFilePath); // 构造语法树，其中已嵌入循环描点绘图函数

    MSG Msg;
    while (GetMessage(&Msg, nullptr, 0, 0)) // 消息循环
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
