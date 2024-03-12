/**
 * @file dllmain.cpp
 * @brief 入口头文件
 * @author EternalZSY
 * @version 5.3.0.0
 * @date 2024-03-10
 * @copyright
 */

#include "stdafx.h"
#include <common/utils/auto_ptr.hpp>

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
			ExAutoPtr<IExResPool> res_pool;
			throw_if_failed(ExResPoolCreate(10, NULL, NULL, &res_pool), L"创建资源池失败");
			auto str = res_pool->ToString();


			return S_OK;
		}
		catch_default({});
	}
}
