/**
 * @file image.cpp
 * @brief 渲染器图像函数实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-20
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/image.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	
	HRESULT EXAPI EXCALL ExImageCreate(uint32_t width, uint32_t height, IExImage** r_image)
	{
		CHECK_PARAM(r_image);
		CHECK_PARAM(width > 0 && height > 0);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImage(width, height, r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromFile(LPCWSTR file, IExImage** r_image)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageFromFile(file, r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromMemory(const byte_t* data, size_t size, IExImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageFromMemory(data, size, r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromBitsData(uint32_t width, uint32_t height,
		const EXBITSDATA bits_data, uint32_t stride, IExImage** r_image)
	{
		CHECK_PARAM(r_image);
		CHECK_PARAM(width > 0 && height > 0);
		CHECK_PARAM(bits_data);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageFromBitsData(width, height, bits_data, stride, r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromImageData(const byte_t* data, IExImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageFromImageData(data, r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromHBITMAP(HBITMAP bitmap, HPALETTE palette, bool pre_alpha, IExImage** r_image)
	{
		CHECK_PARAM(bitmap);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageFromHBITMAP(bitmap, palette, pre_alpha, r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromHICON(HICON icon, IExImage** r_image)
	{
		CHECK_PARAM(icon);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateImageFromHICON(icon, r_image);
	}

}

