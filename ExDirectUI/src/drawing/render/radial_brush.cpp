/**
 * @file radial_brush.cpp
 * @brief 渲染器径向渐变画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-01
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/radial_brush.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExRadialBrushCreate(float left, float top, float right, float bottom,
		EXARGB color_inside, EXARGB color_outside, IExRadialBrush** r_brush)
	{
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateRadialBrush(left, top, right, bottom, color_inside, color_outside, r_brush);
	}

	HRESULT EXAPI EXCALL ExRadialBrushCreateEx(float left, float top, float right, float bottom,
		ExGradientPoint* gradient_points, uint32_t count, IExRadialBrush** r_brush)
	{
		CHECK_PARAM(gradient_points && count >= 2);
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateRadialBrushEx(left, top, right, bottom, gradient_points, count, r_brush);
	}


}
