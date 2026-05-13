#include <wrl.h>
#include <wil/com.h>
#include "WebView2.h"

using namespace Microsoft::WRL;

// 全局指针，用于控制 WebView2
static ComPtr<ICoreWebView2Controller> webviewController;   //网页外观
static ComPtr<ICoreWebView2> webview;                       //网页内核，控制网页跳转等


// 1. 创建 WebView2 环境
CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
	Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
		[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

			// 2. 环境创建成功后，创建 Controller (绑定到我们的窗口 hWnd)
			env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
				[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {

					if (controller != nullptr) {
						webviewController = controller;
						webviewController->get_CoreWebView2(&webview);
					}

					// 3. 调整网页大小以填满窗口
					RECT bounds;
					GetClientRect(hWnd, &bounds);
					webviewController->put_Bounds(bounds);

					// 4. 导航到目标购票网站！
					webview->Navigate(L"https://www.example-ticket-website.com");

					return S_OK;
				}).Get());
			return S_OK;
		}).Get());


// 监听导航完成事件
webview->add_NavigationCompleted(
	Callback<ICoreWebView2NavigationCompletedEventHandler>(
		[](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
			BOOL success;
			args->get_IsSuccess(&success);

			if (success) {
				// 网页加载成功！注入我们的 JavaScript 自动化脚本
				// 这里的 JS 逻辑：找到 class 为 'buy-btn' 的按钮并点击
				LPCWSTR jsScript = L"document.querySelector('.buy-btn').click();";

				sender->ExecuteScript(jsScript, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
					[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
						// 脚本执行完毕后的回调
						return S_OK;
					}).Get());
			}
			return S_OK;
		}).Get(), nullptr);
