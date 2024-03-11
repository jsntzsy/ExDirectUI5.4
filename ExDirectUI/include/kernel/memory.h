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

}
