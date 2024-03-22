/**
 * @file linear_brush.h
 * @brief 渲染器纯色画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExLinearBrushD2D : public ExObjectBaseImpl<IExLinearBrush>
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

			return ExString::format(L"ExLinearBrush: ");
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_brush;
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
		EXMETHOD HRESULT EXOBJCALL SetBeginPoint(const ExPointF* point) override;
		EXMETHOD HRESULT EXOBJCALL SetEndPoint(const ExPointF* point) override;
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

		D2D1::Matrix3x2F m_transform{};
		D2D1_POINT_2F m_begin_point{};
		D2D1_POINT_2F m_end_point{};
		D2D1_COLOR_F m_begin_color{};
		D2D1_COLOR_F m_end_color{};
		ExBrushExtendMode m_extend_mode = ExBrushExtendMode::None;
		bool m_gamma = false;
	};

}
