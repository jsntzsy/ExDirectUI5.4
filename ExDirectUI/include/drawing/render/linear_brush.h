/**
 * @file linear_brush.h
 * @brief 渲染器线性渐变画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once
#include "brush.h"

namespace ExDirectUI
{
	EXINTERFACE("7670B012-D197-42CF-95A4-01F7ED988895") IExLinearBrush : public IExBrush
	{
		EXMETHOD EXARGB EXOBJCALL GetBeginColor() const PURE;
		EXMETHOD EXARGB EXOBJCALL GetEndColor() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetBeginColor(EXARGB color) PURE;
		EXMETHOD HRESULT EXOBJCALL SetEndColor(EXARGB color) PURE;
		EXMETHOD HRESULT EXOBJCALL GetColors(EXARGB* r_begin_color, EXARGB* r_end_color) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetColors(EXARGB begin_color, EXARGB end_color) PURE;

		EXMETHOD HRESULT EXOBJCALL GetBeginPoint(ExPointF* r_point) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetEndPoint(ExPointF* r_point) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetPoints(ExPointF* r_begin_point, ExPointF* r_end_point) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetBeginPoint(const ExPointF* point) PURE;
		EXMETHOD HRESULT EXOBJCALL SetEndPoint(const ExPointF* point) PURE;
		EXMETHOD HRESULT EXOBJCALL SetPoints(const ExPointF* begin_point, const ExPointF* end_point) PURE;

		EXMETHOD uint32_t EXOBJCALL GetGradientPointCount() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetGradientPoints(const ExGradientPoint* points, uint32_t count) PURE;

		EXMETHOD bool EXOBJCALL GetGammaMode() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetGammaMode(bool gamma) PURE;
	};

}
