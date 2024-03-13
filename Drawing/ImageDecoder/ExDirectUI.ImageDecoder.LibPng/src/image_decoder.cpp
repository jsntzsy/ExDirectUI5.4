/**
 * @file image_decoder.cpp
 * @brief 图像解码器实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "stdafx.h"
#include "image_decoder.h"

namespace ExDirectUI
{
	ExImageDecoderLibPng::ExImageDecoderLibPng()
	{
		throw_if_failed(
			ExResPoolCreate(sizeof(ExDecodeImageContextLibPng),
				_OnInitImage, _OnFreeImage, -m_pool
			), L"创建图像资源池失败"
		);
	}
	ExImageDecoderLibPng::~ExImageDecoderLibPng()
	{
		this->ClearInstance(false);
	}

	HRESULT EXOBJCALL ExImageDecoderLibPng::Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExImageDecoderLibPng::LoadImageFromFile(LPCWSTR file, IExDecodeImage** r_image)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);
		ExAssert(m_pool);

		//获取文件原子号
		EXATOM atom = ExAtomFile(file);
		if (atom == EXATOM_UNKNOWN) { return EE_IO; }

		//加载图像上下文
		ExDecodeImageContextLibPng* context = nullptr;
		handle_if_failed(
			m_pool->UseOrCreateItem(atom, file, 0, 0, 0, (void**)&context),
			L"加载图像失败"
		);

		//创建解码图像对象
		ExAutoPtr<ExDecodeImageLibPng> image = NEW ExDecodeImageLibPng(context);
		return image->QueryInterface(r_image);
	}

	HRESULT EXOBJCALL ExImageDecoderLibPng::LoadImageFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);
		ExAssert(m_pool);

		//获取文件原子号
		EXATOM atom = ExAtomData(data, size);
		if (atom == EXATOM_UNKNOWN) { return EE_IO; }

		//加载图像上下文
		ExDecodeImageContextLibPng* context = nullptr;
		handle_if_failed(
			m_pool->UseOrCreateItem(atom, data, size, 0, 0, (void**)&context),
			L"加载图像失败"
		);

		//创建解码图像对象
		ExAutoPtr<ExDecodeImageLibPng> image = NEW ExDecodeImageLibPng(context);
		return image->QueryInterface(r_image);
	}

	HRESULT EXOBJCALL ExImageDecoderLibPng::FreeImage(const ExDecodeImageContextLibPng* image_context)
	{
		CHECK_PARAM(image_context);
		ExAssert(m_pool);
		return m_pool->UnUseItemByPtr((void*)image_context);
	}

	HRESULT ExImageDecoderLibPng::_OnInitImage(IExResPool* pool, EXATOM key,
		const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res)
	{
		ExDecodeImageContextLibPng* context = (ExDecodeImageContextLibPng*)r_res;

		//根据wparam判断是从文件加载还是从内存加载
		if (wparam == 0) {
			context->image = ExAPngLoadFromFile((LPCWSTR)data);
		}
		else {
			context->image = ExAPngLoadFromMemory((byte_t*)data, wparam);
		}
		return context->image ? S_OK : E_FAIL;
	}

	HRESULT ExImageDecoderLibPng::_OnFreeImage(IExResPool* pool, EXATOM key, DWORD flags, void* res)
	{
		ExDecodeImageContextLibPng* context = (ExDecodeImageContextLibPng*)res;
		SAFE_FREE(context->image, ExAPngFree);
		return S_OK;
	}

}


