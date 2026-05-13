#include "TicketEngine.h"
#include <wrl.h>
#include <wil/com.h>
#include "WebView2.h"

using namespace Microsoft::WRL;

// 类的成员变量（代替之前的全局变量）
ComPtr<ICoreWebView2Controller> m_controller;
ComPtr<ICoreWebView2> m_webview;
TicketEngine::LogCallback m_logCallback;

bool TicketEngine::InitializeBrowser(HWND browserContainerHwnd) {
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this, browserContainerHwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				env->CreateCoreWebView2Controller(browserContainerHwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[this, browserContainerHwnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {

						if (controller != nullptr) {
							m_controller = controller;
							m_controller->get_CoreWebView2(&m_webview);
						}

						// 初始调整大小
						RECT bounds;
						GetClientRect(browserContainerHwnd, &bounds);
						m_controller->put_Bounds(bounds);

						// 注册导航完成的监听器
						m_webview->add_NavigationCompleted(
							Callback<ICoreWebView2NavigationCompletedEventHandler>(
								[this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
									BOOL success;
									args->get_IsSuccess(&success);
									if (success && m_logCallback) {
										m_logCallback(L"网页加载完成，准备就绪！");
									}
									return S_OK;
								}).Get(), nullptr);

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());

	return true;
}


void TicketEngine::OnResize(int width, int height) {
	if (m_controller != nullptr) {
		RECT bounds = { 0, 0, width, height };
		m_controller->put_Bounds(bounds);
	}
}


void TicketEngine::NavigateTo(const std::wstring& url) {
	if (m_webview != nullptr) {
		m_webview->Navigate(url.c_str());
		if (m_logCallback) m_logCallback(L"正在跳转至: " + url);
	}
}

void TicketEngine::StartSnapping(const std::wstring& targetTime, int retryIntervalMs) {
	if (m_webview != nullptr) {
		if (m_logCallback) m_logCallback(L"开始执行抢票脚本...");

		// 这里注入真正的抢票 JS
		LPCWSTR jsScript = L"document.querySelector('.buy-btn').click();";
		m_webview->ExecuteScript(jsScript, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
			[this](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
				if (m_logCallback) m_logCallback(L"点击指令已发送！");
				return S_OK;
			}).Get());
	}
}

void TicketEngine::SetLogCallback(LogCallback callback) {
	m_logCallback = callback;
}
