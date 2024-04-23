/**
 * @file dllmain.cpp
 * @brief 入口头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */

#include "stdafx.h"
#include "src/kernel/module_utils.h"
#include "common/string.hpp"

#define _ROOT_(path)	(L"../../../../" path)

BOOL APIENTRY DllMain(HMODULE module_handle, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

namespace ExDirectUI
{
	EXTERN_C HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance)
	{
		try
		{
			auto render = ExModuleUtils::Instance()->GetRender();


			return S_OK;
		}
		catch_default({});
	}

	EXTERN_C EXAPI IExModuleUtils* EXCALL ExDbgGetModuleUtils()
	{
		auto utils = ExModuleUtils::Instance();
		return utils;
	}
	
}
