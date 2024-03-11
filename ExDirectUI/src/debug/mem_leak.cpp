/**
 * @file mem_leak.cpp
 * @brief 内存泄漏检查实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "src/debug/mem_leak.h"

namespace ExDirectUI
{
#ifdef EX_CFG_DEBUG_MEMORY_LEAK

	ExMemLeakChecker::ExMemLeakChecker()
	{
	}
	ExMemLeakChecker::~ExMemLeakChecker()
	{
		DumpLeaks();
		m_pool.Clear();
	}
	void ExMemLeakChecker::Add(const void* ptr, size_t size, LPCWSTR file, int line)
	{
		if (ptr == nullptr) { return; }
		_Locked(m_lock);

		//分配内存
		auto block = m_pool.Alloc();

		//设置信息
		block->next = nullptr;
		block->ptr = ptr;
		block->size = size;
		block->alloc_time = GetTickCount64();
#ifdef EX_CFG_DEBUG_CALL_INFO
		if (file) { wcscpy_s(block->file, file); }
		block->line = line;
#endif // EX_CFG_DEBUG_CALL_INFO

		//链入链表头
		if (m_blocks) { block->next = m_blocks; }
		m_blocks = block;
	}
	void ExMemLeakChecker::Remove(const void* ptr)
	{
		if (ptr == nullptr) { return; }
		_Locked(m_lock);

		//遍历链表
		ExMemBlock* block = m_blocks;
		ExMemBlock* prev = nullptr;
		while (block) {

			//找到了
			if (block->ptr == ptr) {

				//从链表中移除
				if (prev) { prev->next = block->next; }
				else { m_blocks = block->next; }

				//释放内存
				m_pool.Free(block);
				break;
			}

			//继续遍历
			prev = block;
			block = block->next;
		}
	}
	void ExMemLeakChecker::Modify(const void* old_ptr, const void* new_ptr, size_t new_size, LPCWSTR file, int line)
	{
		if (old_ptr == nullptr) { return; }
		if (new_ptr == nullptr) { Remove(old_ptr); return; }
		_Locked(m_lock);

		//遍历链表
		ExMemBlock* block = m_blocks;
		while (block) {

			//找到了
			if (block->ptr == old_ptr) {

				//修改信息
				block->ptr = new_ptr;
				block->size = new_size;
				block->alloc_time = GetTickCount64();
#ifdef EX_CFG_DEBUG_CALL_INFO
				if (file) { wcscpy_s(block->file, file); }
				block->line = line;
#endif // EX_CFG_DEBUG_CALL_INFO
			}

			//继续遍历
			block = block->next;
		}
	}
	void ExMemLeakChecker::DumpLeaks()
	{
		_Locked(m_lock);

		//遍历链表
		ExMemBlock* block = m_blocks;
		uint64_t now = GetTickCount64();
		while (block) {

			//输出信息
			//Ex_Log(L"Memory leak detected: %p, %d bytes, allocated at %s(%d), %d ms ago", 
			//	block->ptr, block->size, block->file, block->line, now - block->alloc_time);

			//继续遍历
			block = block->next;
		}
	}

	LPVOID EXAPI EXCALL ExMemAllocD(size_t size, LPCWSTR file, int line)
	{
		LPVOID ptr = ExMemAlloc(size);
		ExMemLeakChecker::Instance()->Add(ptr, size, file, line);
		return ptr;
	}

	LPVOID EXAPI EXCALL ExMemReAllocD(LPVOID ptr, size_t new_size, LPCWSTR file, int line)
	{
		LPVOID new_ptr = ExMemReAlloc(ptr, new_size);
		ExMemLeakChecker::Instance()->Modify(ptr, new_ptr, new_size, file, line);
		return new_ptr;
	}

	void EXAPI EXCALL ExMemFreeD(LPVOID ptr)
	{
		ExMemLeakChecker::Instance()->Remove(ptr);
		ExMemFree(ptr);
	}

#endif // EX_CFG_DEBUG_MEMORY_LEAK

}


