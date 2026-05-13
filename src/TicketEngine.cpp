#include "TicketEngine.h"

#include <fstream>
#include <sstream>

using namespace Microsoft::WRL;

//创建底层环境
bool TicketEngine::InitializeBrowser(HWND browserContainerHwnd) {
	//把底层引擎跑起来
	CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
		Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[this, browserContainerHwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
				//创建窗口控制器：与UI窗口绑定在一起，生成一个控制器
				env->CreateCoreWebView2Controller(browserContainerHwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[this, browserContainerHwnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {

						if (controller != nullptr) {
							m_controller = controller;//保存控制器遥控
							m_controller->get_CoreWebView2(&m_webview);   //拿到视图遥控器
						}

						// 初始调整大小
						RECT bounds;
						GetClientRect(browserContainerHwnd, &bounds);//测量窗口有多大
						m_controller->put_Bounds(bounds); //让浏览器画面撑满整个窗口

						// 注册导航完成的监听器
						m_webview->add_NavigationCompleted(
							Callback<ICoreWebView2NavigationCompletedEventHandler>(
								[this](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
									BOOL success;
									args->get_IsSuccess(&success);//问系统是否打开网页了？
									if (success && m_logCallback) {
										m_logCallback(TEXT("网页加载完成，准备就绪！"));  //通过对讲机向UI汇报
									}
									return S_OK;
								}).Get(), nullptr);

						return S_OK;
					}).Get());
				return S_OK;
			}).Get());

	return true;
}

//日常维护与控制
//窗口自适应（用户可以随时放大缩小网页大小）
void TicketEngine::OnResize(int width, int height) {
	if (m_controller != nullptr) {
		RECT bounds = { 0, 0, width, height };
		m_controller->put_Bounds(bounds);
	}
}

//导航跳转
void TicketEngine::NavigateTo(const std::wstring& url) {
	if (m_webview != nullptr) {
		m_webview->Navigate(url.c_str());
		if (m_logCallback) m_logCallback(TEXT("正在跳转至: ") + url);
	}
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
}

//与UI连接
void TicketEngine::SetLogCallback(LogCallback callback) {
	m_logCallback = callback;
}

