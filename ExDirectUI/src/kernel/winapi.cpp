/**
 * @file winapi.cpp
 * @brief 对接管理实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "src/kernel/winapi.h"

namespace ExDirectUI
{
	HMODULE g_winapi_ntdll = NULL;
	HMODULE g_winapi_user32 = NULL;
	_RtlComputeCrc32Proc _RtlComputeCrc32 = nullptr;

	HRESULT _ExWinAPI_Init(const ExEngineInitInfo* init_info)
	{
		g_winapi_ntdll = LoadLibraryW(L"ntdll.dll");
		handle_if_false(g_winapi_ntdll, EE_LOST_NECESSARY, L"ntdll.dll加载失败");

		_RtlComputeCrc32 = (_RtlComputeCrc32Proc)GetProcAddress(g_winapi_ntdll, "RtlComputeCrc32");
		handle_if_false(_RtlComputeCrc32, EE_LOST_NECESSARY, L"RtlComputeCrc32加载失败");

		g_winapi_user32 = LoadLibraryW(L"user32.dll");
		handle_if_false(g_winapi_user32, EE_LOST_NECESSARY, L"user32.dll加载失败");
		
		return S_OK;
	}
	void _ExWinAPI_UnInit()
	{
		_RtlComputeCrc32 = nullptr;
		SAFE_FREE(g_winapi_user32, FreeLibrary);
		SAFE_FREE(g_winapi_ntdll, FreeLibrary);
	}
}


