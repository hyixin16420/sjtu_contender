#include "Basic/pch.h"
#include "Basic/CheckFailure.h"

#include "resource.h"
#include "window_messages.h"
#include "TicketEngine.h"

TicketEngine* gTicketEngine = nullptr;

void LogCallback(const std::wstring& message) {
	OutputDebugString(message.c_str());
	OutputDebugString(TEXT("\n"));
	return;
}
LRESULT WINAPI WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	switch (Msg) {
	case WM_CREATE: {
		gTicketEngine = &TicketEngine::instance();
		gTicketEngine->SetLogCallback(LogCallback);
		gTicketEngine->InitializeBrowser(hWnd);
		break;
	}
	case WM_WEBVIEWINITIALIZED: {
		gTicketEngine->NavigateTo(L"https://my.sjtu.edu.cn/ui/task?by=history&from=kkframenew");
		break;
	}
	case WM_SIZE: {
		RECT bounds = {};
		GetClientRect(hWnd, &bounds);
		gTicketEngine->OnResize(bounds);
		break;
	}
	case WM_CLOSE: {
		gTicketEngine->OnClose();
		DestroyWindow(hWnd);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default: break;
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	CHECK_FAILURE(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
	TCHAR CLASS_NAME[16] = {}, WINDOW_NAME[128] = {};
	LoadString(hInstance, IDS_CLASSNAME, CLASS_NAME, 16);
	LoadString(hInstance, IDS_WINDOWNAME, WINDOW_NAME, 128);

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = CLASS_NAME;
	CHECK_FAILURE_BOOL(RegisterClassEx(&wc));

	HWND hWnd = CreateWindow(
		CLASS_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr, nullptr, hInstance, nullptr
	);
	CHECK_FAILURE_BOOL(hWnd);
	if (true && hWnd) {
		ShowWindow(hWnd, nShowCmd);
		UpdateWindow(hWnd);
	}

	TicketEngine::instance().SetLogCallback(LogCallback);
	TicketEngine::instance().InitializeBrowser(hWnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	gTicketEngine = nullptr;
	UnregisterClass(CLASS_NAME, hInstance);
	CoUninitialize();
	return static_cast<int>(msg.wParam);
}
