/**
 * @file module.cpp
 * @brief 模块管理实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "kernel/module.h"
#include "interface/module.i.h"
#include "src/kernel/module.h"
#include "src/kernel/module_utils.h"
#include "common/utils/auto_ptr.hpp"


namespace ExDirectUI
{
	struct ExModuleFileLoaderDefault : ExModuleFileLoader
	{
		static HMODULE WINAPI LoadModule(const LPVOID file, WPARAM waram, LPARAM lparam)
		{
			return LoadLibraryW((LPCWSTR)file);
		}
		static bool WINAPI FreeModule(HMODULE module_handle)
		{
			return FreeLibrary(module_handle);
		}
		static FARPROC WINAPI GetProcAddress(HMODULE module_handle, LPCSTR proc_name)
		{
			return GetProcAddress(module_handle, proc_name);
		}

		ExModuleFileLoaderDefault()
		{
			ExModuleFileLoader::LoadModule = LoadModule;
			ExModuleFileLoader::FreeModule = FreeModule;
			ExModuleFileLoader::GetProcAddress = GetProcAddress;
		}
	};

	ExModuleFileLoader g_module_loader = ExModuleFileLoaderDefault();

	///////////////

	struct ExModuleContext
	{
		HMODULE module_handle;
		IExModule* module_instance;
	};

	std::unordered_map<EXATOM, ExModuleContext> g_modules;

	void EXCALL _ExModule_LoadFromEntry(HMODULE module_handle, ExModuleEntryProc entry) MAYTHROW
	{
		//调用模块入口,获取模块实例
		ExAutoPtr<IExModule> module_instance;
		throw_if_failed(
			entry(module_handle, nullptr, &module_instance),
			L"模块入口函数执行失败"
		);

		// 获取模块信息
		ExModuleInfo info{};
		module_instance->GetInfo(&info);

		// 检查模块ID是否已存在
		auto it = g_modules.find(info.id);
		throw_if_false(it == g_modules.end(), EE_EXISTS, L"模块ID冲突");

		// 模块分组
		ExModuleUtils::Group(&info, module_instance);

		// 加入模块表
		g_modules[info.id] = { module_handle, module_instance.Detach() };

	}

	HRESULT EXCALL _ExModule_UnLoad(EXATOM id)
	{
		auto it = g_modules.find(id);
		handle_if_false(it != g_modules.end(), EE_NOEXISTS, L"模块未找到");

		// 模块取消分组
		ExModuleUtils::UnGroup(it->second.module_instance);

		// 释放模块实例
		SAFE_RELEASE(it->second.module_instance);

		// 释放模块句柄
		SAFE_FREE(it->second.module_handle, g_module_loader.FreeModule);

		// 从模块表中移除
		g_modules.erase(it);

		return S_OK;
	}

	/////////////////////////

	HRESULT EXAPI EXCALL ExModuleLoadFromFile(const LPVOID file, WPARAM waram, LPARAM lparam, EXATOM* r_module_id)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_module_id);

		// 加载模块
		HMODULE module_handle = g_module_loader.LoadModule(file, waram, lparam);
		handle_if_false(module_handle, EE_LOAD, L"模块加载失败");

		try
		{
			// 获取模块入口
			ExModuleEntryProc entry = (ExModuleEntryProc)g_module_loader.GetProcAddress(
				module_handle, "ExModuleEntry");
			throw_if_false(entry, EE_NOEXISTS, L"模块入口函数未找到");

			// 加载模块
			_ExModule_LoadFromEntry(module_handle, entry);
			return S_OK;
		}
		catch_default(g_module_loader.FreeModule(module_handle));
	}
	HRESULT EXAPI EXCALL ExModuleLoadFromHandle(HMODULE module_handle, EXATOM* r_module_id)
	{
		CHECK_PARAM(module_handle);
		CHECK_PARAM(r_module_id);

		try
		{
			// 获取模块入口
			ExModuleEntryProc entry = (ExModuleEntryProc)g_module_loader.GetProcAddress(
				module_handle, "ExModuleEntry");
			throw_if_false(entry, EE_NOEXISTS, L"模块入口函数未找到");

			// 加载模块
			_ExModule_LoadFromEntry(module_handle, entry);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXAPI EXCALL ExModuleLoadFromEntry(ExModuleEntryProc entry_proc, EXATOM* r_module_id)
	{
		CHECK_PARAM(entry_proc);
		CHECK_PARAM(r_module_id);

		try
		{
			// 加载模块
			_ExModule_LoadFromEntry(NULL, entry_proc);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXAPI EXCALL ExModuleUnload(EXATOM module_id)
	{
		return _ExModule_UnLoad(module_id);
	}
	HRESULT EXAPI EXCALL ExModuleGetInfo(EXATOM module_id, ExModuleInfo* r_info)
	{
		CHECK_PARAM(r_info);

		auto it = g_modules.find(module_id);
		handle_if_false(it != g_modules.end(), EE_NOEXISTS, L"模块未找到");

		it->second.module_instance->GetInfo(r_info);
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExModuleInvoke(EXATOM module_id, uint32_t msg, WPARAM wparam, LPARAM lparam, LRESULT* r_result)
	{
		auto it = g_modules.find(module_id);
		handle_if_false(it != g_modules.end(), EE_NOEXISTS, L"模块未找到");

		return it->second.module_instance->Invoke(msg, wparam, lparam, r_result);
	}

}


