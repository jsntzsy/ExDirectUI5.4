/**
 * @file decode_image.cpp
 * @brief 解码图像实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */

#include "stdafx.h"
#include "decode_image.h"

namespace ExDirectUI
{
	ExDecodeImageWIC::ExDecodeImageWIC(ExDecodeImageContextWIC* context)
		:m_context(context)
	{}

	ExDecodeImageWIC::~ExDecodeImageWIC()
	{
		if (m_context) {
			ExImageDecoderWIC::Instance()->FreeImage(m_context);
		}
	}

	uint32_t ExDecodeImageWIC::GetWidth() noexcept
	{
		return m_context->width;
	}

	uint32_t EXOBJCALL ExDecodeImageWIC::GetHeight()
	{
		return m_context->height;
	}

	uint32_t EXOBJCALL ExDecodeImageWIC::GetFrameCount()
	{
		return m_context->frame_count;
	}

	HRESULT EXOBJCALL ExDecodeImageWIC::GetFrameDelay(uint32_t index, uint32_t* r_delay)
	{
		handle_if_false(index < m_context->frame_count, EE_OUTOFBOUNDS, L"帧索引超界");
		if (m_context->delays == nullptr) { *r_delay = 0; }
		else { *r_delay = m_context->delays[index]; }
		return S_OK;
	}
	HRESULT EXOBJCALL ExDecodeImageWIC::CopyFrame(uint32_t index, EXBITSDATA r_bits, uint32_t stride)
	{
		CHECK_PARAM(r_bits);
		handle_if_false(index < m_context->frame_count, EE_OUTOFBOUNDS, L"帧索引超界");

		// 计算帧数据的大小
		uint32_t stride_src = m_context->width * sizeof(EXARGB);
		uint32_t frame_size = stride * m_context->height;
		EXBITSDATA frame_data = m_context->frames_data + frame_size * index;

		//拷贝像素数据
		return ExImageCopyData(m_context->width, m_context->height,
			r_bits, frame_data, stride, stride_src, 0, 0
		);
	}
}


