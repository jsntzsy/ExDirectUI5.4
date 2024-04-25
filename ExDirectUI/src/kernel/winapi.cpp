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
	HMODULE g_winapi_shcore = NULL;

	_RtlComputeCrc32Proc _RtlComputeCrc32 = nullptr;

	HRESULT _ExWinAPI_Init(const ExEngineInitInfo* init_info)
	{
		g_winapi_ntdll = LoadLibraryW(L"ntdll.dll");
		handle_if_false(g_winapi_ntdll, EE_LOST_NECESSARY, L"ntdll.dll加载失败");

		_RtlComputeCrc32 = (_RtlComputeCrc32Proc)GetProcAddress(g_winapi_ntdll, "RtlComputeCrc32");
		handle_if_false(_RtlComputeCrc32, EE_LOST_NECESSARY, L"RtlComputeCrc32加载失败");

		g_winapi_user32 = LoadLibraryW(L"user32.dll");
		handle_if_false(g_winapi_user32, EE_LOST_NECESSARY, L"user32.dll加载失败");

		g_winapi_shcore = LoadLibraryW(L"shcore.dll");

		return S_OK;
	}
	void _ExWinAPI_UnInit()
	{
		_RtlComputeCrc32 = nullptr;
		SAFE_FREE(g_winapi_shcore, FreeLibrary);
		SAFE_FREE(g_winapi_user32, FreeLibrary);
		SAFE_FREE(g_winapi_ntdll, FreeLibrary);
	}

	/////////////////////////////////

	bool EXAPI EXCALL ExSleep(uint64_t us, bool block)
	{
		LARGE_INTEGER li{};
		li.QuadPart = us * -10;

		HANDLE timer = CreateWaitableTimer(nullptr, false, nullptr);
		return_if_false(timer, {}, false);

		return_if_false(
			SetWaitableTimer(
				timer, &li, 0,
				nullptr, nullptr, false
			), { CloseHandle(timer); }, false
		);

		if (block) {
			WaitForSingleObject(timer, INFINITE);
		}
		else {
			while (MsgWaitForMultipleObjects(1, &timer, false,
				INFINITE, QS_ALLEVENTS)) {
				ExWndDoEvent();
			}
		}

		CloseHandle(timer);
		return true;
	}










}


