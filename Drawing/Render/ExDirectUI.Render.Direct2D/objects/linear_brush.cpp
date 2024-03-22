/**
 * @file l_brush.cpp
 * @brief 渲染器纯色画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */

#include "stdafx.h"
#include "objects/linear_brush.h"

namespace ExDirectUI
{
	ExLinearBrushD2D::ExLinearBrushD2D(float begin_x, float begin_y, float end_x, float end_y,
		EXARGB begin_color, EXARGB end_color)
	{
		m_begin_color = Color(begin_color);
		m_end_color = Color(end_color);
		m_begin_point = D2D1::Point2F(begin_x, begin_y);
		m_end_point = D2D1::Point2F(end_x, end_y);

		throw_if_failed(Flush(), L"创建线性画刷失败");
	}
	ExLinearBrushD2D::ExLinearBrushD2D(float begin_x, float begin_y, float end_x, float end_y,
		const ExGradientPoint* points, uint32_t count)
	{
		m_begin_point = D2D1::Point2F(begin_x, begin_y);
		m_end_point = D2D1::Point2F(end_x, end_y);
		throw_if_failed(SetGradientPoints(points, count), L"创建线性画刷失败");
	}
	ExLinearBrushD2D::~ExLinearBrushD2D()
	{
	}
	HRESULT EXOBJCALL ExLinearBrushD2D::GetTransform(ExMatrixElements* r_tranform) const
	{
		CHECK_PARAM(r_tranform);
		*r_tranform = Matrix(m_transform);
		return S_OK;
	}
	HRESULT EXOBJCALL ExLinearBrushD2D::SetTransform(const ExMatrixElements* tranform)
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
	HRESULT EXOBJCALL ExLinearBrushD2D::GetExtendMode(ExBrushExtendMode* r_mode) const
	{
		CHECK_PARAM(r_mode);
		*r_mode = m_extend_mode;
		return S_OK;
	}
	HRESULT EXOBJCALL ExLinearBrushD2D::SetExtendMode(ExBrushExtendMode mode)
	{
		if (m_extend_mode == mode) { return S_OK; }
		m_extend_mode = mode;
		return Flush();
	}

	EXARGB EXOBJCALL ExLinearBrushD2D::GetBeginColor() const
	{
		return Color(m_begin_color);
	}

	EXARGB EXOBJCALL ExLinearBrushD2D::GetEndColor() const
	{
		return Color(m_end_color);
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetBeginColor(EXARGB color)
	{
		m_begin_color = Color(color);
		return Flush();
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetEndColor(EXARGB color)
	{
		m_end_color = Color(color);
		return Flush();
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::GetColors(EXARGB* r_begin_color, EXARGB* r_end_color) const
	{
		if (r_begin_color) { *r_begin_color = Color(m_begin_color); }
		if (r_end_color) { *r_end_color = Color(m_end_color); }
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetColors(EXARGB begin_color, EXARGB end_color)
	{
		if (begin_color != COLOR_UNDEFINE) { m_begin_color = Color(begin_color); }
		if (end_color != COLOR_UNDEFINE) { m_end_color = Color(end_color); }
		return Flush();
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::GetBeginPoint(ExPointF* r_point) const
	{
		CHECK_PARAM(r_point);
		r_point->x = m_begin_point.x;
		r_point->y = m_begin_point.y;
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::GetEndPoint(ExPointF* r_point) const
	{
		CHECK_PARAM(r_point);
		r_point->x = m_end_point.x;
		r_point->y = m_end_point.y;
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::GetPoints(ExPointF* r_begin_point, ExPointF* r_end_point) const
	{
		CHECK_PARAM(r_begin_point && r_end_point);
		if (r_begin_point) { *r_begin_point = ExPointF(m_begin_point.x, m_begin_point.y); }
		if (r_end_point) { *r_end_point = ExPointF(m_end_point.x, m_end_point.y); }
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetBeginPoint(const ExPointF* point)
	{
		CHECK_PARAM(point);
		m_begin_point = D2D1::Point2F(point->x, point->y);
		m_brush->SetStartPoint(m_begin_point);
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetEndPoint(const ExPointF* point)
	{
		CHECK_PARAM(point);
		m_end_point = D2D1::Point2F(point->x, point->y);
		m_brush->SetEndPoint(m_end_point);
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetPoints(const ExPointF* begin_point, const ExPointF* end_point)
	{
		if (begin_point) {
			m_begin_point = D2D1::Point2F(begin_point->x, begin_point->y);
			m_brush->SetStartPoint(m_begin_point);
		}
		if (end_point) {
			m_end_point = D2D1::Point2F(end_point->x, end_point->y);
			m_brush->SetEndPoint(m_end_point);
		}
		return S_OK;
	}

	uint32_t EXOBJCALL ExLinearBrushD2D::GetGradientPointCount() const
	{
		return m_gradient_points.size();
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const
	{
		CHECK_PARAM(r_points);

		uint32_t c = min(m_gradient_points.size(), count);
		if (c == 0) { return S_FALSE; }

		memcpy(r_points, m_gradient_points.data(), c * sizeof(ExGradientPoint));
		return S_OK;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetGradientPoints(const ExGradientPoint* points, uint32_t count)
	{
		CHECK_PARAM(points && count >= 2);

		m_gradient_points.resize(count);
		memcpy(m_gradient_points.data(), points, count * sizeof(ExGradientPoint));

		return Flush();
	}

	bool EXOBJCALL ExLinearBrushD2D::GetGammaMode() const
	{
		return m_gamma;
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::SetGammaMode(bool gamma)
	{
		if (m_gamma == gamma) { return S_OK; }
		m_gamma = gamma;
		return Flush();
	}

	HRESULT EXOBJCALL ExLinearBrushD2D::Flush()
	{
		try
		{
			auto render = GetRender();

			//获取扩展模式
			D2D1_EXTEND_MODE extend_mode = D2D1_EXTEND_MODE_CLAMP;
			switch (m_extend_mode)
			{
			case ExBrushExtendMode::None: extend_mode = D2D1_EXTEND_MODE_CLAMP; break;
			case ExBrushExtendMode::Tile: extend_mode = D2D1_EXTEND_MODE_WRAP; break;
			case ExBrushExtendMode::Mirror: extend_mode = D2D1_EXTEND_MODE_MIRROR; break;
			}

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
				gradient_stops[0].color = m_begin_color;
				gradient_stops[0].position = 0.0f;
				gradient_stops[1].color = m_end_color;
				gradient_stops[1].position = 1.0f;
			}

			//生成渐变点对象
			ExAutoPtr<ID2D1GradientStopCollection> stops;
			throw_if_failed(
				render->m_d2d_dc->CreateGradientStopCollection(
					gradient_stops.data(), gradient_stops.size(),
					m_gamma ? D2D1_GAMMA_2_2 : D2D1_GAMMA_1_0, extend_mode,
					&stops
				), L"创建渐变点集失败"
			);

			//创建线性渐变画刷
			ExAutoPtr<ID2D1LinearGradientBrush> brush;
			throw_if_failed(
				render->m_d2d_dc->CreateLinearGradientBrush(
					D2D1::LinearGradientBrushProperties(m_begin_point, m_end_point),
					stops, &brush
				), L"创建线性画刷失败"
			);

			//应用矩阵变换
			if(!IsIdentityMatrix(m_transform)) { 
				brush->SetTransform(m_transform); 
			}

			//替换成员变量
			m_brush = brush;
			return S_OK;
		}
		catch_default({});
	}

}


