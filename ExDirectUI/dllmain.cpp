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
#include "atom.h"

#include "src/element/window.h"

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
	LRESULT CALLBACK _ExWnd_TestClass_WndProc(HWND window, uint32_t msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_DESTROY: PostQuitMessage(0); break;
		default: return DefWindowProcW(window, msg, wparam, lparam);
		}
		return 0;
	}

	EXTERN_C HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance)
	{
		try
		{
			auto render = ExModuleUtils::Instance()->GetRender();

			ExWndRegister(L"ExDirectUI.Window.Test", _ExWnd_TestClass_WndProc);

			HWND window = ExWndCreate(NULL, 10, 10, 400, 300, L"ExDirectUI.Window.Test",
				L"ExDirectUI.Window.Test");

			ExWindow* w = new ExWindow(window, 0, nullptr);
			w->Update();
			w->Show(SW_SHOW);


			ExWndMessageLoop();


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
