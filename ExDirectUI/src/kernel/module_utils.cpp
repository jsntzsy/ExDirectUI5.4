/**
 * @file module_utils.cpp
 * @brief 模块辅助对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "stdafx.h"
#include "src/kernel/module_utils.h"
#include "kernel/module.h"

#include "src/drawing/image_decoder.h"

namespace ExDirectUI
{
	HRESULT EXOBJCALL ExModuleUtils::Group(uint16_t type, IExModule* instance) MAYTHROW
	{
		switch (type)
		{
		case ExDirectUI::EX_MODULE_IMAGE_DECODER: {
			ExAutoPtr<IExImageDecoder> decoder;
			throw_if_failed(instance->QueryInterface(&decoder), L"未实现对应接口");
			return _ExImageDecoder_Group(decoder);
		}
		case ExDirectUI::EX_MODULE_RENDER: {
			
			throw_ex(E_NOTIMPL, L"尚未实现");
		}
		default: throw_ex(E_NOTIMPL, L"不支持的模块类型");
		}
	}
	HRESULT EXOBJCALL ExModuleUtils::UnGroup(IExModule* instance) MAYTHROW
	{
		switch (instance->GetType())
		{
		case ExDirectUI::EX_MODULE_IMAGE_DECODER: {
			return _ExImageDecoder_UnGroup((IExImageDecoder*)instance);
		}
		case ExDirectUI::EX_MODULE_RENDER: {
			
			throw_ex(E_NOTIMPL, L"尚未实现");
		}
		default: throw_ex(E_NOTIMPL, L"不支持的模块类型");
		}
	}
	
	HRESULT EXOBJCALL ExModuleUtils::DecodeImageFile(LPCWSTR file, IExDecodeImage** r_image)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);
		
		return _ExImageDecoder_LoadFromFile(file, r_image);
	}
	HRESULT EXOBJCALL ExModuleUtils::DecodeImageMemory(const byte_t* data, size_t size, IExDecodeImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);
		
		return _ExImageDecoder_LoadFromMemory(data, size, r_image);
	}
}


