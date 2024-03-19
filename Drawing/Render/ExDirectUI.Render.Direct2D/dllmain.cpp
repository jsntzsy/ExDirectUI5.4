/**
 * @file dllmain.cpp
 * @brief 模块入口实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */

#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

namespace ExDirectUI
{
	EXTERN_C HRESULT APIENTRY _ExDirectUI_Render_Direct2D_(HMODULE module_handle, IExModuleUtils* utils,
		ExModuleInfo* r_info, IExModule** r_module)
	{
		try
		{
			auto instance = ExRenderD2D::GetInstance(utils);
			instance->GetInfo(r_info);
			throw_if_failed(instance->QueryInterface(r_module), L"不支持的接口");
			instance->Release();
			return S_OK;
		}
		catch_default({ ExRenderD2D::ClearInstance(true); });
	}
}


