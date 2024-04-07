/**
 * @file linear_brush.cpp
 * @brief 渲染器线性渐变画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/linear_brush.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXOBJCALL ExLinearBrushCreate(float x_begin, float y_begin, float x_end, float y_end,
		EXARGB color_begin, EXARGB color_end, IExLinearBrush** r_brush)
	{
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateLinearBrush(x_begin, y_begin, x_end, y_end, color_begin, color_end, r_brush);
	}

	HRESULT EXAPI EXOBJCALL ExLinearBrushCreateEx(float x_begin, float y_begin, float x_end, float y_end,
		ExGradientPoint* gradient_points, uint32_t count, IExLinearBrush** r_brush)
	{
		CHECK_PARAM(gradient_points && count >= 2);
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateLinearBrushEx(x_begin, y_begin, x_end, y_end, gradient_points, count, r_brush);
	}

}
