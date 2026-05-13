#pragma once
#include <windows.h>
#include <string>
#include <functional>

// 抢票引擎核心类 
class TicketEngine {
public:
	// ==========================================
	// 1. 生命周期与视图绑定
	// ==========================================

	// UI 初始化完成后调用，传入用来显示网页的控件句柄
	bool InitializeBrowser(HWND browserContainerHwnd);

	// UI 窗口大小改变时调用，防止网页尺寸错乱
	void OnResize(int width, int height);

	// ==========================================
	// 2. UI 控制指令 (伙伴在按钮点击事件中调用)
	// ==========================================

	// 导航到指定网址
	void NavigateTo(const std::wstring& url);

	// 启动抢票任务 (可传入 UI 界面上填写的参数)
	void StartSnapping(const std::wstring& targetTime, int retryIntervalMs);

	// 停止抢票
	void StopSnapping();

	// ==========================================
	// 3. 状态反馈回调 (伙伴用来接收你的消息)
	// ==========================================

	// 定义一个回调函数类型
	using LogCallback = std::function<void(const std::wstring& message)>;

	// 伙伴调用此函数，把 UI 的日志打印逻辑交给你
	void SetLogCallback(LogCallback callback);

private: 
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_webview;
	LogCallback m_logCallback;
};
