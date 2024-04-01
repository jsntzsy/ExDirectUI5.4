/**
 * @file radial_brush.h
 * @brief 渲染器径向渐变画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once
#include "brush.h"

namespace ExDirectUI
{
	EXINTERFACE("E8A07A2F-425D-43D0-9A7F-83E61B7F8C61") IExRadialBrush : public IExBrush
	{
		EXMETHOD EXARGB EXOBJCALL GetInsideColor() const PURE;
		EXMETHOD EXARGB EXOBJCALL GetOutsideColor() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetInsideColor(EXARGB color) PURE;
		EXMETHOD HRESULT EXOBJCALL SetOutsideColor(EXARGB color) PURE;
		EXMETHOD HRESULT EXOBJCALL GetColors(EXARGB* r_inside_color, EXARGB* r_outside_color) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetColors(EXARGB inside_color, EXARGB outside_color) PURE;

		EXMETHOD HRESULT EXOBJCALL GetBoundsRect(ExRectF* r_rect) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetBoundsRect(float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL GetCenterOffset(ExPointF* r_offset) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetCenterOffset(float horz, float vert) PURE;

		EXMETHOD uint32_t EXOBJCALL GetGradientPointCount() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetGradientPoints(const ExGradientPoint* points, uint32_t count) PURE;

		EXMETHOD bool EXOBJCALL GetGammaMode() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetGammaMode(bool gamma) PURE;
	};

	////////////////////////

	/**
	 * @brief 径向渐变画刷_创建
	 * 通过内外颜色创建径向渐变画刷
	 * @param left 边界矩形左边
	 * @param top 边界矩形顶边
	 * @param right 边界矩形右边
	 * @param bottom 边界矩形底边
	 * @param color_inside 内部颜色
	 * @param color_outside 外部颜色
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExRadialBrushCreate(float left, float top, float right, float bottom,
		EXARGB color_inside, EXARGB color_outside, IExRadialBrush** r_brush);
	
	/**
	 * @brief 径向渐变画刷_创建自渐变点
	 * 通过渐变点数组创建径向渐变画刷
	 * @param left 边界矩形左边
	 * @param top 边界矩形顶边
	 * @param right 边界矩形右边
	 * @param bottom 边界矩形底边
	 * @param gradient_points 渐变点数组
	 * @param count 渐变点数量 <需要至少两个>
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExRadialBrushCreateEx(float left, float top, float right, float bottom,
		ExGradientPoint* gradient_points, uint32_t count, IExRadialBrush** r_brush);

}
