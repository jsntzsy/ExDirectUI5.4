/**
 * @file decode_image.h
 * @brief 解码图像头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once
#include "drawing/image_decoder.h"
#include "common/unknown_impl.hpp"

namespace ExDirectUI
{
	struct ExDecodeImageContextLibPng
	{
		ExAPngImageInfo* image;
	};

	class ExDecodeImageLibPng : public ExUnknownImpl<IExDecodeImage>
	{
	public:
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IExDecodeImage);
		EX_DECLEAR_INTERFACE_END();

	public:
		ExDecodeImageLibPng(ExDecodeImageContextLibPng* context);
		virtual ~ExDecodeImageLibPng();

		EXMETHOD uint32_t EXOBJCALL GetWidth() override;
		EXMETHOD uint32_t EXOBJCALL GetHeight() override;
		EXMETHOD uint32_t EXOBJCALL GetFrameCount() override;

		EXMETHOD HRESULT EXOBJCALL GetFrameDelay(uint32_t index, uint32_t* r_delay) override;
		EXMETHOD HRESULT EXOBJCALL CopyFrame(uint32_t index, EXBITSDATA r_bits, uint32_t stride = 0) override;

	private:
		ExDecodeImageContextLibPng* m_context{};
	};


}
