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

namespace ExDirectUI
{
	IExRender* g_drawing_render = nullptr;

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

	///////////////////////

	HRESULT EXAPI EXCALL ExRenderGetFacotry(IExRender** r_factory)
	{
		CHECK_PARAM(r_factory);
		handle_if_false(!!g_drawing_render, EE_NOREADY, L"渲染引擎尚未就绪");
		*r_factory = g_drawing_render;
		g_drawing_render->AddRef();
		return S_OK;
	}

}
