/**
 * @file region.cpp
 * @brief 渲染器区域对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-07
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/region.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{

	HRESULT EXAPI EXCALL ExRegionCreate(IExRegion** r_region, bool infinite)
	{
		CHECK_PARAM(r_region);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		if (infinite) { return g_drawing_render->CreateInfinityRegion(r_region); }
		else { return g_drawing_render->CreateEmptyRegion(r_region); }
	}

	HRESULT EXAPI EXCALL ExRegionCreateFromRect(float left, float top, float right, float bottom, IExRegion** r_region)
	{
		CHECK_PARAM(r_region);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateRectRegion(left, top, right, bottom, r_region);
	}

	HRESULT EXAPI EXCALL ExRegionCreateFromPath(const IExPath* path, const ExMatrixElements* tranform, IExRegion** r_region)
	{
		CHECK_PARAM(path);
		CHECK_PARAM(r_region);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreatePathRegion(path, tranform, r_region);
	}
}

