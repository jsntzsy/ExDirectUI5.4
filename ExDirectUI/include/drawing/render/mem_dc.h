/**
 * @file mem_dc.h
 * @brief 内存设备上下文头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 内存设备上下文
	struct ExMemDC
	{
		///宽度
		uint32_t width;

		///高度
		uint32_t height;
		
		///设备上下文句柄
		HDC dc;
		
		///位图句柄
		HBITMAP bitmap;

		///位图像素数据
		EXBITSDATA* bits;
	};

	/////////////////////////////////

	HRESULT EXAPI EXCALL ExMemDCCreate(uint32_t width, uint32_t height, ExMemDC* r_dc);
	HRESULT EXAPI EXCALL ExMemDCDestroy(ExMemDC* dc);
	HRESULT EXAPI EXCALL ExMemDCResize(ExMemDC* dc, uint32_t new_width, uint32_t new_height);

	BOOL EXAPI EXCALL ExUpdateLayeredWindow(HWND window, const UPDATELAYEREDWINDOWINFO* ulwi);

}
