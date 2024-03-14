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
		const EXBITSDATA data;

		/// 保留
		const uintptr_t reserved;
	};


	EXINTERFACE("E29C3B54-282B-4EEB-8CE0-9DE6D50A015A") IExImage : public IExObject
	{
		EXMETHOD uint32_t EXOBJCALL GetWidth() PURE;
		EXMETHOD uint32_t EXOBJCALL GetHeight() PURE;

		EXMETHOD uint32_t EXOBJCALL GetFrameCount() PURE;
		EXMETHOD uint32_t EXOBJCALL GetCurFrame() PURE;
		EXSTDMETHOD SetCurFrame(uint32_t index) PURE;
		EXSTDMETHOD NextFrame(uint32_t* r_next_index = nullptr, uint32_t* r_next_delay = nullptr) PURE;

		EXSTDMETHOD GetFrameDelay(uint32_t* r_delay) PURE;
		EXSTDMETHOD Lock(ExImageLock* r_lock) PURE;
		EXSTDMETHOD Unlock(const ExImageLock* lock) PURE;

		EXSTDMETHOD GetPixel(uint32_t x, uint32_t y, uint32_t* r_color) PURE;
		EXSTDMETHOD SetPixel(uint32_t x, uint32_t y, uint32_t color) PURE;

		EXSTDMETHOD Copy(IExImage* r_dest, const ExRect* copy_rect = nullptr) PURE;
		EXSTDMETHOD Scale(uint32_t new_width, uint32_t new_height, 
			IExImage* r_dest, const ExRect* copy_rect = nullptr) PURE;
		EXSTDMETHOD Save(ExData* r_data) PURE;

	}


}
