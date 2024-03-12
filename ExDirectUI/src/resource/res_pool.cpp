/**
 * @file res_pool.cpp
 * @brief 资源池对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */

#include "stdafx.h"
#include "resource/res_pool.h"
#include "common/interfaces/object_impl.hpp"

#include "common/utils/mem_pool.hpp"
#include "common/utils/lock.hpp"
#include "common/utils/auto_ptr.hpp"

namespace ExDirectUI
{
	class CExResPool : public ExObjectBaseImpl<IExResPool>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IExResPool);
		EX_INTERFACE_ENTRY(IUnknown);
		EX_END_INTERFACE_MAP();

	public:
		CExResPool(size_t item_size, ExResPoolInitItemProc init_item_proc, ExResPoolFreeItemProc free_item_proc)
			: m_pool(sizeof(ExResPoolItemHeader) + item_size),
			m_init_item_proc(init_item_proc), m_free_item_proc(free_item_proc)
		{
		}

		virtual ~CExResPool()
		{
			if (m_free_item_proc) {
				for (auto& item : m_res) {
					m_free_item_proc(this, item.first, item.second->flags, item.second + 1);
				}
			}
			m_res.clear();
			m_pool.Clear();
		}

		EXMETHOD HRESULT EXOBJCALL UseOrCreateItem(EXATOM key, const void* data,
			WPARAM wparam, LPARAM lparam, DWORD flags, void** r_res) noexcept override
		{
			_Locked(m_lock);

			//先找找看
			auto it = m_res.find(key);

			//如果找到了就直接使用
			if (it != m_res.end()) {
				auto header = it->second;
				header->ref_count++;
				if (r_res) { *r_res = header + 1; }
				return S_OK;
			}

			ExResPoolItemHeader* header = nullptr;
			try
			{
				//如果没找到，就创建一个
				header = m_pool.Alloc<ExResPoolItemHeader>();

				//设置参数
				header->flags = flags;
				header->ref_count = 1;
				header->atom = key;

				// 如果有初始化回调，则调用
				if (m_init_item_proc) {
					throw_if_failed(
						m_init_item_proc(this, key, data, wparam, lparam, flags, header + 1),
						L"初始化项目失败"
					);
				}

				//加入资源表
				m_res[key] = header;

				//返回
				if (r_res) { *r_res = header + 1; }
				return S_OK;
			}
			//如果创建失败，就释放内存
			catch_default({ if (header) { m_pool.Free(header); } });
		}

		EXMETHOD HRESULT EXOBJCALL UseItem(EXATOM key, void** r_res) override
		{
			_Locked(m_lock);

			auto it = m_res.find(key);
			handle_if_false(it != m_res.end(), EE_NOEXISTS, L"该项目不存在");

			auto header = it->second;
			header->ref_count++;
			if (r_res) { *r_res = header + 1; }

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL UnUseItem(EXATOM key) override
		{
			_Locked(m_lock);

			auto it = m_res.find(key);
			handle_if_false(it != m_res.end(), EE_NOEXISTS, L"该项目不存在");

			auto header = it->second;
			try
			{
				// 减少引用计数,如果引用计数为0
				if (--header->ref_count == 0) {

					// 如果不是永久项目，则释放
					if ((header->flags & EX_RESPOOL_ITEM_ETERNAL) == 0) {

						// 如果有释放回调，则调用
						if (m_free_item_proc) {
							throw_if_failed(
								m_free_item_proc(this, key, header->flags, header + 1),
								L"释放项目失败"
							);
						}

						// 从资源表和内存池中移除
						m_res.erase(it);
						m_pool.Free(header);
					}
				}

				return S_OK;
			}
			//如果发生异常，就恢复引用计数
			catch_default({ header->ref_count++; });
		}

		EXMETHOD HRESULT EXOBJCALL EnumItems(ExResPoolEnumItemProc enum_proc, LPARAM lparam) override
		{
			CHECK_PARAM(enum_proc);
			_Locked(m_lock);

			for (auto& item : m_res) {
				if (!enum_proc(this, item.first, item.second->flags, item.second + 1, lparam)) {
					return S_FALSE;
				}
			}

			return S_OK;
		}

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			std::wstringstream ss;
			ss << L"ExResPool: " "item_size: " << m_pool.GetBlockSize() - sizeof(ExResPoolItemHeader)
				<< L", item_count: " << m_res.size();
			return ss.str();
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return (void*)&m_res;
			case 1: return (void*)&m_pool;
			case 2: return (void*)m_init_item_proc;
			case 3: return (void*)m_free_item_proc;
			default: return __super::GetContext(index);
			}
		}

	private:
		struct ExResPoolItemHeader
		{
			DWORD flags;
			int32_t ref_count;
			EXATOM atom;
		};

		ExDynamicMemPool m_pool;
		ExLock m_lock;
		ExResPoolInitItemProc m_init_item_proc;
		ExResPoolFreeItemProc m_free_item_proc;
		std::unordered_map<EXATOM, ExResPoolItemHeader*> m_res;
	};

	////////////////////////


	HRESULT EXAPI EXCALL ExResPoolCreate(size_t item_size, ExResPoolInitItemProc init_item_proc,
		ExResPoolFreeItemProc free_item_proc, IExResPool** r_pool)
	{
		CHECK_PARAM(r_pool);

		try
		{
			ExAutoPtr<CExResPool> pool(NEW CExResPool(item_size, init_item_proc, free_item_proc));
			return pool->QueryInterface(r_pool);
		}
		catch_default({});
	}

}
