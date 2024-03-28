/**
 * @file pen.cpp
 * @brief 渲染器画笔实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */

#include "stdafx.h"
#include "objects/pen.h"

namespace ExDirectUI
{
	ExPenD2D::ExPenD2D(EXARGB color, float width) : m_width(width)
	{
		m_color = color == COLOR_UNDEFINE ? COLOR_TRANSPARENT : color;
		
		auto render = GetRender();
		throw_if_failed(Flush(true), L"刷新画刷失败");
	}
	ExPenD2D::ExPenD2D(const IExBrush* brush, float width) : m_width(width)
	{
		m_color = COLOR_UNDEFINE;
		m_brush = const_cast<IExBrush*>(brush);
		throw_if_failed(Flush(false), L"刷新画刷失败");
	}
	ExPenD2D::~ExPenD2D()
	{
	}
	
	HRESULT EXOBJCALL ExPenD2D::GetColor(EXARGB* r_color)
	{
		CHECK_PARAM(r_color);
		*r_color = m_color;
		return S_OK;
	}
	HRESULT EXOBJCALL ExPenD2D::SetColor(EXARGB color)
	{
		if (color == COLOR_UNDEFINE) { color = COLOR_TRANSPARENT; }
		m_color = color;
		return Flush(true);
	}
	HRESULT EXOBJCALL ExPenD2D::GetBrush(IExBrush** r_brush)
	{
		CHECK_PARAM(r_brush);
		*r_brush = const_cast<IExBrush*>(m_brush.Get());
		(*r_brush)->AddRef();
		return S_OK;
	}
	HRESULT EXOBJCALL ExPenD2D::SetBrush(IExBrush* brush)
	{
		CHECK_PARAM(brush);
		m_brush = brush;
		m_color = COLOR_UNDEFINE;
		return Flush(false);
	}
	float EXOBJCALL ExPenD2D::GetStrokeWidth() const
	{
		return m_width;
	}
	HRESULT EXOBJCALL ExPenD2D::SetStrokeWidth(float stroke_width)
	{
		m_width = stroke_width;
		return S_OK;
	}
	ExDashStyle EXOBJCALL ExPenD2D::GetDashStyle() const
	{
		return m_dash_style;
	}
	HRESULT EXOBJCALL ExPenD2D::SetDashStyle(ExDashStyle dash_style)
	{
		m_dash_style = dash_style;
		return Flush(false);
	}
	ExCapStyle EXOBJCALL ExPenD2D::GetStartCap() const
	{
		return m_start_cap;
	}
	HRESULT EXOBJCALL ExPenD2D::SetStartCap(ExCapStyle start_cap)
	{
		m_start_cap = start_cap;
		return Flush(false);
	}
	ExCapStyle EXOBJCALL ExPenD2D::GetEndCap() const
	{
		return m_end_cap;
	}
	HRESULT EXOBJCALL ExPenD2D::SetEndCap(ExCapStyle end_cap)
	{
		m_end_cap = end_cap;
		return Flush(false);
	}
	ExCapStyle EXOBJCALL ExPenD2D::GetDashCap() const
	{
		return m_dash_cap;
	}
	HRESULT EXOBJCALL ExPenD2D::SetDashCap(ExCapStyle dash_cap)
	{
		m_dash_cap = dash_cap;
		return Flush(false);
	}
	ExLineJoinStyle EXOBJCALL ExPenD2D::GetLineJoin() const
	{
		return m_line_join;
	}
	HRESULT EXOBJCALL ExPenD2D::SetLineJoin(ExLineJoinStyle line_join)
	{
		m_line_join = line_join;
		return Flush(false);
	}
	float EXOBJCALL ExPenD2D::GetMiterLimit() const
	{
		return m_miter_limit;
	}
	HRESULT EXOBJCALL ExPenD2D::SetMiterLimit(float miter_limit)
	{
		m_miter_limit = miter_limit;
		return Flush(false);
	}
	uint32_t EXOBJCALL ExPenD2D::GetDashCount() const
	{
		return (uint32_t)m_dashes.size();
	}
	HRESULT EXOBJCALL ExPenD2D::GetDashArray(float* dash_array, uint32_t count) const
	{
		CHECK_PARAM(dash_array);
		CHECK_PARAM(count > 0);

		uint32_t len = (uint32_t)m_dashes.size();
		if (len > 0) {
			memcpy(dash_array, m_dashes.data(), min(len, count) * sizeof(float));
		}
		return S_OK;
	}
	HRESULT EXOBJCALL ExPenD2D::SetDashArray(const float* dash_array, uint32_t count)
	{
		m_dashes.resize(count);
		if (dash_array && count > 0) {
			memcpy(m_dashes.data(), dash_array, count * sizeof(float));
		}
		return Flush(false);
	}
	float EXOBJCALL ExPenD2D::GetDashOffset() const
	{
		return m_dash_offset;
	}
	HRESULT EXOBJCALL ExPenD2D::SetDashOffset(float offset)
	{
		m_dash_offset = offset;
		return Flush(false);
	}

