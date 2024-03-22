/**
 * @file radial_brush.cpp
 * @brief 渲染器径向渐变画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */

#include "stdafx.h"
#include "objects/radial_brush.h"

namespace ExDirectUI
{
	ExRadialBrushD2D::ExRadialBrushD2D(float left, float top, float right, float bottom,
		EXARGB inside_color, EXARGB outside_color)
	{
		m_inside_color = inside_color;
		m_outside_color = outside_color;
		m_bounds = ExRect(left, top, right, bottom).Normalize();

		throw_if_failed(Flush(), L"创建径向渐变画刷失败");
	}
	ExRadialBrushD2D::ExRadialBrushD2D(float left, float top, float right, float bottom,
		const ExGradientPoint* points, uint32_t count)
	{
		m_inside_color = COLOR_UNDEFINE;
		m_outside_color = COLOR_UNDEFINE;
		m_bounds = ExRect(left, top, right, bottom).Normalize();
		throw_if_failed(SetGradientPoints(points, count), L"创建径向渐变画刷失败");
	}
	ExRadialBrushD2D::~ExRadialBrushD2D()
	{
	}
	HRESULT EXOBJCALL ExRadialBrushD2D::GetTransform(ExMatrixElements* r_tranform) const
	{
		CHECK_PARAM(r_tranform);
		*r_tranform = Matrix(m_transform);
		return S_OK;
	}
	HRESULT EXOBJCALL ExRadialBrushD2D::SetTransform(const ExMatrixElements* tranform)
	{
		CHECK_PARAM(tranform);

		if (tranform) {
			m_transform = D2D1::Matrix3x2F(
				tranform->_11, tranform->_12,
				tranform->_21, tranform->_22,
				tranform->_31, tranform->_32
			);
		}
		else { m_transform = D2D1::Matrix3x2F::Identity(); }

		m_brush->SetTransform(m_transform);
		return S_OK;
	}
	HRESULT EXOBJCALL ExRadialBrushD2D::GetExtendMode(ExBrushExtendMode* r_mode) const
	{
		CHECK_PARAM(r_mode);
		*r_mode = m_extend_mode;
		return S_OK;
	}
	HRESULT EXOBJCALL ExRadialBrushD2D::SetExtendMode(ExBrushExtendMode mode)
	{
		if (m_extend_mode == mode) { return S_FALSE; }

		m_extend_mode = mode;
		return Flush();
	}

	EXARGB EXOBJCALL ExRadialBrushD2D::GetInsideColor() const
	{
		return m_inside_color;
	}

