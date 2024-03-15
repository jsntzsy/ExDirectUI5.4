/**
 * @file pen.h
 * @brief 渲染器画笔头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 虚线样式
	enum ExDashStyle
	{
		EX_DASH_STYLE_SOLID = 0,		///< 虚线样式：实线
		EX_DASH_STYLE_DASH = 1,			///< 虚线样式：划线
		EX_DASH_STYLE_DOT = 2,			///< 虚线样式：点线
		EX_DASH_STYLE_DASHDOT = 3,		///< 虚线样式：划点线
		EX_DASH_STYLE_DASHDOTDOT = 4,	///< 虚线样式：划点点线
	};

	/// 笔帽样式
	enum ExCapStyle
	{
		EX_CAP_STYLE_FLAT = 0,			///< 笔帽样式：平头
		EX_CAP_STYLE_ROUND = 1,			///< 笔帽样式：圆头
		EX_CAP_STYLE_SQUARE = 2,		///< 笔帽样式：方头
		EX_CAP_STYLE_TRIANGLE = 3,		///< 笔帽样式：三角头
	};

	/// 笔段连接样式
	enum ExLineJoinStyle
	{
		EX_JOIN_STYLE_MITER = 0,		///< 笔段连接样式：斜接
		EX_JOIN_STYLE_ROUND = 1,		///< 笔段连接样式：圆接
		EX_JOIN_STYLE_BEVEL = 2,		///< 笔段连接样式：斜角
	};

	interface IExBrush;

	EXINTERFACE("A100F4C6-55BC-4F59-993E-B20FE7C51E48") IExPen : public IExRenderObject
	{
		EXMETHOD HRESULT EXOBJCALL GetColor(EXARGB * r_color) PURE;
		EXMETHOD HRESULT EXOBJCALL SetColor(EXARGB color) PURE;

		EXMETHOD HRESULT EXOBJCALL GetBrush(IExBrush* r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL SetBrush(IExBrush* brush) PURE;

		EXMETHOD float EXOBJCALL GetStrokeWidth() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetStrokeWidth(float stroke_width) PURE;

		EXMETHOD ExDashStyle EXOBJCALL GetDashStyle() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetDashStyle(ExDashStyle dash_style) PURE;

		EXMETHOD ExCapStyle EXOBJCALL GetStartCap() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetStartCap(ExCapStyle start_cap) PURE;

		EXMETHOD ExCapStyle EXOBJCALL GetEndCap() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetEndCap(ExCapStyle end_cap) PURE;

		EXMETHOD ExCapStyle EXOBJCALL GetDashCap() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetDashCap(ExCapStyle dash_cap) PURE;

		EXMETHOD ExLineJoinStyle EXOBJCALL GetLineJoin() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetLineJoin(ExLineJoinStyle line_join) PURE;

		EXMETHOD float EXOBJCALL GetMiterLimit() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetMiterLimit(float miter_limit) PURE;

		EXMETHOD uint32_t EXOBJCALL GetDashCount() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetDashArray(float* dash_array, uint32_t count) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetDashArray(const float* dash_array, uint32_t count) PURE;

	};

	///////////////////

	//HRESULT EXAPI EXCALL ExPenCreate(EXARGB color, float stroke_width, IExPen** r_pen);
	//HRESULT EXAPI EXCALL ExPenCreateEx(EXARGB color, float stroke_width,
	//	ExDashStyle dash_style, ExCapStyle start_cap, ExCapStyle end_cap, ExCapStyle dash_cap,
	//	ExLineJoinStyle line_join, float miter_limit, const float* dash_array, uint32_t dash_count,
	//	IExPen** r_pen
	//);
	//HRESULT EXAPI EXCALL ExPenCreateFromBrush(IExBrush* brush, float stroke_width, IExPen** r_pen);
	//HRESULT EXAPI EXCALL ExPenCreateFromBrushEx(IExBrush* brush, float stroke_width,
	//	ExDashStyle dash_style, ExCapStyle start_cap, ExCapStyle end_cap, ExCapStyle dash_cap,
	//	ExLineJoinStyle line_join, float miter_limit, const float* dash_array, uint32_t dash_count,
	//	IExPen** r_pen
	//);

}
