/**
 * @file mem_dc.cpp
 * @brief 内存设备上下文实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-16
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/mem_dc.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExMemDCCreate(uint32_t width, uint32_t height, ExMemDC* r_dc)
	{
		CHECK_PARAM(r_dc);

		//默认参数
		if (width == -1 || width == 0) { width = 1; }
		if (height == -1 || height == 0) { height = 1; }

		ExMemDC mdc{};
		try
		{
			//创建HDC
			mdc.dc = CreateCompatibleDC(NULL);
			throw_if_false(mdc.dc, E_FAIL, L"创建内存DC失败");

			//创建位图
			BITMAPINFO bi{};
			bi.bmiHeader.biSize = sizeof(bi);
			bi.bmiHeader.biWidth = width;
			bi.bmiHeader.biHeight = -(int32_t)height;
			bi.bmiHeader.biBitCount = 32;
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biCompression = BI_RGB;
			mdc.bitmap = CreateDIBSection(mdc.dc, &bi, DIB_RGB_COLORS,
				(void**)&mdc.bits, NULL, 0);
			throw_if_false(mdc.bitmap, E_FAIL, L"创建位图失败");

			//选择位图
			DeleteObject(SelectObject(mdc.dc, mdc.bitmap));

			//设置宽高
			mdc.width = width;
			mdc.height = height;

			//返回
			*r_dc = mdc;
			return S_OK;
		}
		catch_default({
			if (mdc.bitmap) { DeleteObject(mdc.bitmap); }
			if (mdc.dc) { DeleteDC(mdc.dc); }
			}
		);
	}
	HRESULT EXAPI EXCALL ExMemDCDestroy(ExMemDC* dc)
	{
		CHECK_PARAM(dc);
		if (dc->bitmap) { DeleteObject(dc->bitmap); }
		if (dc->dc) { DeleteDC(dc->dc); }
		*dc = {};
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExMemDCResize(ExMemDC* dc, uint32_t new_width, uint32_t new_height)
	{
		CHECK_PARAM(dc);
		if (new_width == dc->width && new_height == dc->height) { return S_OK; }
		
		ExMemDC new_dc{};
		handle_if_failed(
			ExMemDCCreate(new_width, new_height, &new_dc),
			L"重新创建内存DC失败"
		);
		ExMemDCDestroy(dc);
		*dc = new_dc;
		return S_OK;
	}

}


