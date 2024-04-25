/**
 * @file native_window.h
 * @brief 原生窗口内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	extern ATOM g_element_default_wnd_class_atom;

	typedef HRESULT(WINAPI* _GetDpiForMonitorProc)(
		HMONITOR monitor,
		DWORD dpi_type,
		UINT* r_dpi_x,
		UINT* r_dpi_y
	);

	extern _GetDpiForMonitorProc _GetDpiForMonitor;
	

	void EXCALL _ExWnd_Init(const ExEngineInitInfo* init_info);
	void EXCALL _ExWnd_UnInit();

		
}
