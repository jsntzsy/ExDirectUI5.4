/**
 * @file factory.h
 * @brief 渲染器工厂头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	constexpr uint32_t EX_DEFAULT_DPI = 96;

	extern IExRender* g_drawing_render;
	extern ExFontInfo g_drawing_default_font;
	extern DWORD g_drawing_antialias_mode;
	extern uint32_t g_drawing_default_dpi;

	typedef BOOL(WINAPI* _UpdateLayeredWindowIndirectProc)(HWND window, const UPDATELAYEREDWINDOWINFO* ulwi);
	extern _UpdateLayeredWindowIndirectProc _UpdateLayeredWindowIndirect;

	void EXCALL _ExRender_Init(const ExEngineInitInfo* info);
	void EXCALL _ExRender_UnInit();

	HRESULT EXCALL _ExRender_Group(IExRender* render);
	HRESULT EXCALL _ExRender_UnGroup(IExRender* render);

	
}

