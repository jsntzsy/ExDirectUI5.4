/**
 * @file status_handle.h
 * @brief 状态处理功能定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

#define EX_WARNING_MARK		L"[警告]"
#define EX_WARNING(TXT)	EX_WARNING_MARK TXT

namespace ExDirectUI
{
	typedef bool(CALLBACK* ExStatusHandleProc)(HRESULT status, LPCWSTR file, int line, LPCWSTR text) NOTHROW;
	typedef bool(CALLBACK* ExDbgOutputHandleProc)(LPCWSTR str) NOTHROW;

	ExStatusHandleProc EXAPI EXCALL ExStatusSetUserHandler(ExStatusHandleProc handler);
	ExStatusHandleProc EXAPI EXCALL ExStatusGetUserHandler();

	LPCWSTR EXAPI EXCALL ExStatusGetText(HRESULT status, bool* free);
	HRESULT EXAPI EXCALL ExStatusHandle(HRESULT status, LPCWSTR file, int line, LPCWSTR text);
	HRESULT EXAPI CDECL ExStatusHandleEx(HRESULT status, LPCWSTR file, int line, LPCWSTR format, ...);

	void EXAPI EXCALL ExDbgOutput(LPCWSTR str);
	ExDbgOutputHandleProc EXAPI EXCALL ExDbgSetOutputHandler(ExDbgOutputHandleProc proc);
	ExDbgOutputHandleProc EXAPI EXCALL ExDbgGetOutputHandler();

}
