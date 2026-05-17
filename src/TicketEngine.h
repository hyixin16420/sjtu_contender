#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <WebView2.h>
#include <opencv2/core/mat.hpp>

#include <string>
#include <functional>

class TicketEngine {
public:
	static void create();
	static void destroy();
	static TicketEngine& instance();
	/**
	* 1. 生命周期与窗口绑定
	*/
	// UI初始化完成后调用，传入用来显示网页的控件句柄
	void InitializeBrowser(HWND browserContainerHwnd);
	// UI窗口大小改变时调用，防止网页尺寸错乱
	void OnResize(RECT bounds);
	// UI窗口关闭
	void OnClose();
	/**
	* 2. UI控制指令
	*/
	// 导航到指定网址
	void NavigateTo(const std::wstring& url);
	// 截图
	void Capture(cv::Mat& mat);
	// 临时用于JS指令测试
	void ExecuteScript(const std::wstring& script);
	// 启动抢票任务 (可传入UI界面上填写的参数）
	void StartSnapping(const std::wstring& targetTime, int retryIntervalMs);

	void StartSnappingFromFile(const std::wstring& filePath);
	// 停止抢票
	void StopSnapping();

	/**
	* 3. 日志反馈回调
	*/
	// 输出日志
	void LogOut(const std::wstring& message, const wchar_t* endc = L"\n");

private:
	TicketEngine();
	~TicketEngine();
	TicketEngine(const TicketEngine&) = delete;
	TicketEngine& operator=(const TicketEngine&) = delete;
	TicketEngine(TicketEngine&&) = delete;
	TicketEngine& operator=(TicketEngine&&) = delete;

	bool m_initialized;
	HWND m_hContainerWnd;

	wil::com_ptr<ICoreWebView2Environment15>m_environment; // 环境
	wil::com_ptr<ICoreWebView2Controller4>m_controller; // 控制器
	wil::com_ptr<ICoreWebView2>m_webview; // 核心视图

	EventRegistrationToken m_tokenNavigationCompleted;
	EventRegistrationToken m_tokenWebMessageReceived;
	EventRegistrationToken m_tokenBrowserProcessExited;
};
