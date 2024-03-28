/**
 * @file pen.h
 * @brief 渲染器画笔头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExPenD2D : public ExObjectImpl<IExPen>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExPen);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExPenD2D(width: %f, dash_style: 0x%04X)",
				m_width, m_dash_style
			);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_brush;
			case 1: return m_style;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExPenD2D(EXARGB color, float width);
		ExPenD2D(const IExBrush* brush, float width);
		virtual ~ExPenD2D();

		EXMETHOD HRESULT EXOBJCALL GetColor(EXARGB* r_color) override;
		EXMETHOD HRESULT EXOBJCALL SetColor(EXARGB color) override;

		EXMETHOD HRESULT EXOBJCALL GetBrush(IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL SetBrush(IExBrush* brush) override;

		EXMETHOD float EXOBJCALL GetStrokeWidth() const override;
		EXMETHOD HRESULT EXOBJCALL SetStrokeWidth(float stroke_width) override;

		EXMETHOD ExDashStyle EXOBJCALL GetDashStyle() const override;
		EXMETHOD HRESULT EXOBJCALL SetDashStyle(ExDashStyle dash_style) override;

		EXMETHOD ExCapStyle EXOBJCALL GetStartCap() const override;
		EXMETHOD HRESULT EXOBJCALL SetStartCap(ExCapStyle start_cap) override;

		EXMETHOD ExCapStyle EXOBJCALL GetEndCap() const override;
		EXMETHOD HRESULT EXOBJCALL SetEndCap(ExCapStyle end_cap) override;

		EXMETHOD ExCapStyle EXOBJCALL GetDashCap() const override;
		EXMETHOD HRESULT EXOBJCALL SetDashCap(ExCapStyle dash_cap) override;

		EXMETHOD ExLineJoinStyle EXOBJCALL GetLineJoin() const override;
		EXMETHOD HRESULT EXOBJCALL SetLineJoin(ExLineJoinStyle line_join) override;

		EXMETHOD float EXOBJCALL GetMiterLimit() const override;
		EXMETHOD HRESULT EXOBJCALL SetMiterLimit(float miter_limit) override;

		EXMETHOD uint32_t EXOBJCALL GetDashCount() const override;
		EXMETHOD HRESULT EXOBJCALL GetDashArray(float* dash_array, uint32_t count) const override;
		EXMETHOD HRESULT EXOBJCALL SetDashArray(const float* dash_array, uint32_t count) override;

		EXMETHOD float EXOBJCALL GetDashOffset() const override;
		EXMETHOD HRESULT EXOBJCALL SetDashOffset(float offset) override;
		
	private:
		HRESULT EXOBJCALL Flush(bool recreate_bursh);

		EXARGB m_color = COLOR_UNDEFINE;
		float m_width = 1.0f;
		ExDashStyle m_dash_style = ExDashStyle::Solid;
		ExCapStyle m_start_cap = ExCapStyle::Flat;
		ExCapStyle m_end_cap = ExCapStyle::Flat;
		ExCapStyle m_dash_cap = ExCapStyle::Flat;
		ExLineJoinStyle m_line_join = ExLineJoinStyle::Miter;
		float m_miter_limit = 10.0f;
		float m_dash_offset = 0.0f;
		std::vector<float> m_dashes{};
		ExAutoPtr<IExBrush> m_brush{};
		ExAutoPtr<ID2D1StrokeStyle> m_style{};

		friend class ExCanvasD2D;
		friend class ExTextRenderD2D;
		friend class ExPathD2D;
	};

}
