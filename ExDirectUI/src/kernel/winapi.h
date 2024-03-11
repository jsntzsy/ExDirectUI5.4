/**
 * @file winapi.h
 * @brief WinAPI对接管理内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	typedef DWORD(WINAPI* _RtlComputeCrc32Proc)(DWORD init_value, const void* ptr, DWORD size);

	extern HMODULE g_winapi_ntdll;
	extern HMODULE g_winapi_user32;
	extern _RtlComputeCrc32Proc _RtlComputeCrc32;

	HRESULT _ExWinAPI_Init(const ExEngineInitInfo* init_info);
	void _ExWinAPI_UnInit();

}
