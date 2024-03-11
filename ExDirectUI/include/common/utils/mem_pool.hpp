/**
 * @file mem_pool.hpp
 * @brief 内存池功能实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once
#include <Windows.h>
//#include "kernel/memory.h"
#include "lock.hpp"

namespace ExDirectUI
{
	//REMARK:这里不可以使用exmalloc/ExMemAllocD等函数及new,否则会无限递归导致栈溢出

	/// 静态内存池
	template<typename T, size_t BLOCK_NUM = 16>
	class ExMemPool
	{
	public: 
		ExMemPool() :m_blocks_free(nullptr), m_chunks(nullptr)
		{
		}

		~ExMemPool()
		{
			Clear();
		}

		T* Alloc() 
		{
			_Locked(m_lock);

			// 没有空闲块了,则申请新组
			if (m_blocks_free == nullptr)
			{
				//申请新组
				ExMemChunkInfo* chunk_new = (ExMemChunkInfo*)ExMemAlloc(sizeof(ExMemChunkInfo));
				//throw_if_false(chunk_new, ES_MEM_ALLOC_FAILED, L"内存组分配失败");
				//if (!chunk_new)
				//{
				//	ExDbgThrow(ES_MEM_ALLOC_FAILED, __CALLINFO__);					
				//	return nullptr;
				//}

				//链入申请的内存组链表
				chunk_new->next = nullptr;
				if (m_chunks) m_chunks->next = chunk_new;
				m_chunks = chunk_new;

				//把新组的内存块链入空闲块链表
				m_blocks_free = &(chunk_new->blocks[0]);
				for (size_t i = 1; i < BLOCK_NUM; i++)
					chunk_new->blocks[i - 1].next = &(chunk_new->blocks[i]);
				chunk_new->blocks[BLOCK_NUM - 1].next = nullptr;

			}

			//从空闲块链表中取出一个空闲块，并将链表头后移
			ExMemBlockInfo* pBlock = m_blocks_free;
			m_blocks_free = m_blocks_free->next;
			T* item_new = (T*)pBlock;

			return item_new;
		}

		void Free(T* ptr)
		{
			_Locked(m_lock);

			ExMemBlockInfo* block = (ExMemBlockInfo*)ptr;

			//将释放的内存块链入空闲块链表
			block->next = m_blocks_free;
			m_blocks_free = block;

		}

		void Clear()
		{
			_Locked(m_lock);

			ExMemChunkInfo* chunk;
			while (m_chunks)
			{
				chunk = m_chunks;
				m_chunks = m_chunks->next;
				ExMemFree(chunk);
			}

			m_blocks_free = nullptr;
			m_chunks = nullptr;

		}

	private:

		// 内存块信息
		struct ExMemBlockInfo
		{
			BYTE data[sizeof(T)];
			ExMemBlockInfo* next;
		};

		// 内存组信息
		struct ExMemChunkInfo
		{
			ExMemBlockInfo blocks[BLOCK_NUM];
			ExMemChunkInfo* next;
		};

		// 空闲块链表头
		ExMemBlockInfo* m_blocks_free;

		// 申请的内存组链表头
		ExMemChunkInfo* m_chunks;

		//线程锁
		ExLock m_lock;

	};


	/// 动态内存池
	class ExDynamicMemPool
	{
	public:

		ExDynamicMemPool(size_t block_size, size_t block_count = 16)
			:m_block_size(block_size), m_block_count(block_count)
		{
			m_block_head_free = nullptr;
			m_chunk_head = nullptr;
		}

		~ExDynamicMemPool()
		{
			Clear();
		}

		LPVOID Alloc() 
		{
			_Locked(m_lock);

			// 如果没有空块,则申请新组
			if (m_block_head_free == nullptr)
			{
				//申请新组
				ExMemChunkHeader* chunk_new = (ExMemChunkHeader*)ExMemAlloc(sizeof(ExMemChunkHeader) +
					(sizeof(ExMemBlockHeader) + m_block_size) * m_block_count);
				//throw_if_false(chunk_new, ES_MEM_ALLOC_FAILED, L"内存组分配失败");

				chunk_new->next = nullptr;

				// 设置当前空块头为新组的第一个快
				m_block_head_free = (ExMemBlockHeader*)(chunk_new + 1);
				BYTE* block_ptr = (BYTE*)m_block_head_free;
				ExMemBlockHeader* block = nullptr;

				// 链上申请的所有块
				for (size_t i = 1; i < m_block_count; i++)
				{
					block = (ExMemBlockHeader*)block_ptr;
					block_ptr += sizeof(ExMemBlockHeader) + m_block_size;
					block->next = (ExMemBlockHeader*)block_ptr;
				}

				//最后一块的下一个为空
				block->next = nullptr;

				//连上当前的组,并设置组头为新申请的
				if (m_chunk_head)
					m_chunk_head->next = chunk_new;
				m_chunk_head = chunk_new;
			}

			// 将当前空闲头作为申请到的内存,并将当前空闲头后移
			LPVOID block_alloced = m_block_head_free + 1;
			m_block_head_free = m_block_head_free->next;

			return block_alloced;
		}

		void Free(LPVOID p)
		{
			_Locked(m_lock);
			ExMemBlockHeader* block = ((ExMemBlockHeader*)p) - 1;

			//将此块加入空块头
			block->next = m_block_head_free;
			m_block_head_free = block;
		}

		void Clear()
		{
			_Locked(m_lock);

			//遍历所有组,释放
			ExMemChunkHeader* chunk;
			while (m_chunk_head)
			{
				chunk = m_chunk_head->next;
				ExMemFree(m_chunk_head);
				m_chunk_head = chunk;
			}

			m_block_head_free = nullptr;
			m_chunk_head = nullptr;

		}

		template<typename T>
		inline T* Alloc() { return (T*)Alloc(); }

	private:

		// 内存块头
		struct ExMemBlockHeader
		{
			ExMemBlockHeader* next;
		};

		// 内存组头
		struct ExMemChunkHeader
		{
			ExMemChunkHeader* next;
		};

		// 分配内存块尺寸
		size_t m_block_size;

		// 单次分配内存块数量
		size_t m_block_count;

		// 空闲块链表头
		ExMemBlockHeader* m_block_head_free;

		// 申请的内存组链表头
		ExMemChunkHeader* m_chunk_head;

		// 锁
		ExLock m_lock;

	};





}
