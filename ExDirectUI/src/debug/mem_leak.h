/**
 * @file mem_leak.h
 * @brief 内存泄漏检查内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once
#include "common/utils/singleton.hpp"
#include "common/utils/mem_pool.hpp"
#include "common/utils/lock.hpp"

namespace ExDirectUI
{
#ifdef EX_CFG_DEBUG_MEMORY_LEAK

	class ExMemLeakChecker : public ExSingleton<ExMemLeakChecker>
	{
	public:
		ExMemLeakChecker();
		~ExMemLeakChecker();

		void Add(const void* ptr, size_t size, LPCWSTR file, int line);
		void Remove(const void* ptr);
		void Modify(const void* old_ptr, const void* new_ptr, size_t new_size, LPCWSTR file, int line);
		void DumpLeaks();

	private:

		struct ExMemBlock
		{
			ExMemBlock* next;
			const void* ptr;
			size_t size;
			uint64_t alloc_time;
#ifdef EX_CFG_DEBUG_CALL_INFO
			wchar_t file[MAX_PATH];
			int line;
#endif // EX_CFG_DEBUG_CALL_INFO
		};

		ExMemBlock* m_blocks{};
		ExMemPool<ExMemBlock> m_pool{};
		ExLock m_lock{};
	};

#endif // EX_CFG_DEBUG_MEMORY_LEAK
}
