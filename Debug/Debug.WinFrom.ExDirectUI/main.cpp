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
	HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	HRESULT status = ExDbgEntry(hInstance);


	return status;
}

