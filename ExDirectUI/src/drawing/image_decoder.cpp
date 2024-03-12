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


}


