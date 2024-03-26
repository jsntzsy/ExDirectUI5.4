/**
 * @file stdafx.h
 * @brief 预编译头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#include "ExDirectUI.h"
using namespace ExDirectUI;

namespace ExDirectUI
{
	EXTERN_C HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance);
	EXTERN_C EXAPI IExModuleUtils* EXCALL ExDbgGetModuleUtils();

	void APIENTRY _RenderTest_(HINSTANCE instance);
}

