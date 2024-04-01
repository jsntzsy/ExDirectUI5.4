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
	EXENUM(ExDeviceType)
	{
		Window,
			CompositionWindow,
			Bitmap,
	};

	EXINTERFACE("827442C1-D85C-4A8B-9EF9-99AC9AFC9AA9") IExDevice : public IExRenderObject
	{
		EXMETHOD ExDeviceType EXOBJCALL GetDeviceType() const PURE;
		EXMETHOD HANDLE EXOBJCALL GetObject() const PURE;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const PURE;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) PURE;

		EXMETHOD HRESULT EXOBJCALL Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc) PURE;
	};

	////////////////////

	/**
	 * @brief 窗口_更新层窗口
	 * 更新层窗口
	 * @param window 更新的窗口句柄
	 * @param ulwi 更新层窗口信息
	 * @return 返回是否成功
	 */
	BOOL EXAPI EXCALL ExUpdateLayeredWindow(HWND window, const UPDATELAYEREDWINDOWINFO* ulwi);

	/**
	 * @brief 设备_创建到位图
	 * 创建内存位图设备对象
	 * @param width 宽度
	 * @param height 高度
	 * @param r_device 接收创建的设备对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExDeviceCreateToBitmap(uint32_t width, uint32_t height, IExDevice** r_device);

	/**
	 * @brief 设备_创建到窗口
	 * 创建窗口设备对象
	 * @param window 窗口句柄
	 * @param composition_mode 是否启用混合窗口设备模式
	 * @param r_device 接收创建的设备对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExDeviceCreateToWindow(HWND window, bool composition_mode, IExDevice** r_device);


}
