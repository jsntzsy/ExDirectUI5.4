/**
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
		EXMETHOD HRESULT EXOBJCALL MoveTo(float x, float y) PURE;
		EXMETHOD HRESULT EXOBJCALL LineTo(float x, float y) PURE;
		EXMETHOD HRESULT EXOBJCALL ArcTo(float left, float top, float right, float bottom,
			float start_angle, float sweep_angle) PURE;
		EXMETHOD HRESULT EXOBJCALL ArcTo2(float x_end,float y_end, float radius_horz, float radius_vert,
			float rotate, bool clockwise, bool large_arc) PURE;
		EXMETHOD HRESULT EXOBJCALL RoundTo(float x_ctrl, float y_ctrl,float x_end, float y_end, float radius) PURE;
		EXMETHOD HRESULT EXOBJCALL CurveTo(float x_ctrl, float y_ctrl, float x_end, float y_end) PURE;
		EXMETHOD HRESULT EXOBJCALL BezierTo(float x_ctrl1, float y_ctrl1, float x_ctrl2, float y_ctrl2,
			float x_end, float y_end) PURE;

		EXMETHOD HRESULT EXOBJCALL AddRect(float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL AddEllipse(float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL AddRoundRect(float left, float top, float right, float bottom,
			float radius_x, float radius_y) PURE;
		EXMETHOD HRESULT EXOBJCALL AddCustomRoundRect(float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom
		) PURE;
		EXMETHOD HRESULT EXOBJCALL AddPolygon(const ExPointF* points, uint32_t count) PURE;
		EXMETHOD HRESULT EXOBJCALL AddText(LPCWSTR text, int len, const IExFont* font, DWORD text_format,
			float left, float top, float right, float bottom) PURE;

		EXMETHOD HRESULT EXOBJCALL HitTest(float x, float y, const ExMatrixElements* tranform) const PURE;
		EXMETHOD HRESULT EXOBJCALL HitTestStroke(const IExPen* pen, float x, float y, const ExMatrixElements* tranform) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetBounds(ExRectF* r_rect, const ExMatrixElements* tranform) const PURE;

	};

	////////////////////////////

	//HRESULT EXAPI EXCALL ExPathCreate(IExPath** r_path);
}
