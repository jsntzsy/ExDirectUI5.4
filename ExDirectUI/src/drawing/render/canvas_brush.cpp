/**
 * @file canvas_brush.cpp
 * @brief 渲染器画布画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/canvas_brush.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExCanvasBrushCreate(const IExCanvas* canvas, ExBrushExtendMode extend_mode,
		EXCHANNEL alpha, IExCanvasBrush** r_brush)
	{
		CHECK_PARAM(canvas);
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateCanvasBrush(canvas, extend_mode, alpha, r_brush);
	}
	HRESULT EXAPI EXCALL ExCanvasBrushCreateFromTarget(const IExCanvasTarget* target, 
		ExBrushExtendMode extend_mode, EXCHANNEL alpha, IExCanvasBrush** r_brush)
	{
		CHECK_PARAM(target);
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateCanvasBrushFromTarget(target, extend_mode, alpha, r_brush);
	}
}


