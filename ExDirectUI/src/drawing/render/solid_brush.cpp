/**
 * @file solid_brush.cpp
 * @brief 渲染器纯色画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-01
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/solid_brush.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{

	HRESULT EXAPI EXCALL ExSolidBrushCreate(EXARGB color, IExSolidBrush** r_brush)
	{
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		handle_if_failed(
			g_drawing_render->CreateSolidBrush(color,r_brush),
			L"创建XXX对象失败"
		);
		return S_OK;
	}

}