#include "Basic/pch.h"
#include "Basic/CheckFailure.h"

#include "TicketEngine.h"
#include "Window/window_messages.h"
#include <WebView2EnvironmentOptions.h>
#include <wil/com.h>

#include <fstream>
#include <sstream>

using namespace Microsoft::WRL;

TicketEngine& TicketEngine::instance() {
	static TicketEngine ins;
	return ins;
}

//创建底层环境
void TicketEngine::InitializeBrowser(HWND browserContainerHwnd) {
	if (m_initialized) {
		return;
	}
	m_initialized = true;

	auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
	//把底层引擎跑起来
	CHECK_FAILURE(CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, options.Get(),
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this, browserContainerHwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
				m_environment = env;

				//创建窗口控制器：与UI窗口绑定在一起，生成一个控制器
				env->CreateCoreWebView2Controller(
					browserContainerHwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[this, browserContainerHwnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
						if (m_webview) {
								return S_OK; // 已经初始化过了，跳过
						}

						CHECK_FAILURE_BOOL(controller);
						if (controller) {
							(m_controller = controller)->get_CoreWebView2(&m_webview);
						}

						// 对webview进行一些设置
						wil::com_ptr<ICoreWebView2Settings>settings;
						m_webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);

						// 初始调整大小
						RECT bounds = {};
						GetClientRect(browserContainerHwnd, &bounds);
						m_controller->put_Bounds(bounds); //让浏览器画面撑满整个窗口

						// 注册导航完成的监听器
						m_webview->add_NavigationCompleted(
							Callback<ICoreWebView2NavigationCompletedEventHandler>(
								[this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
									BOOL success;
									args->get_IsSuccess(&success);
									if (success && m_logCallback) {
										m_logCallback(TEXT("网页加载完成，准备就绪！"));  //通过对讲机向UI汇报
									}
									return S_OK;
								}).Get(), &m_TokenNavigationCompleted);

						m_webview->add_WebMessageReceived(
							Callback<ICoreWebView2WebMessageReceivedEventHandler>(
								[this](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
									wil::unique_cotaskmem_string message;
									args->TryGetWebMessageAsString(&message);
									// processMessage(&message);
									webview->PostWebMessageAsString(message.get());
									return S_OK;
								}).Get(), &m_TokenWebMessageReceived);

						PostMessage(browserContainerHwnd, WM_WEBVIEWINITIALIZED, 0, 0);
						return S_OK;
					}).Get());
				return S_OK;
			}).Get()));
	return;
}

//日常维护与控制
//窗口自适应（用户可以随时放大缩小网页大小）
void TicketEngine::OnResize(RECT bounds) {
	if (m_controller != nullptr) {
		m_controller->put_Bounds(bounds);
	}
	return;
}

// 窗口关闭时的资源清理
void TicketEngine::OnClose() {
	if (m_webview) {
		m_webview->remove_NavigationCompleted(m_TokenNavigationCompleted);
		m_webview->remove_WebMessageReceived(m_TokenWebMessageReceived);
	}
	m_webview.reset();
	m_controller.reset();
	m_environment.reset();
	return;
}

//导航跳转
void TicketEngine::NavigateTo(const std::wstring& url) {
	if (m_webview != nullptr) {
		m_webview->Navigate(url.c_str());
		if (m_logCallback) m_logCallback(TEXT("正在跳转至: ") + url);
	}
	return;
}


void TicketEngine::StartSnapping(const std::wstring& targetTime, int retryIntervalMs) {
	if (m_webview != nullptr) {
		if (m_logCallback) m_logCallback(TEXT("开始执行抢票脚本..."));

		// 这里注入真正的抢票 JS
		LPCWSTR jsScript = TEXT("document.querySelector('.buy-btn').click();");
		//把代码注入当前网页的V8引擎中执行
		m_webview->ExecuteScript(jsScript, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
			[this](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
				if (m_logCallback) m_logCallback(TEXT("点击指令已发送！"));
				return S_OK;
			}).Get());
	}
	return;
}

// 读取外部 JS 文件并注入
void TicketEngine::StartSnappingFromFile(const std::wstring& filePath) {
	std::wifstream file(filePath);
	if (!file.is_open()) {
		if (m_logCallback) m_logCallback(TEXT("未找到JS文件！"));
		return;
	}

	std::wstringstream buffer;
	buffer << file.rdbuf();
	std::wstring jsCode = buffer.str();

	// 将读取到的纯文本代码注入浏览器
	// m_webview->ExecuteScript(jsCode.c_str(), Callback<...>(...).Get());
	return;
}

void TicketEngine::StopSnapping() {
	// 1. 安全检查：确保浏览器已经初始化
	if (m_webview == nullptr) {
		if (m_logCallback) m_logCallback(TEXT("浏览器初始化失败！"));
		return;
	}

	// 2. 编写刹车指令：把 JS 里的开关关掉，并清理定时器
	LPCWSTR stopScript = TEXT("window.isSnapping = false; if(window.snappingTimer) clearInterval(window.snappingTimer);");

	// 3. 发射刹车指令
	m_webview->ExecuteScript(stopScript, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
		[this](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
			if (m_logCallback) m_logCallback(TEXT("抢票脚本已成功停止！"));
			return S_OK;
		}
	).Get());
	return;
}

void TicketEngine::SetLogCallback(LogCallback callback) {
	m_logCallback = callback;
	return;
}
void TicketEngine::LogOut(const std::wstring& message) {
	if (m_logCallback) {
		m_logCallback(message);
	}
	return;
}