	EXARGB EXOBJCALL ExRadialBrushD2D::GetOutsideColor() const
	{
		return m_outside_color;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetInsideColor(EXARGB color)
	{
		if (m_inside_color == color) { return S_FALSE; }

		m_inside_color = color;
		return Flush();
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetOutsideColor(EXARGB color)
	{
		if (m_outside_color == color) { return S_FALSE; }

		m_outside_color = color;
		return Flush();
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::GetColors(EXARGB* r_inside_color, EXARGB* r_outside_color) const
	{
		if (r_inside_color) { *r_inside_color = m_inside_color; }
		if (r_outside_color) { *r_outside_color = m_outside_color; }
		return S_OK;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetColors(EXARGB inside_color, EXARGB outside_color)
	{
		bool changed = false;
		if (inside_color != COLOR_UNDEFINE && inside_color != m_inside_color)
		{
			m_inside_color = inside_color;
			changed = true;
		}
		if (outside_color != COLOR_UNDEFINE && outside_color != m_outside_color)
		{
			m_outside_color = outside_color;
			changed = true;
		}
		return changed ? Flush() : S_FALSE;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::GetBoundsRect(ExRectF* r_rect) const
	{
		CHECK_PARAM(r_rect);
		CopyMemory(r_rect, &m_bounds, sizeof(ExRectF));
		return S_OK;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetBoundsRect(float left, float top, float right, float bottom)
	{
		m_bounds = ExRectF(left, top, right, bottom).Normalize();
		m_brush->SetRadiusX(m_bounds.Width() / 2);
		m_brush->SetRadiusY(m_bounds.Height() / 2);
		m_brush->SetCenter(D2D1::Point2F(
			(m_bounds.left + m_bounds.right),
			(m_bounds.top + m_bounds.bottom)
		));
		return S_OK;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::GetCenterOffset(ExPointF* r_offset) const
	{
		CHECK_PARAM(r_offset);
		r_offset->x = m_center_offset.x;
		r_offset->y = m_center_offset.y;
		return S_OK;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetCenterOffset(float horz, float vert)
	{
		m_brush->SetGradientOriginOffset(D2D1::Point2F(horz, vert));
		return S_OK;
	}

	uint32_t EXOBJCALL ExRadialBrushD2D::GetGradientPointCount() const
	{
		return (uint32_t)m_gradient_points.size();
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const
	{
		CHECK_PARAM(r_points);

		uint32_t c = (uint32_t)m_gradient_points.size();
		c = min(c, count);
		if (c == 0) { return S_FALSE; }

		memcpy(r_points, m_gradient_points.data(), c * sizeof(ExGradientPoint));
		return S_OK;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetGradientPoints(const ExGradientPoint* points, uint32_t count)
	{
		CHECK_PARAM(points && count >= 2);

		m_gradient_points.resize(count);
		memcpy(m_gradient_points.data(), points, count * sizeof(ExGradientPoint));

		return Flush();
	}

	bool EXOBJCALL ExRadialBrushD2D::GetGammaMode() const
	{
		return m_gamma;
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::SetGammaMode(bool gamma)
	{
		if (m_gamma == gamma) { return S_FALSE; }

		m_gamma = gamma;
		return Flush();
	}

	HRESULT EXOBJCALL ExRadialBrushD2D::Flush()
	{
		try
		{
			auto render = GetRender();

			//获取扩展模式
			D2D1_EXTEND_MODE extend_mode = GetD2DExtendMode(m_extend_mode);

			//生成渐变点数组
			std::vector<D2D1_GRADIENT_STOP> gradient_stops;
			if (m_gradient_points.size() >= 2) {
				gradient_stops.resize(m_gradient_points.size());
				for (size_t i = 0; i < m_gradient_points.size(); i++) {
					gradient_stops[i].color = Color(m_gradient_points[i].color);
					gradient_stops[i].position = m_gradient_points[i].pos;
				}
			}
			else {
				gradient_stops.resize(2);
				gradient_stops[0].color = Color(m_inside_color);
				gradient_stops[0].position = 0.0f;
				gradient_stops[1].color = Color(m_outside_color);
				gradient_stops[1].position = 1.0f;
			}

			//生成渐变点对象
			ExAutoPtr<ID2D1GradientStopCollection> stops;
			throw_if_failed(
				render->m_d2d_dc->CreateGradientStopCollection(
					gradient_stops.data(), (uint32_t)gradient_stops.size(),
					m_gamma ? D2D1_GAMMA_2_2 : D2D1_GAMMA_1_0, extend_mode,
					&stops
				), L"创建渐变点集失败"
			);

			//创建径向渐变画刷
			ExAutoPtr<ID2D1RadialGradientBrush> brush;
			throw_if_failed(
				render->m_d2d_dc->CreateRadialGradientBrush(
					D2D1::RadialGradientBrushProperties(
						D2D1::Point2F((
							m_bounds.left + m_bounds.right) / 2,
							(m_bounds.top + m_bounds.bottom) / 2
						), D2D1::Point2F(m_center_offset.x, m_center_offset.y),
						m_bounds.Width() / 2, m_bounds.Height() / 2
					), stops, &brush
				), L"创建径向渐变画刷失败"
			);

			//应用矩阵变换
			if (!IsIdentityMatrix(m_transform)) {
				brush->SetTransform(m_transform);
			}

			//替换成员变量
			m_brush = brush;
			return S_OK;
		}
		catch_default({});
	}

}


