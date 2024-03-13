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
#include "src/kernel/module.h"
#include "src/kernel/module_utils.h"
#include "src/kernel/builtin_modules.hpp"
#include "common/utils/auto_ptr.hpp"


namespace ExDirectUI
{
	struct ExModuleFileLoaderDefault : ExModuleFileLoader
	{
		static HMODULE WINAPI LoadModule(const void* file, WPARAM waram, LPARAM lparam)
		{
			return ::LoadLibraryW((LPCWSTR)file);
		}
		static bool WINAPI FreeModule(HMODULE module_handle)
		{
			return ::FreeLibrary(module_handle);
		}
		static FARPROC WINAPI GetProcAddress(HMODULE module_handle, LPCSTR proc_name)
		{
			return ::GetProcAddress(module_handle, proc_name);
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
		ExModuleInfo info{};
		throw_if_failed(
			entry(module_handle, ExModuleUtils::Instance(), &info, &module_instance),
			L"模块入口函数执行失败"
		);

		// 检查模块ID是否已存在
		auto it = g_modules.find(info.id);
		throw_if_false(it == g_modules.end(), EE_EXISTS, L"模块ID冲突");

		// 模块分组 (如果抛出的是S_FALSE,则表示不需要报错但不需要了(比如加载了多个渲染器))
		throw_if_notok(
			ExModuleUtils::Group(module_instance->GetType(), module_instance),
			L"模块分组失败"
		);

		// 加入模块表
		g_modules[info.id] = { module_handle, module_instance.Detach() };

	}

	HRESULT EXCALL _ExModule_UnLoad(EXATOM id, bool is_uninit)
	{
		auto it = g_modules.find(id);
		handle_if_false(it != g_modules.end(), EE_NOEXISTS, L"模块未找到");

		IExModule* instance = it->second.module_instance;

		//判断模块是否可以卸载
		if (!is_uninit) {
			ExModuleInfo info{};
			instance->GetInfo(&info);
			handle_if_false(info.flags & EX_MODULE_FLAG_CAN_UNLOAD, E_NOTIMPL, L"模块不允许提前卸载");
		}

		// 模块取消分组
		handle_if_failed(
			ExModuleUtils::UnGroup(instance),
			L"模块取消分组失败"
		);
		
		//调用OnModuleUnLoad
		handle_if_failed(instance->OnModuleUnLoad(), L"模块卸载失败");

		// 释放模块实例
		SAFE_RELEASE(it->second.module_instance);

		// 释放模块句柄
		SAFE_FREE(it->second.module_handle, g_module_loader.FreeModule);

		// 从模块表中移除 (如果是反初始化,则直接由外部统一清除)
		if (!is_uninit) { g_modules.erase(it); }

		return S_OK;
	}

	//////////////////////

	HRESULT EXCALL _ExModule_Init(const ExEngineInitInfo* init_info)
	{
		//设置模块加载器
		if (init_info->module_loader) { g_module_loader = *init_info->module_loader; }
		else { g_module_loader = ExModuleFileLoaderDefault(); }

		//加载内置模块（预先加载部分）
		_ExModule_LoadBuiltinModules(true);

		//加载初始化时传入的模块
		if (init_info->modules.unknown && init_info->module_count != 0) {
			
			EXATOM module_id = 0;
			HRESULT status = S_OK;
			
			for (uint32_t i = 0; i < init_info->module_count; ++i) {
				switch (init_info->module_load_mode) {
				case EX_ENGINE_MODULE_LOAD_FROM_HANDLE:
					status = ExModuleLoadFromHandle(
						init_info->modules.handles[i], &module_id
					);
					break;
				case EX_ENGINE_MODULE_LOAD_FROM_FILE:
					status = ExModuleLoadFromFile(
						init_info->modules.files[i].file,
						init_info->modules.files[i].wparam,
						init_info->modules.files[i].lparam,
						&module_id
					);
					break;
				case EX_ENGINE_MODULE_LOAD_FROM_FILE_PTR:
					status = ExModuleLoadFromFile(
						init_info->modules.files_ptr[i]->file,
						init_info->modules.files_ptr[i]->wparam,
						init_info->modules.files_ptr[i]->lparam,
						&module_id
					);
					break;
				case EX_ENGINE_MODULE_LOAD_FROM_ENTRY:
					status = ExModuleLoadFromEntry(
						init_info->modules.entries[i],
						&module_id
					);
					break;
				}
			}
		}

		//加载内置模块（最后加载部分）
		_ExModule_LoadBuiltinModules(false);

		//加载完成后检查必要模块
		return ExModuleUtils::Instance()->NessaryCheck();
	}

	void EXCALL _ExModule_UnInit()
	{
		//释放所有模块
		for (auto& it : g_modules) {
			_ExModule_UnLoad(it.first, true);
		}
		g_modules.clear();
	}


	/////////////////////////

	HRESULT EXAPI EXCALL ExModuleLoadFromFile(const void* file, WPARAM waram, LPARAM lparam, EXATOM* r_module_id)
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
				module_handle, "_ExDirectUI_Module_Entry_");
			throw_if_false(entry, EE_NOEXISTS, L"模块入口函数未找到");

			// 加载模块
			_ExModule_LoadFromEntry(module_handle, entry);
			return S_OK;
		}
		catch_default({
			g_module_loader.FreeModule(module_handle);
			return e.handle(true);
			});
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
		catch_default({ return e.handle(true); });
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
		catch_default({ return e.handle(true); });
	}
	HRESULT EXAPI EXCALL ExModuleUnload(EXATOM module_id)
	{
		return _ExModule_UnLoad(module_id, false);
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


