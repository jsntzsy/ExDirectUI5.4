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

#include <OleCtl.h>
#pragma comment(lib,"OleAut32.lib")

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

	/////////////////////////////////////////////

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
	
	HRESULT EXAPI EXCALL ExImageLoadObject(const byte_t* data, size_t size,
		uint32_t type, uint32_t index, HGDIOBJ* r_object)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_object);

		handle_if_false(!IsBadReadPtr(data, size), E_INVALIDARG, L"内存数据不可读");

		HGLOBAL hdata = nullptr;
		try
		{
			hdata = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
			throw_if_false(hdata, E_OUTOFMEMORY, L"分配内存失败");

			byte_t* pdata = (byte_t*)GlobalLock(hdata);
			throw_if_false(pdata, E_OUTOFMEMORY, L"锁定内存失败");
			CopyMemory(pdata, data, size);
			GlobalUnlock(hdata);

			if (type == IMAGE_ICON || type == IMAGE_CURSOR) {

#pragma pack(push, 1)
				struct _IconFile_Header {

					//文件头
					byte_t header[2];
					uint16_t type;
					uint16_t count;

					//图像信息
					byte_t width;
					byte_t height;
				};
#pragma pack(pop)

				const _IconFile_Header* header = (_IconFile_Header*)data;

				//检查索引
				if (index == 0) { index = 1; }
				throw_if_false(header->count >= index, EE_OUTOFBOUNDS, L"索引超界");

				bool is_icon = header->type == 0x0001;
				const uint32_t* frame = (uint32_t*)(data + 6 + (index - 1) * 16);

				byte_t* temp = pdata;
				if (!is_icon) {
					((uint32_t*)pdata)[0] = frame[1];
					temp += sizeof(uint32_t);
				}

				CopyMemory(temp, data + frame[3], frame[2]);
				uint8_t w = ((uint8_t*)frame)[0];
				uint8_t h = ((uint8_t*)frame)[1];

				HICON icon_temp = CreateIconFromResourceEx(pdata, frame[2], is_icon,
					196608, w, h, 0);

				*r_object = CopyImage(icon_temp, type, 0, 0, LR_COPYRETURNORG);
			}
			else {
				ExAutoPtr<IStream> stream;
				ExAutoPtr<IPicture> picture;
				throw_if_failed(CreateStreamOnHGlobal(hdata, false, &stream), L"创建流失败");
				throw_if_failed(
					OleLoadPicture(stream, (LONG)size, true,
						IID_IPicture, (void**)&picture),
					L"加载图片对象失败"
				);

				OLE_HANDLE handle = NULL;
				SHORT obj_type = 0;
				picture->get_Handle(&handle);
				picture->get_Type(&obj_type);

				throw_if_false(type == obj_type, E_INVALIDARG, L"图像类型不匹配");

				*r_object = CopyImage((HANDLE)handle, type, 0, 0, LR_COPYRETURNORG);
			}

			GlobalFree(hdata);
			return S_OK;
		}
		catch_default({
			if (hdata) { GlobalFree(hdata); }
			}
		);
	}

}
