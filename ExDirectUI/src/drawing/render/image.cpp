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

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImage(width, height, &image),
			L"创建空图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromFile(LPCWSTR file, IExImage** r_image)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromFile(file, &image),
			L"从文件创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromMemory(const byte_t* data, size_t size, IExImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromMemory(data, size, &image),
			L"从内存创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromBitsData(uint32_t width, uint32_t height,
		const EXBITSDATA bits_data, uint32_t stride, IExImage** r_image)
	{
		CHECK_PARAM(r_image);
		CHECK_PARAM(width > 0 && height > 0);
		CHECK_PARAM(bits_data);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromBitsData(width, height, bits_data, stride, &image),
			L"从像素数据创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromImageData(const byte_t* data, IExImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromImageData(data, &image),
			L"从图像数据创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromHBITMAP(HBITMAP bitmap, HPALETTE palette, bool pre_alpha, IExImage** r_image)
	{
		CHECK_PARAM(bitmap);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromHBITMAP(bitmap, palette, pre_alpha, &image),
			L"从位图句柄创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromHICON(HICON icon, IExImage** r_image)
	{
		CHECK_PARAM(icon);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromHICON(icon, &image),
			L"从图标句柄创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

}

