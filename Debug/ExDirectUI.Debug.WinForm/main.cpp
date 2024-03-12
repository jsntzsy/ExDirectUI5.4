/**
 * @file main.cpp
 * @brief 入口实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */

#include "stdafx.h"


namespace ExDirectUI
{
	EXTERN_C HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	try {
		ExEngineInitInfo eii{};
		eii.instance = hInstance;
		eii.flags = EX_ENGINE_FLAG_DEBUG;
		HRESULT status = ExEngineInit(&eii);
		throw_if_failed(status, L"引擎初始化失败");
		status = ExDbgEntry(hInstance);
		ExEngineUnInit();
		return status;
	}
	catch_default({ ExEngineUnInit(); });
}

