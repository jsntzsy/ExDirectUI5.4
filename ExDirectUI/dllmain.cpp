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
#include "common/utils/string.hpp"

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
			ExAutoPtr<IExResPool> res_pool;
			throw_if_failed(ExResPoolCreate(10, NULL, NULL, &res_pool), L"创建资源池失败");

			ExAutoPtr<IExDecodeImage> image1;
			ExModuleUtils::Instance()->DecodeImageFile(_ROOT_(L"_res/apng/ball.a.png"), &image1);
			if (image1) {
				ExDebugOutputRaw(
					ExString::format(L"ImageInfo: size: %ux%u, frame: %u\n",
						image1->GetWidth(), image1->GetHeight(), image1->GetFrameCount()
					).c_str()
				);
			}

			ExAutoPtr<IExDecodeImage> image2;
			ExModuleUtils::Instance()->DecodeImageFile(_ROOT_(L"_res/1.gif"), &image2);
			if (image2) {
				ExDebugOutputRaw(
					ExString::format(L"ImageInfo: size: %ux%u, frame: %u\n",
						image2->GetWidth(), image2->GetHeight(), image2->GetFrameCount()
					).c_str()
				);
			}


			return S_OK;
		}
		catch_default({});
	}

	EXTERN_C EXAPI IExModuleUtils* EXCALL ExDbgGetModuleUtils()
	{
		auto utils = ExModuleUtils::Instance();
		utils->AddRef();
		return utils;
	}
	
}
