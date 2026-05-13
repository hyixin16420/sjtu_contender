// ModernWin32Window.cpp
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

// 程序入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 注册窗口类
    const wchar_t CLASS_NAME[] = L"sjtu_contender";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wc))
        return 0;

    // 创建窗口
    HWND hWnd = CreateWindowExW(
        0,                              // 扩展样式
        CLASS_NAME,                     // 窗口类名
        L"Windows 窗口",           // 窗口标题
        WS_OVERLAPPEDWINDOW,            // 窗口样式（可调整大小、最大化、最小化、关闭）
        CW_USEDEFAULT, CW_USEDEFAULT,   // 位置
        800, 600,                       // 宽度、高度
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 消息循环
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
