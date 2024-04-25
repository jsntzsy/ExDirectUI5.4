/**
 * @file solid_brush.h
 * @brief 渲染器纯色画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExSolidBrushD2D : public ExObjectImpl<IExSolidBrush>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExBrush);
		EX_INTERFACE_ENTRY(IExSolidBrush);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			EXARGB argb = Color(m_brush->GetColor());
			return ExString::format(L"ExSolidBrushD2D(color: #%08X)", argb);
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
		ExSolidBrushD2D(EXARGB color);
		virtual ~ExSolidBrushD2D();

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrix* r_tranform) const override;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrix* tranform) override;

		EXMETHOD HRESULT EXOBJCALL GetExtendMode(ExBrushExtendMode* r_mode) const override;
		EXMETHOD HRESULT EXOBJCALL SetExtendMode(ExBrushExtendMode mode) override;

		EXMETHOD EXARGB EXOBJCALL GetColor() const override;
		EXMETHOD HRESULT EXOBJCALL SetColor(EXARGB color) override;

	private:
		ExAutoPtr<ID2D1SolidColorBrush> m_brush;
	};

}
