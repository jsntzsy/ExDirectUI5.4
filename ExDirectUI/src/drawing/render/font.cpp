/**
 * @file font.cpp
 * @brief 渲染器字体实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/font.h"
#include "src/drawing/render/font.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExFontLoadFile(const byte_t* data, size_t size, EXATOM* r_atom)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_atom);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->LoadFontFile(data, size, r_atom);
	}

	HRESULT EXAPI EXCALL ExFontUnLoadFile(EXATOM atom)
	{
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->UnLoadFontFile(atom);
	}

	HRESULT EXAPI EXCALL ExFontCreate(IExFont** r_font)
	{
		CHECK_PARAM(r_font);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateFont(r_font);
	}

	HRESULT EXAPI EXCALL ExFontCreateFromName(const wchar_t name[LF_FACESIZE], uint32_t size,
		DWORD style, EXATOM file_atom, IExFont** r_font)
	{
		CHECK_PARAM(r_font);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateFontFromName(name, size, style, file_atom, r_font);
	}

	HRESULT EXAPI EXCALL ExFontCreateFromInfo(const ExFontInfo* info, IExFont** r_font)
	{
		CHECK_PARAM(info);
		CHECK_PARAM(r_font);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateFontFromInfo(info, r_font);
	}

}
