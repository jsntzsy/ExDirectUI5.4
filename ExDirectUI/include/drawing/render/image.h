/**
 * @file image.h
 * @brief 渲染器图像头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 图像锁
	struct ExImageLock
	{
		/// 图像宽度
		const uint32_t width;

		/// 图像高度
		const uint32_t height;

		/// 行跨步
		const uint32_t stride;

		/// 图像像素数据
		EXBITSDATA data;

		/// 保留
		const uintptr_t reserved;

		ExImageLock() : width(), height(), stride(), data(), reserved() {}
	};

	/// 渲染器图像接口
	EXINTERFACE("E29C3B54-282B-4EEB-8CE0-9DE6D50A015A") IExImage : public IExRenderObject
	{
		EXMETHOD uint32_t EXOBJCALL GetWidth() const PURE;
		EXMETHOD uint32_t EXOBJCALL GetHeight() const PURE;

		EXMETHOD uint32_t EXOBJCALL GetFrameCount() const PURE;
		EXMETHOD uint32_t EXOBJCALL GetCurFrame() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetCurFrame(uint32_t index) PURE;
		EXMETHOD HRESULT EXOBJCALL NextFrame(uint32_t* r_next_index = nullptr, uint32_t* r_next_delay = nullptr) PURE;

		EXMETHOD uint32_t EXOBJCALL GetFrameDelay() const PURE;
		EXMETHOD HRESULT EXOBJCALL Lock(ExImageLock* r_lock, const ExRect* lock_rect) PURE;
		EXMETHOD HRESULT EXOBJCALL Unlock(ExImageLock* lock) PURE;

		EXMETHOD HRESULT EXOBJCALL GetPixel(uint32_t x, uint32_t y, uint32_t* r_color) PURE;
		EXMETHOD HRESULT EXOBJCALL SetPixel(uint32_t x, uint32_t y, uint32_t color) PURE;

		EXMETHOD HRESULT EXOBJCALL Copy(IExImage** r_dest, const ExRect* copy_rect = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL Scale(uint32_t new_width, uint32_t new_height,
			IExImage** r_dest, const ExRect* copy_rect = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL Save(ExData* r_data) PURE;
	};

	///////////////////////////

	//HRESULT EXAPI EXCALL ExImageCreateFromFile(LPCWSTR file, IExImage** r_image);
	//HRESULT EXAPI EXCALL ExImageCreateFromMemory(const byte_t* data, size_t size, IExImage** r_image);
	//HRESULT EXAPI EXCALL ExImageCreateFromBitsData(uint32_t width , uint32_t height ,
	//	const EXBITSDATA bits_data, uint32_t stride, IExImage** r_image);
	//HRESULT EXAPI EXCALL ExImageCreateFromImageData(const byte_t* data, IExImage** r_image);
	//HRESULT EXAPI EXCALL ExImageCreateFromHBITMAP(HBITMAP bitmap, HPALETTE palette, 
	//	bool pre_alpha, IExImage** r_image);
	//HRESULT EXAPI EXCALL ExImageCreateFromHICON(HICON icon, IExImage** r_image);

}
