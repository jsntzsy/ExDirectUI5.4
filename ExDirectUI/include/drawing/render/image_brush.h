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
		EXMETHOD HRESULT EXOBJCALL GetImage(IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL GetSourceRect(ExRectF* r_src_rect) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetImage(IExImage* image, ExRectF* src_rect = nullptr) PURE;

		EXMETHOD EXCHANNEL EXOBJCALL GetOpacity() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetOpacity(EXCHANNEL alpha) PURE;
		
		EXMETHOD HRESULT EXOBJCALL Flush() PURE;
	}

}
