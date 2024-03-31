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
		EXMETHOD HRESULT EXOBJCALL SetBeginPoint(float x, float y) PURE;
		EXMETHOD HRESULT EXOBJCALL SetEndPoint(float x, float y) PURE;
		EXMETHOD HRESULT EXOBJCALL SetPoints(const ExPointF* begin_point, const ExPointF* end_point) PURE;

		EXMETHOD uint32_t EXOBJCALL GetGradientPointCount() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetGradientPoints(const ExGradientPoint* points, uint32_t count) PURE;

		EXMETHOD bool EXOBJCALL GetGammaMode() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetGammaMode(bool gamma) PURE;
	};

	//////////////////////////////

	/**
	 * @brief 线性渐变画刷_创建
	 * 通过颜色创建线性渐变画刷
	 * @param x_begin 起点X
	 * @param y_begin 起点Y
	 * @param x_end 终点X
	 * @param y_end 终点Y
	 * @param color_begin 起点颜色 
	 * @param color_end 终点颜色
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXOBJCALL ExLinearBrushCreate(float x_begin, float y_begin, float x_end, float y_end,
		EXARGB color_begin, EXARGB color_end, IExLinearBrush** r_brush);

	/**
	 * @brief 线性渐变画刷_创建
	 * 通过颜色创建线性渐变画刷
	 * @param x_begin 起点X
	 * @param y_begin 起点Y
	 * @param x_end 终点X
	 * @param y_end 终点Y
	 * @param gradient_points 渐变点数组指针
	 * @param count 渐变点数量 <需要至少两个>
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXOBJCALL ExLinearBrushCreateEx(float x_begin, float y_begin, float x_end, float y_end,
		ExGradientPoint* gradient_points, uint32_t count, IExLinearBrush** r_brush);

}
