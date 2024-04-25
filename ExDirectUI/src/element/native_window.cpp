/**
 * @file native_window.cpp
 * @brief 原生窗口操作实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */

#include "stdafx.h"
#include "element/native_window.h"
#include "src/kernel/winapi.h"
#include "src/app/engine.h"
#include "src/drawing/render/factory.h"
#include "src/element/native_window.h"

namespace ExDirectUI
{
	ATOM g_element_default_wnd_class_atom = 0;
	_GetDpiForMonitorProc _GetDpiForMonitor = nullptr;
	

	void EXCALL _ExWnd_Init(const ExEngineInitInfo* init_info)
	{
		if (init_info->default_class_name) {
			g_element_default_wnd_class_atom = ExWndRegister(
				init_info->default_class_name, DefWindowProcW
			);
		}
		
		if (g_winapi_shcore) {
			_GetDpiForMonitor = (_GetDpiForMonitorProc)
				GetProcAddress(g_winapi_shcore, "GetDpiForMonitor");
		}
		
	}

	void EXCALL _ExWnd_UnInit()
	{
		_GetDpiForMonitor = nullptr;

		if (g_element_default_wnd_class_atom) {
			UnregisterClassW(
				(LPCWSTR)g_element_default_wnd_class_atom, 
				g_engine_instance
			);
		}
		
	}

	ATOM EXAPI EXCALL ExWndRegister(LPCWSTR class_name, WNDPROC wnd_proc, DWORD style)
	{
		return ExWndRegisterEx(class_name, wnd_proc, style);
	}
	ATOM EXAPI EXCALL ExWndRegisterEx(LPCWSTR class_name, WNDPROC wnd_proc, DWORD style,
		HICON icon, HICON icon_small, HCURSOR cursor, HBRUSH brush)
	{
		CHECK_PARAM_RET(class_name, 0);
		CHECK_PARAM_RET(wnd_proc, 0);

		WNDCLASSEXW cls{};
		cls.cbSize = sizeof(cls);
		cls.style = style != -1 ? style : (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
		cls.lpfnWndProc = wnd_proc;
		cls.cbClsExtra = 0;
		cls.cbWndExtra = 0;
		cls.hInstance = g_engine_instance;
		cls.hIcon = icon;
		cls.hIconSm = icon_small;
		cls.hCursor = cursor ? cursor : LoadCursor(NULL, IDC_ARROW);
		cls.hbrBackground = brush ? brush : (HBRUSH)COLOR_WINDOW;
		cls.lpszClassName = class_name;
		cls.lpszMenuName = nullptr;

		return RegisterClassExW(&cls);
	}

	HWND EXAPI EXCALL ExWndCreate(HWND parent, int left, int top, int width, int height,
		LPCWSTR class_name, LPCWSTR window_name, DWORD style, DWORD style_ex, LPVOID param)
	{
		CHECK_PARAM_RET(class_name, NULL);

		return CreateWindowExW(
			style_ex != -1 ? style_ex : WS_EX_APPWINDOW,
			class_name,
			window_name,
			style != -1 ? style : WS_OVERLAPPEDWINDOW,
			left, top, width, height,
			parent,
			NULL,
			g_engine_instance,
			param
		);
	}

	bool EXAPI EXCALL ExWndCenterTo(HWND window, HWND target_window, bool full_screen)
	{
		CHECK_PARAM_RET(window, false);

		ExRect rect{}, target_rect{};
		return_if_false(GetWindowRect(window, &rect), {}, false);

		if (target_window == NULL) {
			HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
			return_if_false(monitor, {}, false);

			MONITORINFO mi{};
			mi.cbSize = sizeof(mi);
			return_if_false(GetMonitorInfo(monitor, &mi), {}, false);

			if (full_screen) { target_rect = mi.rcMonitor; }
			else { target_rect = mi.rcWork; }
		}
		else {
			return_if_false(GetWindowRect(target_window, &target_rect), {}, false);
		}

		rect = target_rect.CenterRect(rect.Width(), rect.Height());
		return SetWindowPos(window, NULL, rect.left, rect.top,
			0, 0, SWP_NOREDRAW | SWP_NOZORDER | SWP_NOSIZE
		);
	}

	WNDPROC EXAPI EXCALL ExWndSubClass(HWND window, WNDPROC new_proc)
	{
		CHECK_PARAM_RET(window, nullptr);
		CHECK_PARAM_RET(new_proc, nullptr);
		
		return (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)new_proc);
	}

	DWORD EXAPI EXCALL ExWndModifyStyle(HWND window, DWORD style_add, DWORD style_remove, bool ex_style)
	{
		CHECK_PARAM_RET(window, 0);

		int index = ex_style ? GWL_EXSTYLE : GWL_STYLE;
		DWORD style = (DWORD)GetWindowLong(window, index);
		
		style |= style_add;
		style &= ~style_remove;

		return (DWORD)SetWindowLong(window, index, style);
	}

	WPARAM EXAPI EXCALL ExWndMessageLoop()
	{
		MSG msg{};
		while (GetMessageW(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return msg.wParam;
	}

	void EXAPI EXCALL ExWndDoEvent()
	{
		MSG msg{};
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			//if (!IsDialogMessageW(NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	HICON EXAPI EXCALL ExWndGetHICON(HWND window, bool small_icon)
	{
		HICON icon = (HICON)SendMessageW(window, WM_GETICON, small_icon ? ICON_SMALL : ICON_BIG, 0);
		if (!icon) {
			icon = (HICON)GetClassLongPtrW(window, small_icon ? GCLP_HICONSM : GCLP_HICON);
		}
		return icon;
	}

	uint32_t EXAPI EXCALL ExWndGetDPI(HWND window)
	{
		uint32_t dpi = g_drawing_default_dpi;
		
		if (_GetDpiForMonitor && window) {
			HMONITOR monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
			if (monitor) {
				UINT dpi_x = 0, dpi_y = 0;
				if (SUCCEEDED(_GetDpiForMonitor(monitor, 0, &dpi_x, &dpi_y))) { 	//MDT_EFFECTIVE_DPI
					dpi = dpi_x;
				}
			}
		}

		return dpi;
	}

}


