#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

LRESULT WINAPI WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	switch (Msg) {
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
	const TCHAR CLASS_NAME[] = TEXT("SJTU_CONTENDER");

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = CLASS_NAME;

	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, TEXT("Register window class failed!"), TEXT("Error"), MB_OK);
		return 0;
	}

	HWND hWnd = CreateWindow(
		CLASS_NAME, TEXT("Basic Window"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr, nullptr, hInstance, nullptr
	);
	if (!hWnd) {
		UnregisterClass(CLASS_NAME, hInstance);
		MessageBox(nullptr, TEXT("Window creation failed!"), TEXT("Error"), MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnregisterClass(CLASS_NAME, hInstance);
	return static_cast<int>(msg.wParam);
}
