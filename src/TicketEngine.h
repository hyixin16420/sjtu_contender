#pragma once
#include <Windows.h>
#include <wrl.h>
#include <WebView2.h>

#include <string>
#include <functional>

class TicketEngine {
public:
	/**
	* 1. 生命周期与窗口绑定
	*/
	// UI 初始化完成后调用，传入用来显示网页的控件句柄
	bool InitializeBrowser(HWND browserContainerHwnd);
	// UI 窗口大小改变时调用，防止网页尺寸错乱
	void OnResize(int width, int height);

	/**
	* 2. UI控制指令
	*/
	// 导航到指定网址
	void NavigateTo(const std::wstring& url);
	// 启动抢票任务 (可传入 UI 界面上填写的参数）
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
	Microsoft::WRL::ComPtr<ICoreWebView2Controller>m_controller;
	Microsoft::WRL::ComPtr<ICoreWebView2>m_webview;
	LogCallback m_logCallback;
};
