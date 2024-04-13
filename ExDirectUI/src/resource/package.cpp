/**
 * @file package.cpp
 * @brief 数据包实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-28
 * @copyright
 */

#include "stdafx.h"
#include "resource/package.h"
#include "src/resource/package_processor.hpp"
#include "common/object_impl.hpp"
#include "common/string.hpp"

namespace ExDirectUI
{
	class ExPackage : public ExObjectImpl<IExPackage>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExPackage);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExPackage(item_count: %zu)", m_items.size());
		}

		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return (void*)&m_items;
			case 1: return (void*)&m_info;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExPackage(const byte_t* data, uint32_t data_size, const byte_t* key, uint32_t key_size)
		{
			throw_if_failed(ExPackageProcessor::Load(
				data, data_size, key, key_size,
				&m_info, m_items),
				L"加载数据包失败"
			);
		}

	public:

		EXMETHOD HRESULT EXOBJCALL GetPackageInfo(ExPackageInfo* r_info) const override
		{
			CHECK_PARAM(r_info);
			*r_info = m_info;
			return S_OK;
		}
		EXMETHOD uint32_t EXOBJCALL GetItemCount() const override
		{
			return (uint32_t)m_items.size();
		}

		EXMETHOD bool EXOBJCALL HasItem(EXATOM id) const override
		{
			return m_items.find(id) != m_items.end();
		}
		EXMETHOD EXATOM EXOBJCALL FindItem(LPCWSTR path) const override
		{
			CHECK_PARAM(path);

			//先使用路径原子号找一遍
			EXATOM atom = ExAtom(path);
			if (m_items.find(atom) != m_items.end()) { return atom; }

			//如果没找到,则遍历查找相同路径的项目(不区分大小写)
			for (auto& item : m_items) {
				if (_wcsicmp(item.second.path, path) == 0) {
					return item.first;
				}
			}

			return EXATOM_UNKNOWN;
		}

		EXMETHOD HRESULT EXOBJCALL GetItemInfo(EXATOM id, ExPackageItemInfo* r_info) const override
		{
			CHECK_PARAM(r_info);

			//找到项目
			auto it = m_items.find(id);
			handle_if_false(it != m_items.end(), EE_NOEXISTS, L"该ID项目不存在");

			//设置返回值
			*r_info = it->second;
			return S_OK;
		}
		EXMETHOD HRESULT EXOBJCALL GetItemData(EXATOM id, const byte_t** r_data, uint32_t* r_size) const override
		{
			CHECK_PARAM(r_data);

			//找到项目
			auto it = m_items.find(id);
			handle_if_false(it != m_items.end(), EE_NOEXISTS, L"该ID项目不存在");

			//设置返回值
			*r_data = it->second.data;
			if (r_size) { *r_size = (uint32_t)it->second.size; }
			return S_OK;
		}
		EXMETHOD LPCWSTR EXOBJCALL GetItemPath(EXATOM id) const override
		{
			//找到项目
			auto it = m_items.find(id);
			if (it == m_items.end()) {
				handle_continue(EE_NOEXISTS, L"该ID项目不存在");
				return nullptr;
			}

			return it->second.path;
		}

		EXMETHOD HRESULT EXOBJCALL EnumItems(ExPackageItemEnumProc proc, LPARAM lparam) const override
		{
			CHECK_PARAM(proc);

			//遍历项目
			for (auto& item : m_items) {

				//调用回调,如果返回true则停止遍历
				if (proc(this, item.first, &item.second, lparam)) {
					return S_FALSE;
				}
			}
			return S_OK;
		}

	private:
		ExPackageInfo m_info;
		std::unordered_map<EXATOM, ExPackageItemInfo> m_items;
	};


	////////////////////////////////

	HRESULT EXAPI EXCALL ExPackageBuild(const ExPackageInfo* package_info, const ExPackageItemInfo* items,
		uint32_t item_count, const byte_t* key, uint32_t key_size, ExData* r_data)
	{
		CHECK_PARAM(package_info);
		CHECK_PARAM(r_data);

		try
		{
			return ExPackageProcessor::Build(package_info, items, item_count, 
				key, key_size, false, 0, r_data
			);
		}
		catch_default({});
	}

	HRESULT EXAPI EXCALL ExPackageBuildFromDirectory(const ExPackageInfo* package_info, LPCWSTR directory,
		LPCWSTR wildcard, DWORD build_flags, ExPackageBuildFilterProc filter_proc, LPARAM lparam,
		const byte_t* key, uint32_t key_size, ExData* r_data)
	{
		CHECK_PARAM(package_info);
		CHECK_PARAM(directory);
		CHECK_PARAM(r_data);

		try
		{
			return ExPackageProcessor::BuildFromDirectory(package_info, directory, wildcard, build_flags,
				filter_proc, lparam, key, key_size, r_data
			);
		}
		catch_default({});
	}

	HRESULT EXAPI EXCALL ExPackageLoad(const byte_t* package_data, uint32_t package_size,
		const byte_t* key, uint32_t key_size, IExPackage** r_package)
	{
		CHECK_PARAM(package_data);
		CHECK_PARAM(r_package);

		try
		{
			ExAutoPtr<ExPackage> package = new ExPackage(package_data, package_size, key, key_size);
			return package->QueryInterface(r_package);
		}
		catch_default({});
	}

}
