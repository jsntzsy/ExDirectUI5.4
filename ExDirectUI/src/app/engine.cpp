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


	HRESULT EXAPI EXCALL ExEngineInit(const ExEngineInitInfo* init_info)
	{
		CHECK_PARAM(init_info);
		CHECK_PARAM(init_info->instance);
		handle_if_false(g_engine_instance == NULL, EE_NOREADY, L"引擎已被初始化");

		_ExWinAPI_Init(init_info);

		g_engine_instance = init_info->instance;
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExEngineUnInit()
	{
		handle_if_false(g_engine_instance != NULL, EE_NOREADY, L"引擎尚未初始化");

		_ExWinAPI_UnInit();

		g_engine_instance = NULL;
		return S_OK;
	}

}


