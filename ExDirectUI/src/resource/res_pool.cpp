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
#include "common/object_impl.hpp"

#include "common/mem_pool.hpp"
#include "common/lock.hpp"
#include "common/auto_ptr.hpp"
#include "common/string.hpp"

namespace ExDirectUI
{
	class ExResPool : public ExObjectImpl<IExResPool>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IExResPool);
		EX_INTERFACE_ENTRY(IUnknown);
		EX_END_INTERFACE_MAP();

	public:
		ExResPool(size_t item_size, ExResPoolInitItemProc init_item_proc, ExResPoolFreeItemProc free_item_proc)
			: m_pool(sizeof(ExResPoolItemHeader) + item_size),
			m_init_item_proc(init_item_proc), m_free_item_proc(free_item_proc)
		{
		}

		virtual ~ExResPool()
		{
			if (m_free_item_proc) {
				for (auto& item : m_res) {
					m_free_item_proc(this, item.first, item.second->flags, item.second + 1);
				}
			}
			m_res.clear();
			m_pool.Clear();
		}
		EXMETHOD uint32_t EXOBJCALL GetItemCount() const override
		{
			_Locked(m_lock);
			return (uint32_t)m_res.size();
		}

		EXMETHOD bool EXOBJCALL HasItem(EXATOM key) const override
		{
			_Locked(m_lock);
			return m_res.find(key) != m_res.end();
		}

		EXMETHOD HRESULT EXOBJCALL FindKeyByPtr(void* res, EXATOM* r_key) const override
		{
			CHECK_PARAM(res);
			CHECK_PARAM(r_key);

			//获得头部
			ExResPoolItemHeader* header = (ExResPoolItemHeader*)res;
			header--;

			//如果是可读区域,则尝试通过key找下
			if (!::IsBadReadPtr(header, sizeof(ExResPoolItemHeader))) {
				auto it = m_res.find(header->key);

				//找到了
				if (it != m_res.end()) {
					*r_key = it->first;
					return S_OK;
				}
			}

			//其他情况就只能遍历表了
			for (auto it = m_res.begin(); it != m_res.end(); ++it)
			{
				if (it->second == header) {
					*r_key = it->first;
					return S_OK;
				}
			}

			handle_ex(EE_NOEXISTS, L"该项目不存在");
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
				header->key = key;

				// 如果有初始化回调，则调用
				if (m_init_item_proc) {
					throw_if_notok(
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
			catch_ignore({ if (header) { m_pool.Free(header); } return e.handle(true); });
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

			return _UnUseItemByIter(it);
		}

		EXMETHOD HRESULT EXOBJCALL UnUseItemByPtr(void* res) override
		{
			CHECK_PARAM(res);

			//获得头部
			ExResPoolItemHeader* header = (ExResPoolItemHeader*)res;
			header--;

			//如果是可读区域,则尝试通过key找下
			if (!::IsBadReadPtr(header, sizeof(ExResPoolItemHeader))) {
				auto it = m_res.find(header->key);

				//找到了
				if (it != m_res.end()) {
					if (SUCCEEDED(_UnUseItemByIter(it))) { return S_OK; }
				}
			}

			//其他情况就只能遍历表了
			for (auto it = m_res.begin(); it != m_res.end(); ++it)
			{
				if (it->second == header) { return _UnUseItemByIter(it); }
			}

			handle_ex(EE_NOEXISTS, L"该项目不存在");
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
			return ExString::format(L"ExResPool(item_size: %u, count: %u)",
				m_pool.GetBlockSize() - sizeof(ExResPoolItemHeader), m_res.size()
			);
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

		struct ExResPoolItemHeader;

		HRESULT _UnUseItemByIter(std::unordered_map<EXATOM, ExResPoolItemHeader*>::iterator& it)
		{
			auto header = it->second;
			try
			{
				// 减少引用计数,如果引用计数为0
				if (--header->ref_count == 0) {

					// 如果不是永久项目，则释放
					if ((header->flags & ExResPoolItemFlags::Eternal) == 0) {

						// 如果有释放回调，则调用
						if (m_free_item_proc) {
							throw_if_failed(
								m_free_item_proc(this, header->key, header->flags, header + 1),
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

		struct ExResPoolItemHeader
		{
			DWORD flags;
			int32_t ref_count;
			EXATOM key;
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
		CHECK_PARAM(item_size > 0);
		CHECK_PARAM(r_pool);

		try
		{
			ExAutoPtr<ExResPool> pool = new ExResPool(item_size, init_item_proc, free_item_proc);
			return pool->QueryInterface(r_pool);
		}
		catch_default({});
	}

}
