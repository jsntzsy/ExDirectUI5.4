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
#include "interface/module.i.h"
#include "common/utils/auto_ptr.hpp"

namespace ExDirectUI
{

	HMODULE WINAPI _ExModule_LoadModule(const LPVOID data, WPARAM waram, LPARAM lparam)
	{
		return LoadLibraryW((LPCWSTR)data);
	}
	bool WINAPI _ExModule_FreeModule(HMODULE module_handle)
	{
		return FreeLibrary(module_handle);
	}
	FARPROC WINAPI _ExModule_GetProcAddress(HMODULE module_handle, LPCSTR proc_name)
	{
		return GetProcAddress(module_handle, proc_name);
	}

	ExModuleFileLoader g_module_loader = {
		_ExModule_LoadModule,
		_ExModule_FreeModule,
		_ExModule_GetProcAddress
	};

	///////////////

	struct ExModuleContext
	{
		HMODULE module_handle;
		IExModule* module_instance;
	};

	std::unordered_map<EXATOM, ExModuleContext> g_modules;

	void EXCALL _ExModule_LoadFromEntry(HMODULE module_handle, ExModuleEntryProc entry)
	{
		try
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
			//------------------------------------------------

			// 加入模块表
			g_modules[info.id] = { module_handle, module_instance.Detach() };
		}
		catch_continue({});
	}

	HRESULT EXCALL _ExModule_UnLoad(EXATOM id)
	{
		auto it = g_modules.find(id);
		handle_if_false(it != g_modules.end(), EE_NOEXISTS, L"模块未找到");
		
		// 模块取消分组
		//------------------------------------------------

		// 释放模块实例
		SAFE_RELEASE(it->second.module_instance);
		
		// 释放模块句柄
		SAFE_FREE(it->second.module_handle, g_module_loader.FreeModule);
		
		// 从模块表中移除
		g_modules.erase(it);

		return S_OK;
	}

	/////////////////////////







}


