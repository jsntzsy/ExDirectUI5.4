/**
 * @file main.cpp
 * @brief 入口实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */

#include "stdafx.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	try {
		ExEngineInitInfo eii{};
		eii.instance = hInstance;
		eii.flags = ExEngineFlags::Debug;

		ExModuleFileInfo modules[] = {
			{L"ExDirectUI.ImageDecoder.WIC.dll",0,0},
			{L"ExDirectUI.Render.Direct2D.dll",0,0},
		};

		eii.module_load_mode = ExModuleLoadFileMode::FromFile;
		eii.modules.files = modules;
		eii.module_count = _countof(modules);

		HRESULT status = ExEngineInit(&eii);
		throw_if_failed(status, L"引擎初始化失败");
		
		//_RenderTest_(hInstance);
		status = ExDbgEntry(hInstance);
		
		ExEngineUnInit();
		return status;
	}
	catch_default({ ExEngineUnInit(); });
}

