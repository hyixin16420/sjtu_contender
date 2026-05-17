#include "Basic/pch.h"
#include "Basic/CheckFailure.h"

#include "resource.h"
#include "window_messages.h"
#include "TicketEngine.h"

#include <opencv2/opencv.hpp>

cv::Mat* gmat = nullptr;

LRESULT WINAPI WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	switch (Msg) {
	case WM_CREATE: {
		TicketEngine::create();
		TicketEngine::instance().InitializeBrowser(hWnd);
		break;
	}

	case WM_WEBVIEWINITIALIZED: {
		TicketEngine::instance().NavigateTo(L"https://my.sjtu.edu.cn/");
		// TicketEngine::instance().NavigateTo(L"https://www.baidu.com/");
		break;
	}
	case WM_WEBVIEWEXITED: {
		TicketEngine::instance().destroy();
		DestroyWindow(hWnd);
		break;
	}
	case WM_WEBVIEWCAPTURECOMPLETED: {
		cv::Mat templateImg1 = cv::imread("template1.png");
		cv::Mat templateImg2 = cv::imread("template2.png");
		cv::Mat& img = *gmat;

		cv::cvtColor(templateImg1, templateImg1, cv::COLOR_BGR2GRAY);
		cv::cvtColor(templateImg2, templateImg2, cv::COLOR_BGR2GRAY);
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

		cv::Mat result1, result2;
		cv::matchTemplate(img, templateImg1, result1, cv::TM_CCOEFF_NORMED);
		cv::matchTemplate(img, templateImg2, result2, cv::TM_CCOEFF_NORMED);
		double minVal1, maxVal1, minVal2, maxVal2;
		cv::Point minLoc1, maxLoc1, minLoc2, maxLoc2;
		cv::minMaxLoc(result1, &minVal1, &maxVal1, &minLoc1, &maxLoc1);
		cv::minMaxLoc(result2, &minVal2, &maxVal2, &minLoc2, &maxLoc2);
		if (maxVal1 < 0.8 || maxVal2 < 0.8) {
			TicketEngine::instance().LogOut(L"匹配失败！");
		}
		else {
			TicketEngine::instance().LogOut(L"匹配成功！");
		}
		cv::Point clickPt1(maxLoc1.x + templateImg1.cols / 2, maxLoc1.y + templateImg1.rows / 2);
		cv::Point clickPt2(maxLoc2.x + templateImg2.cols / 2, maxLoc2.y + templateImg2.rows / 2);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);  // clientRect.left/right 此时是 0
		POINT pt = { clientRect.left, clientRect.top };
		ClientToScreen(hWnd, &pt);

		int virtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int virtualHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		INPUT inputs[38] = {};
		inputs[0].type = INPUT_MOUSE;
		inputs[0].mi.dx = (pt.x + clickPt1.x) * 65535 / virtualWidth;
		inputs[0].mi.dy = (pt.y + clickPt1.y) * 65535 / virtualHeight;
		inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;

		inputs[1].type = INPUT_MOUSE;
		inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		inputs[2].type = INPUT_MOUSE;
		inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

		inputs[3].type = INPUT_KEYBOARD;
		inputs[3].ki.wVk = 'C';

		inputs[4].type = INPUT_KEYBOARD;
		inputs[4].ki.wVk = 'C';
		inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;
		
		inputs[5].type = INPUT_KEYBOARD;
		inputs[5].ki.wVk = 'A';

		inputs[6].type = INPUT_KEYBOARD;
		inputs[6].ki.wVk = 'A';
		inputs[6].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[7].type = INPUT_KEYBOARD;
		inputs[7].ki.wVk = 'S';

		inputs[8].type = INPUT_KEYBOARD;
		inputs[8].ki.wVk = 'S';
		inputs[8].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[9].type = INPUT_KEYBOARD;
		inputs[9].ki.wVk = 'S';

		inputs[10].type = INPUT_KEYBOARD;
		inputs[10].ki.wVk = 'S';
		inputs[10].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[11].type = INPUT_KEYBOARD;
		inputs[11].ki.wVk = 'I';

		inputs[12].type = INPUT_KEYBOARD;
		inputs[12].ki.wVk = 'I';
		inputs[12].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[13].type = INPUT_KEYBOARD;
		inputs[13].ki.wVk = 'E';

		inputs[14].type = INPUT_KEYBOARD;
		inputs[14].ki.wVk = 'E';
		inputs[14].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[15].type = INPUT_KEYBOARD;
		inputs[15].ki.wVk = 'L';

		inputs[16].type = INPUT_KEYBOARD;
		inputs[16].ki.wVk = 'L';
		inputs[16].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[17].type = INPUT_KEYBOARD;
		inputs[17].ki.wVk = VK_LSHIFT;

		inputs[18].type = INPUT_KEYBOARD;
		inputs[18].ki.wVk = VK_OEM_MINUS;

		inputs[19].type = INPUT_KEYBOARD;
		inputs[19].ki.wVk = VK_OEM_MINUS;
		inputs[19].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[20].type = INPUT_KEYBOARD;
		inputs[20].ki.wVk = VK_LSHIFT;
		inputs[20].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[21].type = INPUT_KEYBOARD;
		inputs[21].ki.wVk = 'F';

		inputs[22].type = INPUT_KEYBOARD;
		inputs[22].ki.wVk = 'F';
		inputs[22].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[23].type = INPUT_KEYBOARD;
		inputs[23].ki.wVk = 'A';

		inputs[24].type = INPUT_KEYBOARD;
		inputs[24].ki.wVk = 'A';
		inputs[24].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[25].type = INPUT_KEYBOARD;
		inputs[25].ki.wVk = 'N';

		inputs[26].type = INPUT_KEYBOARD;
		inputs[26].ki.wVk = 'N';
		inputs[26].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[27].type = INPUT_KEYBOARD;
		inputs[27].ki.wVk = 'G';

		inputs[28].type = INPUT_KEYBOARD;
		inputs[28].ki.wVk = 'G';
		inputs[28].ki.dwFlags = KEYEVENTF_KEYUP;
		// inputs[28].ki.time = 1000;

		inputs[29].type = INPUT_MOUSE;
		inputs[29].mi.dx = (pt.x + clickPt2.x) * 65535 / virtualWidth;
		inputs[29].mi.dy = (pt.y + clickPt2.y) * 65535 / virtualHeight;
		inputs[29].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;

		inputs[30].type = INPUT_MOUSE;
		inputs[30].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

		inputs[31].type = INPUT_MOUSE;
		inputs[31].mi.dwFlags = MOUSEEVENTF_LEFTUP;

		inputs[32].type = INPUT_KEYBOARD;
		inputs[32].ki.wVk = VK_NUMPAD1;

		inputs[33].type = INPUT_KEYBOARD;
		inputs[33].ki.wVk = VK_NUMPAD1;
		inputs[33].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[34].type = INPUT_KEYBOARD;
		inputs[34].ki.wVk = VK_NUMPAD2;

		inputs[35].type = INPUT_KEYBOARD;
		inputs[35].ki.wVk = VK_NUMPAD2;
		inputs[35].ki.dwFlags = KEYEVENTF_KEYUP;

		inputs[36].type = INPUT_KEYBOARD;
		inputs[36].ki.wVk = VK_NUMPAD3;

		inputs[37].type = INPUT_KEYBOARD;
		inputs[37].ki.wVk = VK_NUMPAD3;
		inputs[37].ki.dwFlags = KEYEVENTF_KEYUP;

		SendInput(29, inputs, sizeof(INPUT));
		// SendInput(9, inputs+29, sizeof(INPUT));

		delete gmat;
		gmat = nullptr;
		break;
	}

	case WM_SIZE: {
		RECT bounds = {};
		GetClientRect(hWnd, &bounds);
		TicketEngine::instance().OnResize(bounds);
		break;
	}
	case WM_KEYDOWN: {
		TicketEngine& ins = TicketEngine::instance();
		if (wParam == VK_SPACE) {
			gmat = new cv::Mat;
			ins.LogOut(L"开始截图");
			ins.Capture(*gmat);
		}
		break;
	}
	case WM_KILLFOCUS: {
		TicketEngine::instance().LogOut(L"失去焦点");
		break;
	}
	
	case WM_SYSCOMMAND: {
		if (LOWORD(wParam) == SC_CLOSE) {
			ShowWindow(hWnd, SW_HIDE);
			TicketEngine::instance().OnClose();
			return 0;
		}
		else {
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}
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
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
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

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(CLASS_NAME, hInstance);
	CoUninitialize();
	return static_cast<int>(msg.wParam);
}
