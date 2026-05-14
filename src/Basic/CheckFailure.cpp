#include "Basic/pch.h"
#include "CheckFailure.h"

#include <iomanip>
#include <sstream>

void ShowFailure(HRESULT hr, const std::wstring& file, int line, const std::wstring& message) {
	std::wstringstream formatted;
	formatted << L"Failed at " << file << L"(" << line << L"): " << message << std::endl;
	formatted << "Error code: 0x" << std::hex << std::setw(8) << hr;
	MessageBox(nullptr, formatted.str().c_str(), nullptr, MB_ICONERROR | MB_OK);
	return;
}

void CheckFailure(HRESULT hr, const std::wstring& file, int line, const std::wstring& message) {
	if (FAILED(hr)) {
		ShowFailure(hr, file, line, message);
		FAIL_FAST();
	}
	return;
}
