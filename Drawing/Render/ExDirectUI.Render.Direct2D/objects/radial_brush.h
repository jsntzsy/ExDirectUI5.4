/**
 * @file radial_brush.h
 * @brief 渲染器径向渐变画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExRadialBrushD2D : public ExObjectImpl<IExRadialBrush>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExBrush);
		EX_INTERFACE_ENTRY(IExRadialBrush);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{

			return ExString::format(
				L"ExRadialBrushD2D(bounds: %f,%f,%f,%f, offset: %fx%f, "
				"inside_color: #%08X, outside_color:#%08X)",
				_expand_rect_(m_bounds), _expand_point_(m_center_offset),
				m_inside_color, m_outside_color
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
		ExRadialBrushD2D(float left, float top, float right, float bottom,
			EXARGB inside_color, EXARGB outside_color);
		ExRadialBrushD2D(float left, float top, float right, float bottom,
			const ExGradientPoint* points, uint32_t count);
		virtual ~ExRadialBrushD2D();

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrixElements* r_tranform) const override;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrixElements* tranform) override;

		EXMETHOD HRESULT EXOBJCALL GetExtendMode(ExBrushExtendMode* r_mode) const override;
		EXMETHOD HRESULT EXOBJCALL SetExtendMode(ExBrushExtendMode mode) override;

		EXMETHOD EXARGB EXOBJCALL GetInsideColor() const override;
		EXMETHOD EXARGB EXOBJCALL GetOutsideColor() const override;
		EXMETHOD HRESULT EXOBJCALL SetInsideColor(EXARGB color) override;
		EXMETHOD HRESULT EXOBJCALL SetOutsideColor(EXARGB color) override;
		EXMETHOD HRESULT EXOBJCALL GetColors(EXARGB* r_inside_color, EXARGB* r_outside_color) const override;
		EXMETHOD HRESULT EXOBJCALL SetColors(EXARGB inside_color, EXARGB outside_color) override;

		EXMETHOD HRESULT EXOBJCALL GetBoundsRect(ExRectF* r_rect) const override;
		EXMETHOD HRESULT EXOBJCALL SetBoundsRect(float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL GetCenterOffset(ExPointF* r_offset) const override;
		EXMETHOD HRESULT EXOBJCALL SetCenterOffset(float horz, float vert) override;

		EXMETHOD uint32_t EXOBJCALL GetGradientPointCount() const override;
		EXMETHOD HRESULT EXOBJCALL GetGradientPoints(ExGradientPoint* r_points, uint32_t count) const override;
		EXMETHOD HRESULT EXOBJCALL SetGradientPoints(const ExGradientPoint* points, uint32_t count) override;

		EXMETHOD bool EXOBJCALL GetGammaMode() const override;
		EXMETHOD HRESULT EXOBJCALL SetGammaMode(bool gamma) override;

	private:
		HRESULT EXOBJCALL Flush();

		ExAutoPtr<ID2D1RadialGradientBrush> m_brush;
		std::vector<ExGradientPoint> m_gradient_points;

		D2D1_MATRIX_3X2_F m_transform = D2D1::Matrix3x2F::Identity();
		ExRectF m_bounds{};
		ExPointF m_center_offset{};
		EXARGB m_inside_color = COLOR_UNDEFINE;
		EXARGB m_outside_color = COLOR_UNDEFINE;
		ExBrushExtendMode m_extend_mode = ExBrushExtendMode::None;
		bool m_gamma = false;
	};

}
