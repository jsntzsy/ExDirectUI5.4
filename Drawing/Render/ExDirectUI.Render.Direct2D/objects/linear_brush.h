/**
 * @file linear_brush.h
 * @brief 渲染器线性渐变画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExLinearBrushD2D : public ExObjectImpl<IExLinearBrush>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExBrush);
		EX_INTERFACE_ENTRY(IExLinearBrush);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{

			return ExString::format(L"ExLinearBrushD2D(begin: %f,%f/#08X end: %f,%f/#08X)",
				m_begin_point.x, m_begin_point.y, m_begin_color,
				m_end_point.x, m_end_point.y, m_end_color
			);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_brush;
			case 1: return (void*)m_gradient_points.data();
			case 2: return (void*)&m_transform;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExLinearBrushD2D(float begin_x, float begin_y, float end_x, float end_y,
			EXARGB begin_color, EXARGB end_color);
		ExLinearBrushD2D(float begin_x, float begin_y, float end_x, float end_y,
			const ExGradientPoint* points, uint32_t count);
		virtual ~ExLinearBrushD2D();

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrixElements* r_tranform) const override;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrixElements* tranform) override;

		EXMETHOD HRESULT EXOBJCALL GetExtendMode(ExBrushExtendMode* r_mode) const override;
		EXMETHOD HRESULT EXOBJCALL SetExtendMode(ExBrushExtendMode mode) override;

		EXMETHOD EXARGB EXOBJCALL GetBeginColor() const override;
		EXMETHOD EXARGB EXOBJCALL GetEndColor() const override;
		EXMETHOD HRESULT EXOBJCALL SetBeginColor(EXARGB color) override;
		EXMETHOD HRESULT EXOBJCALL SetEndColor(EXARGB color) override;
		EXMETHOD HRESULT EXOBJCALL GetColors(EXARGB* r_begin_color, EXARGB* r_end_color) const override;
		EXMETHOD HRESULT EXOBJCALL SetColors(EXARGB begin_color, EXARGB end_color) override;

		EXMETHOD HRESULT EXOBJCALL GetBeginPoint(ExPointF* r_point) const override;
		EXMETHOD HRESULT EXOBJCALL GetEndPoint(ExPointF* r_point) const override;
		EXMETHOD HRESULT EXOBJCALL GetPoints(ExPointF* r_begin_point, ExPointF* r_end_point) const override;
		EXMETHOD HRESULT EXOBJCALL SetBeginPoint(float x, float y) override;
		EXMETHOD HRESULT EXOBJCALL SetEndPoint(float x, float y) override;
		EXMETHOD HRESULT EXOBJCALL SetPoints(const ExPointF* begin_point, const ExPointF* end_point) override;

		EXMETHOD uint32_t EXOBJCALL GetGradientPointCount() const override;
		EXMETHOD HRESULT EXOBJCALL GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const override;
		EXMETHOD HRESULT EXOBJCALL SetGradientPoints(const ExGradientPoint* points, uint32_t count) override;

		EXMETHOD bool EXOBJCALL GetGammaMode() const override;
		EXMETHOD HRESULT EXOBJCALL SetGammaMode(bool gamma) override;

	private:
		HRESULT EXOBJCALL Flush();


		ExAutoPtr<ID2D1LinearGradientBrush> m_brush;
		std::vector<ExGradientPoint> m_gradient_points;

		D2D1_MATRIX_3X2_F m_transform{};
		D2D1_POINT_2F m_begin_point{};
		D2D1_POINT_2F m_end_point{};
		EXARGB m_begin_color = COLOR_UNDEFINE;
		EXARGB m_end_color = COLOR_UNDEFINE;
		ExBrushExtendMode m_extend_mode = ExBrushExtendMode::None;
		bool m_gamma = false;
	};

}
