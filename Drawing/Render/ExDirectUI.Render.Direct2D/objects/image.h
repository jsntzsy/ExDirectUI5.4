/**
 * @file image.h
 * @brief 渲染器图像对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

namespace ExDirectUI
{

	class ExImageD2D : public ExObjectBaseImpl<IExImage>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExImage);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExImageD2D: size: %ux%u frame: %u/%u",
				m_width, m_height, m_cur_frame, GetFrameCount()
			);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_d2d_bitmap;
			case 1: return m_wic_bitmap;
			case 2: return m_decode_image;
			case 3: return m_bitmap_lock;

			default: return __super::GetContext(index);
			}
		}

		ExImageD2D(uint32_t width, uint32_t height);
		ExImageD2D(LPCWSTR file);
		ExImageD2D(const byte_t* data, size_t size);
		ExImageD2D(uint32_t width, uint32_t height, const EXBITSDATA bits_data, uint32_t stride);
		ExImageD2D(const byte_t* image_data);
		ExImageD2D(HBITMAP bitmap, HPALETTE palette, bool pre_alpha);
		ExImageD2D(HICON icon);
		ExImageD2D(IWICBitmap* src_bitmap);

		virtual ~ExImageD2D();


		EXMETHOD uint32_t EXOBJCALL GetWidth() const override;
		EXMETHOD uint32_t EXOBJCALL GetHeight() const override;

		EXMETHOD uint32_t EXOBJCALL GetFrameCount() const override;
		EXMETHOD uint32_t EXOBJCALL GetCurFrame() const override;
		EXMETHOD HRESULT EXOBJCALL SetCurFrame(uint32_t index) override;
		EXMETHOD HRESULT EXOBJCALL NextFrame(uint32_t* r_next_index = nullptr, uint32_t* r_next_delay = nullptr) override;

		EXMETHOD uint32_t EXOBJCALL GetFrameDelay() const override;
		EXMETHOD HRESULT EXOBJCALL Lock(ExImageLock* r_lock, const ExRect* lock_rect) override;
		EXMETHOD HRESULT EXOBJCALL Unlock(ExImageLock* lock) override;

		EXMETHOD HRESULT EXOBJCALL GetPixel(uint32_t x, uint32_t y, uint32_t* r_color) override;
		EXMETHOD HRESULT EXOBJCALL SetPixel(uint32_t x, uint32_t y, uint32_t color) override;

		EXMETHOD HRESULT EXOBJCALL Copy(IExImage** r_dest, const ExRect* copy_rect = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL Scale(uint32_t new_width, uint32_t new_height,
			IExImage** r_dest, const ExRect* copy_rect = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL Save(ExData* r_data) override;

	private:
		HRESULT EXOBJCALL Flush();

	private:
		uint32_t m_width{};
		uint32_t m_height{};
		uint32_t m_cur_frame{};
		ExAutoPtr<IExDecodeImage> m_decode_image{};
		ExAutoPtr<ID2D1Bitmap> m_d2d_bitmap{};
		ExAutoPtr<IWICBitmap> m_wic_bitmap{};
		ExAutoPtr<IWICBitmapLock> m_bitmap_lock{};
	};
}
