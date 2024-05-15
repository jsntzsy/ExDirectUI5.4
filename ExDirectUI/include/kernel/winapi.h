/**
 * @file winapi.h
 * @brief 系统功能头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	bool EXAPI EXCALL ExSleep(uint64_t us, bool block = false);

	BSTR EXAPI ExSysAllocStringFormat(LPCWSTR format, ...);
}
