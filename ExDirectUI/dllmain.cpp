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

			ExPackageInfo pi = {
				ExPackageType::ThemePackage,
				0x00,
				0,
				L"ExDirectUI.Theme.Default"
			};

			//ExData data{};
			//ExPackageBuildFromDirectory(
			//	&pi,
			//	_ROOT_(L"_res/theme/test/"),
			//	L"*",
			//	ExPackageBuildFlags::Subdirectories,
			//	nullptr, 0,
			//	nullptr, 0,
			//	&data
			//);
			//
			//ExDataWriteFile(_ROOT_(L"_res/theme/test.ext"), data.data, data.size);
			//ExDataFree(&data);

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
