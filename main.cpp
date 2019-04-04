// CapsToggle.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "main.h"

#define LONG_PRESS_THRESHOLD 500

HHOOK hHook;
ULONGLONG pressTick = 0;
BOOL longPress = FALSE;

int ToggleIME() {

	INPUT input;

	input.type = INPUT_KEYBOARD;
	input.ki.time = 0;
	input.ki.wScan = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.dwFlags = 0;

	// Simulate Key Press
	input.ki.wVk = VK_CONTROL;
	SendInput(1, &input, sizeof(INPUT));

	input.ki.wVk = VK_SPACE;
	SendInput(1, &input, sizeof(INPUT));

	// Simulate Key Up
	input.ki.dwFlags = KEYEVENTF_KEYUP;

	input.ki.wVk = VK_SPACE;
	SendInput(1, &input, sizeof(INPUT));

	input.ki.wVk = VK_CONTROL;
	SendInput(1, &input, sizeof(INPUT));

	return 0;
}

LRESULT CALLBACK KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

	if (nCode >= 0 && p->vkCode == VK_CAPITAL) {

		if (wParam == WM_KEYDOWN && !longPress) {

			ULONGLONG currentTick = GetTickCount64();

			if (pressTick == 0) {
				pressTick = currentTick;
			}

			ULONGLONG pressTime = currentTick - pressTick;
			if (pressTime > LONG_PRESS_THRESHOLD) {
				longPress = TRUE;
			}

			return 1;
		}

		if (wParam == WM_KEYUP) {

			pressTick = 0;

			if (!longPress) {
				ToggleIME();

				return 1;
			}

			longPress = FALSE;
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyBoardProc, hInstance, 0);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAPSTOGGLE));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnhookWindowsHookEx(hHook);

	return (int)msg.wParam;
}

