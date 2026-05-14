#pragma once
#include <Windows.h>
#include <string>

// 用消息弹窗通知错误消息
void ShowFailure(HRESULT hr, const std::wstring& file, int line, const std::wstring& message = TEXT("Something failed!"));

// 如果出错了，就显示错误代码，并立即终止
void CheckFailure(HRESULT hr, const std::wstring& file, int line, const std::wstring& message = TEXT("Something Failed!"));

// 预处理器的奇异操作，因为如果arg本身就是一个宏，那么#将只能取到它的名字而不是值
#define STRINGIFY(arg) #arg

/**
* If we use a function-like macro to wrap a function call, the macro expansion covers the
* entire function call, and if that function call contains a lambda which spans many lines,
* it makes error messages, the __LINE__ macro, and debuggers less accurate.  Instead,
* we make it a term-like macro which generates a partially-applied function.  In effect,
*     CHECK_FAILURE(MultiLineFunctionCall(...));
* becomes
*     ([](HRESULT hr){ CheckFailure(hr, "error message"); })(MultiLineFunctionCall(...));
* so that MultiLineFunctionCall(...) doesn't have to be part of the macro expansion.
*/
#define CHECK_FAILURE_EXT(file, line)\
	([](HRESULT hr, const std::wstring& message) {\
		CheckFailure(hr, TEXT(file), line, message);\
		return;\
	})
#define CHECK_FAILURE_MESSAGE CHECK_FAILURE_EXT(__FILE__, __LINE__)
#define CHECK_FAILURE(hr) CHECK_FAILURE_MESSAGE(hr, TEXT(#hr))
#define CHECK_FAILURE_BOOL(value) CHECK_FAILURE((value) ? S_OK : E_UNEXPECTED)
#define FAIL_WITH_MSG(msg) CHECK_FAILURE(!!msg)
