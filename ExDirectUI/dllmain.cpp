/**
 * @file dllmain.cpp
 * @brief 入口头文件
 * @author EternalZSY
 * @version 5.3.0.0
 * @date 2024-03-10
 * @copyright
 */

#include "stdafx.h"
#include "common/interfaces/object_impl.hpp"

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
	HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance)
	{


		return S_OK;
	}
}
