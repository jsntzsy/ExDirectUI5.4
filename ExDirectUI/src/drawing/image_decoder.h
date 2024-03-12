/**
 * @file image_decoder.h
 * @brief 图像解码器管理内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	HRESULT EXCALL _ExImageDecoder_Group(IExImageDecoder* decoder);
	HRESULT EXCALL _ExImageDecoder_UnGroup(IExImageDecoder* decoder);
	
	HRESULT EXCALL _ExImageDecoder_LoadFromFile(LPCWSTR file, IExDecodeImage** r_image);
	HRESULT EXCALL _ExImageDecoder_LoadFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image);
}
