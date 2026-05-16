#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <WebView2.h>

#include <string>
#include <functional>

class TicketEngine {
public:
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
	// 启动抢票任务 (可传入UI界面上填写的参数）
	void StartSnapping(const std::wstring& targetTime, int retryIntervalMs);

	void StartSnappingFromFile(const std::wstring& filePath);
	// 停止抢票
	void StopSnapping();

	/**
	* 3. 状态反馈回调
	*/
	// 定义回调函数类型
	using LogCallback = std::function<void(const std::wstring& message)>;
	// 指派回调函数
	void SetLogCallback(LogCallback callback);

private:
	TicketEngine() = default;
	TicketEngine(const TicketEngine&) = delete;
	TicketEngine& operator=(const TicketEngine&) = delete;
	TicketEngine(TicketEngine&&) = delete;
	TicketEngine& operator=(TicketEngine&&) = delete;

	bool m_initialized = false;

	wil::com_ptr<ICoreWebView2Environment>m_environment; // 环境
	wil::com_ptr<ICoreWebView2Controller>m_controller; // 控制器
	wil::com_ptr<ICoreWebView2>m_webview; // 核心视图
	LogCallback m_logCallback;
};