	inline D2D1_DASH_STYLE ToDashStyle(ExDashStyle dash_style)
	{
		switch (dash_style)
		{
		case ExDashStyle::Solid: return D2D1_DASH_STYLE_SOLID;
		case ExDashStyle::Dash: return D2D1_DASH_STYLE_DASH;
		case ExDashStyle::Dot: return D2D1_DASH_STYLE_DOT;
		case ExDashStyle::DashDot: return D2D1_DASH_STYLE_DASH_DOT;
		case ExDashStyle::DashDotDot: return D2D1_DASH_STYLE_DASH_DOT_DOT;
		default: return D2D1_DASH_STYLE_SOLID;
		}
	}
	inline D2D1_CAP_STYLE ToCapStyle(ExCapStyle cap_style)
	{
		switch (cap_style)
		{
		case ExCapStyle::Flat: return D2D1_CAP_STYLE_FLAT;
		case ExCapStyle::Square: return D2D1_CAP_STYLE_SQUARE;
		case ExCapStyle::Round: return D2D1_CAP_STYLE_ROUND;
		case ExCapStyle::Triangle: return D2D1_CAP_STYLE_TRIANGLE;
		default: return D2D1_CAP_STYLE_FLAT;
		}
	}
	inline D2D1_LINE_JOIN ToLineJion(ExLineJoinStyle line_join)
	{
		switch (line_join)
		{
		case ExLineJoinStyle::Miter: return D2D1_LINE_JOIN_MITER;
		case ExLineJoinStyle::Bevel: return D2D1_LINE_JOIN_BEVEL;
		case ExLineJoinStyle::Round: return D2D1_LINE_JOIN_ROUND;
		default: return D2D1_LINE_JOIN_MITER;
		}
	}

	HRESULT EXOBJCALL ExPenD2D::Flush(bool recreate_brush)
	{
		auto render = GetRender();

		// 重新创建画刷
		if (recreate_brush && m_color != COLOR_UNDEFINE) {
			ExAutoPtr<IExSolidBrush> brush;
			handle_if_failed(render->CreateSolidBrush(m_color, &brush), L"创建画刷失败");
			handle_if_failed(brush->QueryInterface(-m_brush), L"查询画刷接口失败");
		}

		// 获取线段信息
		uint32_t dash_count = (uint32_t)m_dashes.size();
		float* dashes = dash_count > 0 ? m_dashes.data() : nullptr;
		D2D1_DASH_STYLE dash_style = dashes ? D2D1_DASH_STYLE_CUSTOM : ToDashStyle(m_dash_style);

		// 重新创建描边属性
		handle_if_failed(
			render->m_d2d_factory->CreateStrokeStyle(
				D2D1::StrokeStyleProperties(
					ToCapStyle(m_start_cap),
					ToCapStyle(m_end_cap),
					ToCapStyle(m_dash_cap),
					ToLineJion(m_line_join),
					m_miter_limit,
					dash_style,
					m_dash_offset
				), dashes, dash_count, -m_style
			), L"创建描边属性失败"
		);

		return S_OK;
	}
}


