﻿/**
 * @file path.h
 * @brief 渲染器路径头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	interface IExFont;
	interface IExPen;

	/// 渲染器路径接口
	EXINTERFACE("8BE21134-BE19-4DAC-BBB3-C7084FE1647C") IExPath : public IExRenderObject
	{
		EXMETHOD HRESULT EXOBJCALL Reset() PURE;
		EXMETHOD HRESULT EXOBJCALL BeginPath(bool winding_mode) PURE;
		EXMETHOD HRESULT EXOBJCALL EndPath() PURE;

		EXMETHOD HRESULT EXOBJCALL StartFigure(float x, float y) PURE;
		EXMETHOD HRESULT EXOBJCALL FinishFigure(bool close_figure) PURE;

		EXMETHOD HRESULT EXOBJCALL GetCurPoint(ExPointF* r_point) PURE;
		EXMETHOD HRESULT EXOBJCALL MoveTo(float x, float y, bool relative = false) PURE;
		EXMETHOD HRESULT EXOBJCALL LineTo(float x, float y, bool relative = false) PURE;
		EXMETHOD HRESULT EXOBJCALL AngleArcTo(float left, float top, float right, float bottom,
			float start_angle, float sweep_angle, bool relative = false) PURE;
		EXMETHOD HRESULT EXOBJCALL ArcTo(float radius_horz, float radius_vert, float rotate,
			bool large_arc, bool clockwise, float end_x, float end_y, bool relative = false) PURE;
		EXMETHOD HRESULT EXOBJCALL RoundTo(float ctrl_x, float ctrl_y,float end_x, float end_y, float radius, bool relative = false) PURE;
		EXMETHOD HRESULT EXOBJCALL CurveTo(float ctrl_x, float ctrl_y, float end_x, float end_y, bool relative = false) PURE;
		EXMETHOD HRESULT EXOBJCALL BezierTo(float ctrl1_x, float ctrl1_y, float ctrl2_x, float ctrl2_y,
			float end_x, float end_y, bool relative = false) PURE;

		EXMETHOD HRESULT EXOBJCALL AddRect(float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL AddEllipse(float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL AddSuperEllipse(float org_x, float org_y, float radius_x, float radius_y,
			float order, float step = 1.0F) PURE;
		EXMETHOD HRESULT EXOBJCALL AddRoundRect(float left, float top, float right, float bottom, float radius) PURE;
		EXMETHOD HRESULT EXOBJCALL AddCustomRoundRect(float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom
		) PURE;
		EXMETHOD HRESULT EXOBJCALL AddPolygon(const ExPointF* points, uint32_t count, bool close_figure = true) PURE;
		EXMETHOD HRESULT EXOBJCALL AddText(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom) PURE;

		EXMETHOD HRESULT EXOBJCALL HitTest(float x, float y, const ExMatrix* tranform) const PURE;
		EXMETHOD HRESULT EXOBJCALL HitTestStroke(const IExPen* pen, float x, float y, const ExMatrix* tranform) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetBounds(ExRectF* r_rect, const ExMatrix* tranform) const PURE;

	};

	////////////////////////////

	HRESULT EXAPI EXCALL ExPathCreate(IExPath** r_path);
}
