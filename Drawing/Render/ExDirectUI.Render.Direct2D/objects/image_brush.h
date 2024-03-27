/**
 * @file image_brush.h
 * @brief 渲染器图像画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExImageBrushD2D : public ExObjectImpl<IExImageBrush>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExBrush);
		EX_INTERFACE_ENTRY(IExImageBrush);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExImageBrushD2D(image: 0x%p)", m_image.Get());
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_brush;
			case 1: return m_image;
			case 2: return (void*)&m_transform_user;
			case 3: return (void*)&m_src_rect;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExImageBrushD2D(const IExImage* image, const ExRectF* src, ExBrushExtendMode extend_mode, EXCHANNEL opacity);
		virtual ~ExImageBrushD2D();

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrixElements* r_tranform) const override;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrixElements* tranform) override;

		EXMETHOD HRESULT EXOBJCALL GetExtendMode(ExBrushExtendMode* r_mode) const override;
		EXMETHOD HRESULT EXOBJCALL SetExtendMode(ExBrushExtendMode mode) override;

		EXMETHOD HRESULT EXOBJCALL GetImage(IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL GetSourceRect(ExRectF* r_src_rect) const override;
		EXMETHOD HRESULT EXOBJCALL SetImage(IExImage* image, ExRectF* src_rect = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL TransformToRect(float left, float top, float right, float bottom) override;

		EXMETHOD EXCHANNEL EXOBJCALL GetOpacity() const override;
		EXMETHOD HRESULT EXOBJCALL SetOpacity(EXCHANNEL alpha) override;

		EXMETHOD HRESULT EXOBJCALL Flush() override;

	private:
		ExAutoPtr<ID2D1ImageBrush> m_brush;
		ExAutoPtr<ExImageD2D> m_image;
		D2D1_MATRIX_3X2_F m_transform_user = D2D1::Matrix3x2F::Identity();
		D2D1_MATRIX_3X2_F m_transform_src = D2D1::Matrix3x2F::Identity();
		D2D1_RECT_F m_src_rect{};
		EXCHANNEL m_opacity = ALPHA_OPAQUE;
		ExBrushExtendMode m_extend_mode = ExBrushExtendMode::Default;
	};

}
