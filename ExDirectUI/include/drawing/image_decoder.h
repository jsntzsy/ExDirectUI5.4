/**
 * @file image_decoder.h
 * @brief 图像解码器头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	EXINTERFACE("4B122D81-18EB-4702-9417-AA2C95BB17CF") IExDecodeImage : public IUnknown
	{
		EXMETHOD uint32_t EXOBJCALL GetWidth() PURE;
		EXMETHOD uint32_t EXOBJCALL GetHeight() PURE;
		EXMETHOD uint32_t EXOBJCALL GetFrameCount() PURE;

		EXSTDMETHOD GetFrameDelay(uint32_t index, uint32_t* r_delay) PURE;
		EXSTDMETHOD CopyFrame(uint32_t index, EXBITSDATA r_bits,uint32_t stride = 0) PURE;
	};

	EXINTERFACE("ADBE9236-4ECF-46C8-9B9E-98E4A1A67885") IExImageDecoder : public IExObject
	{
		EXSTDMETHOD LoadImageFromFile(LPCWSTR file, IExDecodeImage * *r_image) PURE;
		EXSTDMETHOD LoadImageFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) PURE;
	};

}
