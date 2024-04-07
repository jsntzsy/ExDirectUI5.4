/**
 * @file image_brush.cpp
 * @brief 渲染器图像画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/image_brush.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExImageBrushCreate(const IExImage* image, const ExRectF* src,
		ExBrushExtendMode extend_mode, EXCHANNEL opacity, IExImageBrush** r_brush)
	{
		CHECK_PARAM(image);
		CHECK_PARAM(r_brush);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageBrush(image, src, extend_mode, opacity, r_brush);
	}

}
