/**
 * @file memory.cpp
 * @brief 内存管理实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "kernel/memory.h"

namespace ExDirectUI
{
	LPVOID EXAPI EXCALL ExMemAlloc(size_t size)
	{
		return LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, size);
	}

	LPVOID EXAPI EXCALL ExMemReAlloc(LPVOID ptr, size_t new_size)
	{
		return LocalReAlloc(ptr, new_size, LMEM_MOVEABLE | LMEM_ZEROINIT);
	}

	void EXAPI EXCALL ExMemFree(LPVOID ptr)
	{
		LocalFree(ptr);
	}

	size_t EXAPI EXCALL ExMemGetSize(LPVOID ptr)
	{
		return LocalSize(ptr);
	}
}


