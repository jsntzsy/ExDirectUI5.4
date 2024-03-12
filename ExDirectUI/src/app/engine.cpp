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

namespace ExDirectUI
{
	HINSTANCE g_engine_instance = NULL;
	DWORD g_engine_flags = 0;

	HRESULT EXAPI EXCALL ExEngineInit(const ExEngineInitInfo* init_info)
	{
		CHECK_PARAM(init_info);
		CHECK_PARAM(init_info->instance);
		handle_if_false(g_engine_instance == NULL, EE_NOREADY, L"引擎已被初始化");

		_ExWinAPI_Init(init_info);

		g_engine_flags = init_info->flags;
		g_engine_instance = init_info->instance;
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExEngineUnInit()
	{
		handle_if_false(g_engine_instance != NULL, EE_NOREADY, L"引擎尚未初始化");

		_ExWinAPI_UnInit();

		g_engine_flags = 0;
		g_engine_instance = NULL;
		return S_OK;
	}

	bool EXAPI EXCALL ExEngineQueryFlag(DWORD flag)
	{
		return (g_engine_flags & flag) == flag;
	}

}


