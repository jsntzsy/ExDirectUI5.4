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

			ExAutoPtr<IExDecodeImage> image1;
			ExModuleUtils::Instance()->DecodeImageFile(L"../../../../_res/apng/ball.a.png", &image1);
			ExDebugOutputRaw(to_wstring(image1->GetFrameCount()).c_str());

			ExAutoPtr<IExDecodeImage> image2;
			ExModuleUtils::Instance()->DecodeImageFile(L"../../../../_res/1.jpg", &image2);
			

			return S_OK;
		}
		catch_default({});
	}
}
