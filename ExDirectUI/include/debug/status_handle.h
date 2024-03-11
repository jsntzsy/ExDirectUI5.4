﻿/**
 * @file status_handle.h
 * @brief 状态处理功能定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	typedef bool(CALLBACK* ExStatusHandleProc)(HRESULT status, LPCWSTR file, int line, LPCWSTR text) NOTHROW;

	ExStatusHandleProc EXAPI EXCALL ExStatusSetUserHandler(ExStatusHandleProc handler);
	ExStatusHandleProc EXAPI EXCALL ExStatusGetUserHandler();

	LPCWSTR EXAPI EXCALL ExStatusGetText(HRESULT status, bool* free);
	HRESULT EXAPI EXCALL ExStatusHandle(HRESULT status, LPCWSTR file, int line, LPCWSTR text);
	HRESULT EXAPI CDECL ExStatusHandleEx(HRESULT status, LPCWSTR file, int line, LPCWSTR format, ...);

}
