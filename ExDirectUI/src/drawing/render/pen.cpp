/**
 * @file pen.cpp
 * @brief 渲染器画笔对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-07
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/pen.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExPenCreate(EXARGB color, float stroke_width, IExPen** r_pen)
	{
		CHECK_PARAM(r_pen);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreatePen(color, stroke_width, r_pen);
	}

	HRESULT EXAPI EXCALL ExPenCreateFromBrush(IExBrush* brush, float stroke_width, IExPen** r_pen)
	{
		CHECK_PARAM(brush);
		CHECK_PARAM(r_pen);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreatePenFromBrush(brush, stroke_width, r_pen);
	}

}
