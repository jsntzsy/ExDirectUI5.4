/**
 * @file engine.cpp
 * @brief 引擎管理实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "app/engine.h"
#include "src/app/engine.h"
#include "src/kernel/winapi.h"
#include "src/kernel/module.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HINSTANCE g_engine_instance = NULL;
	DWORD g_engine_flags = 0;

	HRESULT EXAPI EXCALL ExEngineInit(const ExEngineInitInfo* init_info)
	{
		CHECK_PARAM(init_info);
		CHECK_PARAM(init_info->instance);
		handle_if_false(g_engine_instance == NULL, EE_NOREADY, L"引擎已被初始化");

		try
		{
			g_engine_flags = init_info->flags;

			_ExWinAPI_Init(init_info);

			_ExModule_Init(init_info);

			_ExRender_Init(init_info);

			g_engine_instance = init_info->instance;
			return S_OK;
		}
		catch_default({});
	}

	HRESULT EXAPI EXCALL ExEngineUnInit()
	{
		handle_if_false(g_engine_instance != NULL, EE_NOREADY, L"引擎尚未初始化");

		try
		{
			_ExRender_UnInit();

			_ExModule_UnInit();

			_ExWinAPI_UnInit();

			//g_engine_flags = 0;
			g_engine_instance = NULL;
			return S_OK;
		}
		catch_default({});
	}

	bool EXAPI EXCALL ExEngineQueryFlag(DWORD flag)
	{
		return (g_engine_flags & flag) == flag;
	}

}


