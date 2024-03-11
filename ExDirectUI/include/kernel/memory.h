/**
 * @file memory.h
 * @brief 内存管理头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/**
	 * @brief 内存_申请
	 * 申请一段内存
	 * @param size 申请的内存大小
	 * @return 返回分配的内存地址
	 * @attention 使用完毕后，需要调用{ExMemFree}释放内存
	 */
	LPVOID EXAPI EXCALL ExMemAlloc(size_t size);

	/**
	 * @brief 内存_重分配
	 * 重新分配一段内存
	 * @param ptr 旧内存地址
	 * @param new_size 新的内存大小
	 * @return 返回新的内存地址
	 * @attention 使用完毕后，需要调用{ExMemFree}释放内存
	 */
	LPVOID EXAPI EXCALL ExMemReAlloc(LPVOID ptr, size_t new_size);

	/**
	 * @brief 内存_释放
	 * 释放一段内存
	 * @param ptr 内存地址
	 */
	void EXAPI EXCALL ExMemFree(LPVOID ptr);

	/**
	 * @brief 内存_取尺寸
	 * 获取一段内存的尺寸
	 * @param ptr 内存地址
	 * @return 返回内存大小
	 */
	size_t EXAPI EXCALL ExMemGetSize(LPVOID ptr);

#ifdef EX_CFG_DEBUG_MEMORY_LEAK

	LPVOID EXAPI EXCALL ExMemAllocD(size_t size, LPCWSTR file, int line);
	LPVOID EXAPI EXCALL ExMemReAllocD(LPVOID ptr, size_t new_size, LPCWSTR file, int line);
	void EXAPI EXCALL ExMemFreeD(LPVOID ptr);

#define exalloc(size) ExMemAllocD(size, __CALLINFO__)
#define exrealloc(ptr, new_size) ExMemReAllocD(ptr, new_size, __CALLINFO__)
#define exfree(ptr) ExMemFreeD(ptr)

#else

#define exalloc(size) ExMemAlloc(size)
#define exrealloc(ptr, new_size) ExMemReAlloc(ptr, new_size)
#define exfree(ptr) ExMemFree(ptr)

#endif // EX_CFG_DEBUG_MEMORY_LEAK




}
