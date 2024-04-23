/**
 * @file status_handle.cpp
 * @brief 状态处理功能实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "debug/status_handle.h"
#include <sstream>

namespace ExDirectUI
{
	static ExStatusHandleProc s_debug_status_handler = nullptr;
	static ExDbgOutputHandleProc s_debug_output_handler = nullptr;

	////////////////////////////////////////////////////////////////////////////

	ExStatusHandleProc EXAPI EXCALL ExStatusSetUserHandler(ExStatusHandleProc handler)
	{
		ExStatusHandleProc old = s_debug_status_handler;
		s_debug_status_handler = handler;
		return old;
	}
	ExStatusHandleProc EXAPI EXCALL ExStatusGetUserHandler()
	{
		return s_debug_status_handler;
	}

	LPCWSTR EXAPI EXCALL ExStatusGetText(HRESULT status, bool* free)
	{
		CHECK_PARAM_RET(free, nullptr);

		LPWSTR text = nullptr;
		*free = false;

		//先看是不是引擎内部的状态码
		if ((status >> 16) & FACILITY_ITF) {
			text = (LPWSTR)_ExStatus_GetName(status);
			if (text) { return text; }
		}

		//通过系统API获取状态码信息
		FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr, status, 0, (LPWSTR)&text, 0, nullptr
		);

		if (text) {

			//去掉末尾的回车换行符
			size_t len = wcslen(text);
			if (len > 0) {
				if (text[len - 2] == L'\r') { text[len - 2] = L'\0'; }
				else if (text[len - 1] == L'\n') { text[len - 1] = L'\0'; }
			}

			*free = true;
		}
		return text;
	}
	HRESULT EXAPI EXCALL ExStatusHandle(HRESULT status, LPCWSTR file, int line, LPCWSTR text)
	{
		wchar_t buffer[EX_CFG_SIZEOF_FORMAT_BUF]{};

		//生成状态文本信息
		bool free = false;
		LPCWSTR status_text = ExStatusGetText(status, &free);
		swprintf_s(buffer, L"0x%08X(%s) ", status, status_text);
		if (free) { LocalFree((HLOCAL)status_text); }

		//连接附加文本
		if (text) { wcscat_s(buffer, text); }

		//调用用户处理函数
		if (s_debug_status_handler && s_debug_status_handler(status, file, line, buffer)) {
			return status;
		}

		//系统默认处理
		if (ExEngineQueryFlag(ExEngineFlags::Debug)) {
#ifdef EX_CFG_DEBUG_INTERRUPT

			bool interrupt = ExEngineQueryFlag(ExEngineFlags::InterruptWarning);

#ifndef EX_CFG_DEBUG_INTERRUPT_WARNING

			if (interrupt && text) {
				//如果附加文本头部不是MARK，则需要中断
				interrupt = wcsstr(text, EX_WARNING_MARK) != text;
			}
#endif // !EX_CFG_DEBUG_INTERRUPT_WARNING

			if (interrupt) {
				ExAssertFmtCallInfo(SUCCEEDED(status), file, line, buffer);
			}

#elif defined(EX_CFG_DEBUG_OUTPUT)
			if (file) {
				wchar_t file_info[EX_CFG_SIZEOF_FORMAT_BUF]{};
				swprintf_s(file_info, L"%s(%d): ", file, line);
				ExDbgOutput(file_info);
			}
			ExDbgOutput(buffer);
#endif 
		}

		return status;
	}
	HRESULT EXAPI CDECL ExStatusHandleEx(HRESULT status, LPCWSTR file, int line, LPCWSTR format, ...)
	{
		WCHAR buffer[EX_CFG_SIZEOF_FORMAT_BUF]{};
		if (format) {
			va_list args;
			va_start(args, format);
			_vsnwprintf_s(buffer, _countof(buffer), format, args);
			va_end(args);
		}
		return ExStatusHandle(status, file, line, buffer);
	}

	void EXAPI EXCALL ExDbgOutput(LPCWSTR str)
	{
		if (str == nullptr || *str == L'\0') { return; }
		if (s_debug_output_handler && s_debug_output_handler(str)) { return; }

		//系统默认处理
		if (ExEngineQueryFlag(ExEngineFlags::Debug)) {
#ifdef EX_CFG_DEBUG_OUTPUT
			OutputDebugStringW(str);
#endif 
		}
	}

	ExDbgOutputHandleProc EXAPI EXCALL ExDbgSetOutputHandler(ExDbgOutputHandleProc proc)
	{
		auto old_proc = s_debug_output_handler;
		s_debug_output_handler = proc;
		return old_proc;
	}

	ExDbgOutputHandleProc EXAPI EXCALL ExDbgGetOutputHandler()
	{
		return s_debug_output_handler;
	}

}


