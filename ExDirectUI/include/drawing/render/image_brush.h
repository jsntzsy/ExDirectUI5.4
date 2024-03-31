/**
 * @file image_brush.h
 * @brief 渲染器图像画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once
#include "brush.h"

namespace ExDirectUI
{
	EXINTERFACE("531D2A42-CDBB-471C-B525-3B223964DE00") IExImageBrush : public IExBrush
	{
		EXMETHOD HRESULT EXOBJCALL GetImage(IExImage * *r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL GetSourceRect(ExRectF* r_src_rect) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetImage(IExImage* image, ExRectF* src_rect = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL TransformToRect(float left, float top, float right, float bottom) PURE;

		EXMETHOD EXCHANNEL EXOBJCALL GetOpacity() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetOpacity(EXCHANNEL alpha) PURE;

		EXMETHOD HRESULT EXOBJCALL Flush() PURE;
	};

	/////////////////////////////////
	
	/**
	 * @brief 图像画刷_创建
	 * 通过图像对象创建图像画刷
	 * @param image 源图像对象
	 * @param src 源矩形 @可空
	 * @param extend_mode 扩展模式
	 * @param opacity 画刷透明度
	 * @param r_brush 接收的图像画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExImageBrushCreate(const IExImage* image, const ExRectF* src,
		ExBrushExtendMode extend_mode, EXCHANNEL opacity, IExImageBrush** r_brush);

}
