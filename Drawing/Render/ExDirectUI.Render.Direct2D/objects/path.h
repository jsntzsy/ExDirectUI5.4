/**
 * @file path.h
 * @brief 渲染器路径头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExPathD2D : public ExObjectImpl<IExPath>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExPath);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExPathD2D(geometry: 0x%p, sink: 0x%p)",
				m_geometry.Get(), m_sink.Get()
			);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_geometry;
			case 1: return m_sink;
			case 2: return (void*)&m_cur_point;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExPathD2D();
		virtual ~ExPathD2D();

		EXMETHOD HRESULT EXOBJCALL Reset() override;
		EXMETHOD HRESULT EXOBJCALL BeginPath(bool winding_mode) override;
		EXMETHOD HRESULT EXOBJCALL EndPath() override;

		EXMETHOD HRESULT EXOBJCALL StartFigure(float x, float y) override;
		EXMETHOD HRESULT EXOBJCALL FinishFigure(bool close_figure) override;

		EXMETHOD HRESULT EXOBJCALL GetCurPoint(ExPointF* r_point) override;
		EXMETHOD HRESULT EXOBJCALL MoveTo(float x, float y, bool relative) override;
		EXMETHOD HRESULT EXOBJCALL LineTo(float x, float y, bool relative) override;
		EXMETHOD HRESULT EXOBJCALL AngleArcTo(float left, float top, float right, float bottom,
			float start_angle, float sweep_angle, bool relative) override;
		EXMETHOD HRESULT EXOBJCALL ArcTo(float radius_horz, float radius_vert, float rotate,
			bool large_arc, bool clockwise, float end_x, float end_y, bool relative) override;
		EXMETHOD HRESULT EXOBJCALL RoundTo(float ctrl_x, float ctrl_y, float end_x, float end_y,
			float radius, bool relative) override;
		EXMETHOD HRESULT EXOBJCALL CurveTo(float ctrl_x, float ctrl_y, float end_x, float end_y,
			bool relative) override;
		EXMETHOD HRESULT EXOBJCALL BezierTo(float ctrl1_x, float ctrl1_y, float ctrl2_x, float ctrl2_y,
			float end_x, float end_y, bool relative) override;

		EXMETHOD HRESULT EXOBJCALL AddRect(float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL AddEllipse(float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL AddSuperEllipse(float org_x, float org_y, float radius_x, float radius_y,
			float order, float step = 1.0F) override;
		EXMETHOD HRESULT EXOBJCALL AddRoundRect(float left, float top, float right, float bottom, float radius) override;
		EXMETHOD HRESULT EXOBJCALL AddCustomRoundRect(float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom
		) override;
		EXMETHOD HRESULT EXOBJCALL AddPolygon(const ExPointF* points, uint32_t count, bool close_figure) override;
		EXMETHOD HRESULT EXOBJCALL AddText(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom) override;

		EXMETHOD HRESULT EXOBJCALL HitTest(float x, float y, const ExMatrixElements* tranform) const override;
		EXMETHOD HRESULT EXOBJCALL HitTestStroke(const IExPen* pen, float x, float y, const ExMatrixElements* tranform) const override;
		EXMETHOD HRESULT EXOBJCALL GetBounds(ExRectF* r_rect, const ExMatrixElements* tranform) const override;

	private:
		static void MakeRoundRectFigure(ID2D1GeometrySink* sink, float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom);

		friend class ExRegionD2D;
		friend class ExCanvasD2D;

	private:
		ExAutoPtr<ID2D1PathGeometry> m_geometry{};
		ExAutoPtr<ID2D1GeometrySink> m_sink{};
		D2D1_POINT_2F m_cur_point{};
		bool m_figure_started{ false };
	};
}
