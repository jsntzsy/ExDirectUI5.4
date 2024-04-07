/**
 * @file factory.cpp
 * @brief 渲染器工厂类实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/factory.h"
#include "src/kernel/module_utils.h"
#include "src/drawing/render/factory.h"
#include "src/kernel/winapi.h"

namespace ExDirectUI
{
	IExRender* g_drawing_render = nullptr;
	ExFontInfo g_drawing_default_font{};
	DWORD g_drawing_antialias_mode = 0;
	uint32_t g_drawing_default_dpi = EX_DEFAULT_DPI;
	_UpdateLayeredWindowIndirectProc _UpdateLayeredWindowIndirect = nullptr;


	void EXCALL _ExRender_Init(const ExEngineInitInfo* info)
	{
		//如果提供了默认字体信息就使用
		if (info->default_font) { g_drawing_default_font = *info->default_font; }
		else {

			//否则就获取系统默认字体
			LOGFONTW sys_font{};
			if (SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(sys_font),
				&sys_font, false)) {
				g_drawing_default_font = ExFontInfo(&sys_font);
			}
			else {
				//如果还是不行就直接设置
				//TODO: 这里如果是XP可能要设置成宋体
				g_drawing_default_font = ExFontInfo(L"微软雅黑", 12, ExFontStyle::Normal);
			}
		}

		//设置抗锯齿模式
		if (info->antialias_mode != -1) { g_drawing_antialias_mode = info->antialias_mode; }
		else { g_drawing_antialias_mode = ExAntiAliasMode::Default; }

		//尝试获取UpdateLayeredWindowIndirect函数
		_UpdateLayeredWindowIndirect = (_UpdateLayeredWindowIndirectProc)
			GetProcAddress(g_winapi_user32, "UpdateLayeredWindowIndirect");

		//获取系统默认DPI
		HDC hdc = GetDC(NULL);
		if (hdc) {
			g_drawing_default_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
			ReleaseDC(NULL, hdc);
		}
		else { g_drawing_default_dpi = EX_DEFAULT_DPI; }

	}

	void EXCALL _ExRender_UnInit()
	{
		g_drawing_default_font = {};
		g_drawing_antialias_mode = 0;
		g_drawing_default_dpi = EX_DEFAULT_DPI;
		_UpdateLayeredWindowIndirect = nullptr;
	}

	HRESULT EXCALL _ExRender_Group(IExRender* render)
	{
		//渲染器如果已经存在了,就不设置了
		if (g_drawing_render) { return S_FALSE; }

		render->AddRef();
		g_drawing_render = render;
		return S_OK;
	}

	HRESULT EXCALL _ExRender_UnGroup(IExRender* render)
	{
		if (g_drawing_render == render)
		{
			g_drawing_render->Release();
			g_drawing_render = nullptr;
		}
		return S_OK;
	}

	///////////////////////////

	bool EXAPI EXCALL ExRenderIsSupportComposition()
	{
		if (g_drawing_render == nullptr) { return false; }
		return g_drawing_render->IsSupportComposition();
	}

}
