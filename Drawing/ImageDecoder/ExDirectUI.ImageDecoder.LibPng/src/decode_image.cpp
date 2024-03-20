/**
 * @file decode_image.cpp
 * @brief 解码图像实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "stdafx.h"
#include "decode_image.h"

namespace ExDirectUI
{
	ExDecodeImageLibPng::ExDecodeImageLibPng(ExDecodeImageContextLibPng* context)
		:m_context(context)
	{}

	ExDecodeImageLibPng::~ExDecodeImageLibPng()
	{
		if (m_context) {
			ExImageDecoderLibPng::Instance()->FreeImage(m_context);
		}
	}

	uint32_t ExDecodeImageLibPng::GetWidth() noexcept
	{
		return m_context->image->width;
	}

	uint32_t EXOBJCALL ExDecodeImageLibPng::GetHeight()
	{
		return m_context->image->height;
	}

	uint32_t EXOBJCALL ExDecodeImageLibPng::GetFrameCount()
	{
		return m_context->image->frame_count;
	}

	HRESULT EXOBJCALL ExDecodeImageLibPng::GetFrameDelay(uint32_t index, uint32_t* r_delay)
	{
		handle_if_false(index < m_context->image->frame_count, EE_OUTOFBOUNDS, L"帧索引超界");
		if (m_context->image->delays == nullptr) { *r_delay = 0; }
		else { *r_delay = m_context->image->delays[index]; }
		return S_OK;
	}
	HRESULT EXOBJCALL ExDecodeImageLibPng::CopyFrame(uint32_t index, EXBITSDATA r_bits, uint32_t stride)
	{
		CHECK_PARAM(r_bits);
		handle_if_false(index < m_context->image->frame_count, EE_OUTOFBOUNDS, L"帧索引超界");

		// 计算帧数据的大小
		uint32_t stride_src = m_context->image->width * sizeof(EXARGB);
		uint32_t frame_size = stride * m_context->image->height;
		EXBITSDATA frame_data = m_context->image->frames_data + frame_size * index;

		//拷贝像素数据
		return ExImageCopyData(m_context->image->width, m_context->image->height,
			r_bits, frame_data, stride, stride_src, 0, 0
		);
	}
}


