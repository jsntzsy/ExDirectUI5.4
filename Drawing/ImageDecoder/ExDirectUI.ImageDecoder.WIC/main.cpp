/**
 * @file main.cpp
 * @brief 模块入口实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */

#include "stdafx.h"

namespace ExDirectUI
{
	EXTERN_C HRESULT APIENTRY _ExDirectUI_ImageDecoder_WIC_(HMODULE module_handle, IExModuleUtils* utils,
		ExModuleInfo* r_info, IExModule** r_module)
	{
		try
		{
			auto instance = ExImageDecoderWIC::GetInstance();
			instance->GetInfo(r_info);
			throw_if_failed(instance->QueryInterface(r_module), L"不支持的接口");
			instance->Release();
			return S_OK;
		}
		catch_default({ ExImageDecoderWIC::ClearInstance(true); });
	}

}


