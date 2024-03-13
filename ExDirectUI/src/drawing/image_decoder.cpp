/**
 * @file image_decoder.cpp
 * @brief 图像解码器管理实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "stdafx.h"
#include "drawing/image_decoder.h"
#include "src/drawing/image_decoder.h"

namespace ExDirectUI
{
	std::vector<IExImageDecoder*> g_drawing_image_decoders;

	HRESULT EXCALL _ExImageDecoder_Group(IExImageDecoder* decoder)
	{
		auto it = std::find(
			g_drawing_image_decoders.begin(), g_drawing_image_decoders.end(),
			decoder
		);

		handle_if_false(it == g_drawing_image_decoders.end(), EE_EXISTS, L"图像解码器已经存在");

		decoder->AddRef();
		g_drawing_image_decoders.push_back(decoder);
		return S_OK;
	}

	HRESULT EXCALL _ExImageDecoder_UnGroup(IExImageDecoder* decoder)
	{
		auto it = std::find(
			g_drawing_image_decoders.begin(), g_drawing_image_decoders.end(),
			decoder
		);

		handle_if_false(it != g_drawing_image_decoders.end(), EE_NOEXISTS, L"图像解码器不存在");

		(*it)->Release();
		g_drawing_image_decoders.erase(it);
		return S_OK;
	}

	HRESULT EXCALL _ExImageDecoder_LoadFromFile(LPCWSTR file, IExDecodeImage** r_image)
	{
		for (auto& decoder : g_drawing_image_decoders) {
			if (decoder->LoadImageFromFile(file, r_image) == S_OK) {
				return S_OK;
			}
		}
		handle_ex(E_NOTIMPL, L"不支持的图像格式");
	}

	HRESULT EXCALL _ExImageDecoder_LoadFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image)
	{
		for (auto& decoder : g_drawing_image_decoders) {
			if (decoder->LoadImageFromMemory(data, size, r_image) == S_OK) {
				return S_OK;
			}
		}
		handle_ex(E_NOTIMPL, L"不支持的图像格式");
	}

	HRESULT EXAPI EXCALL ExImageCopyData(uint32_t width, uint32_t height, EXBITSDATA r_dest, const EXBITSDATA src,
		uint32_t stride_dst, uint32_t stride_src, uint32_t left, uint32_t top)
	{
		CHECK_PARAM(width > 0);
		CHECK_PARAM(height > 0);
		CHECK_PARAM(r_dest != nullptr);
		CHECK_PARAM(src != nullptr);

		//默认值处理
		if (stride_dst == 0) { stride_dst = width * sizeof(EXARGB); }
		if (stride_src == 0) { stride_src = width * sizeof(EXARGB); }

		//计算拷贝区域
		ExRect copy_rect(left, top, width, height, true);
		size_t line_size = copy_rect.Width() * sizeof(EXARGB);
		uint32_t offset_size = copy_rect.left * sizeof(EXARGB);

		//循环拷贝
		for (uint32_t y = copy_rect.top; y < (uint32_t)copy_rect.bottom; y++)
		{
			memcpy(
				r_dest + stride_dst * y,
				src + stride_src * y + offset_size,
				line_size
			);
		}
		return S_OK;
	}

}
