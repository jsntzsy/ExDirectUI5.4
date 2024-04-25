/**
 * @file native_window.h
 * @brief 原生窗口操作头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	ATOM EXAPI EXCALL ExWndRegister(LPCWSTR class_name, WNDPROC wnd_proc, DWORD style = -1);
	ATOM EXAPI EXCALL ExWndRegisterEx(LPCWSTR class_name, WNDPROC wnd_proc, DWORD style = -1,
		HICON icon = NULL, HICON icon_small = NULL, HCURSOR cursor = NULL, HBRUSH brush = NULL);
	HWND EXAPI EXCALL ExWndCreate(HWND parent, int left, int top, int width, int height,
		LPCWSTR class_name, LPCWSTR window_name, DWORD style = -1, DWORD style_ex = -1, LPVOID param = nullptr);
	bool EXAPI EXCALL ExWndCenterTo(HWND window, HWND target_window = NULL, bool full_screen = false);
	WNDPROC EXAPI EXCALL ExWndSubClass(HWND window, WNDPROC new_proc);
	DWORD EXAPI EXCALL ExWndModifyStyle(HWND window, DWORD style_add = 0, DWORD style_remove = 0, bool ex_style = false);
	WPARAM EXAPI EXCALL ExWndMessageLoop();
	void EXAPI EXCALL ExWndDoEvent();
	HICON EXAPI EXCALL ExWndGetHICON(HWND window, bool small_icon);
	uint32_t EXAPI EXCALL ExWndGetDPI(HWND window);

}
