/**
 * @file device.h
 * @brief 渲染器渲染设备头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	enum ExDeviceType
	{
		EX_DEVICE_WINDOW,
		EX_DEVICE_COMPOSITION_WINDOW,
		EX_DEVICE_BITMAP,
	};

	EXINTERFACE("827442C1-D85C-4A8B-9EF9-99AC9AFC9AA9") IExDevice : public IExRenderObject
	{
		EXMETHOD ExDeviceType EXOBJCALL GetDeviceType() const PURE;
		EXMETHOD HANDLE EXOBJCALL GetObject() const PURE;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const PURE;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) PURE;

		EXMETHOD HRESULT EXOBJCALL Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc) PURE;
	};

}
