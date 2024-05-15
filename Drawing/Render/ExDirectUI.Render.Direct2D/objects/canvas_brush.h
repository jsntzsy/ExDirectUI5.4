/**
 * @file canvas_brush.h
 * @brief 渲染器画布画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExCanvasBrushD2D : public ExObjectImpl<IExCanvasBrush>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExBrush);
		EX_INTERFACE_ENTRY(IExCanvasBrush);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD BSTR EXOBJCALL ToString() const override
		{
			return ExSysAllocStringFormat(L"ExCanvasBrushD2D()");
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
		ExCanvasBrushD2D(const IExCanvas* canvas_src, ExBrushExtendMode extend_mode, EXCHANNEL alpha);
		ExCanvasBrushD2D(const IExCanvasTarget* target_src, ExBrushExtendMode extend_mode, EXCHANNEL alpha);
		virtual ~ExCanvasBrushD2D();

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrix* r_tranform) const override;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrix* tranform) override;

		EXMETHOD HRESULT EXOBJCALL GetExtendMode(ExBrushExtendMode* r_mode) const override;
		EXMETHOD HRESULT EXOBJCALL SetExtendMode(ExBrushExtendMode mode) override;

	private:
		ExAutoPtr<ID2D1BitmapBrush1> m_brush;
		ExBrushExtendMode m_extend_mode = ExBrushExtendMode::Default;
	};

}
