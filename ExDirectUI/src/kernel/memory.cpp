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
		// 如果ptr为空,直接分配新内存
		if (!ptr) { return ExMemAlloc(new_size); }

		//获取一下旧内存尺寸
		size_t old_size = LocalSize(ptr);
		LPVOID new_ptr = nullptr;

		//如果尺寸一样就不分配了
		if (old_size == new_size) { return ptr; }

		//判断一下,如果是缩小,则直接调用LocalReAlloc缩小
		if (old_size > new_size) {
			new_ptr = LocalReAlloc(ptr, new_size, LMEM_FIXED | LMEM_ZEROINIT);
		}

		//如果扩大内存或LocalReAlloc执行失败,则手动重新分配
		if (new_ptr == nullptr) {

			//申请新内存
			new_ptr = ExMemAlloc(new_size);
			if (!new_ptr) { return nullptr; }

			//从源内存拷贝数据到新内存头部
			memcpy(new_ptr, ptr, min(old_size, new_size));

			//释放旧内存
			ExMemFree(ptr);
		}

		//返回新内存
		return new_ptr;
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


