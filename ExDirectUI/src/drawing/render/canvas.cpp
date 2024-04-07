/**
 * @file canvas.cpp
 * @brief 渲染器画布对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-01
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/canvas.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{

	HRESULT EXAPI EXCALL ExCanvasCreate(uint32_t width, uint32_t height, IExCanvas** r_canvas)
	{
		CHECK_PARAM(r_canvas);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateCanvas(width, height, r_canvas);
	}

}
