/**
 * @file image.cpp
 * @brief 渲染器图像函数实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-20
 * @copyright
 */

#include "stdafx.h"
#include "drawing/render/image.h"
#include "src/drawing/render/factory.h"

#include <OleCtl.h>
#pragma comment(lib,"OleAut32.lib")

namespace ExDirectUI
{
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
					OleLoadPicture(stream,(LONG) size, true,
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

	HRESULT EXAPI EXCALL ExImageCreate(uint32_t width, uint32_t height, IExImage** r_image)
	{
		CHECK_PARAM(r_image);
		CHECK_PARAM(width > 0 && height > 0);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImage(width, height, &image),
			L"创建空图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromFile(LPCWSTR file, IExImage** r_image)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromFile(file, &image),
			L"从文件创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromMemory(const byte_t* data, size_t size, IExImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromMemory(data, size, &image),
			L"从内存创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromBitsData(uint32_t width, uint32_t height,
		const EXBITSDATA bits_data, uint32_t stride, IExImage** r_image)
	{
		CHECK_PARAM(r_image);
		CHECK_PARAM(width > 0 && height > 0);
		CHECK_PARAM(bits_data);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromBitsData(width, height, bits_data, stride, &image),
			L"从像素数据创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromImageData(const byte_t* data, IExImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromImageData(data, &image),
			L"从图像数据创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromHBITMAP(HBITMAP bitmap, HPALETTE palette, bool pre_alpha, IExImage** r_image)
	{
		CHECK_PARAM(bitmap);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromHBITMAP(bitmap, palette, pre_alpha, &image),
			L"从位图句柄创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

	HRESULT EXAPI EXCALL ExImageCreateFromHICON(HICON icon, IExImage** r_image)
	{
		CHECK_PARAM(icon);
		CHECK_PARAM(r_image);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		ExAutoPtr<IExImage> image;
		handle_if_failed(
			g_drawing_render->CreateImageFromHICON(icon, &image),
			L"从图标句柄创建图像对象失败"
		);
		return image->QueryInterface(r_image);
	}

}

