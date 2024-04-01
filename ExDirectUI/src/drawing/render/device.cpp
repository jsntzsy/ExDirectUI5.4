/**
 * @file device.cpp
 * @brief 渲染器设备实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-24
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/device.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	BOOL EXAPI EXCALL ExUpdateLayeredWindow(HWND window, const UPDATELAYEREDWINDOWINFO* ulwi)
	{
		CHECK_PARAM(window);
		CHECK_PARAM(ulwi);

		if (_UpdateLayeredWindowIndirect) {
			return _UpdateLayeredWindowIndirect(window, ulwi);
		}
		else {
			return UpdateLayeredWindow(
				window, ulwi->hdcDst,
				const_cast<POINT*>(ulwi->pptDst),
				const_cast<SIZE*>(ulwi->psize),
				ulwi->hdcSrc,
				const_cast<POINT*>(ulwi->pptSrc),
				ulwi->crKey,
				const_cast<BLENDFUNCTION*>(ulwi->pblend),
				ulwi->dwFlags
			);
		}
	}

	HRESULT EXAPI EXCALL ExDeviceCreateToBitmap(uint32_t width, uint32_t height, IExDevice** r_device)
	{
		CHECK_PARAM(r_device);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		handle_if_failed(
			g_drawing_render->CreateBitmapDevice(width, height, r_device),
			L"创建位图设备对象失败"
		);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExDeviceCreateToWindow(HWND window, bool composition_mode, IExDevice** r_device)
	{
		CHECK_PARAM(window);
		CHECK_PARAM(r_device);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		handle_if_failed(
			g_drawing_render->CreateWindowDevice(
				window,
				composition_mode ? ExDeviceType::CompositionWindow : ExDeviceType::Window,
				r_device
			), L"创建窗口设备对象失败"
		);
		return S_OK;
	}

}
