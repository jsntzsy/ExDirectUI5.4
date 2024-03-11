/**
 * @file module.h
 * @brief 模块管理头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 模块文件加载器
	struct ExModuleFileLoader
	{
		HMODULE(WINAPI* LoadModule)(const LPVOID data, WPARAM waram, LPARAM lparam);
		bool(WINAPI* FreeModule)(HMODULE module_handle);
		FARPROC(WINAPI* GetProcAddress)(HMODULE module_handle, LPCSTR proc_name);
	};
	
	/// 模块类型
	enum ExModuleType
	{
		EX_MODULE_UNKNOWN,		///< 模块类型：未知
		
	};

	/// 模块信息
	struct ExModuleInfo
	{
		/// 模块ID
		EXATOM id;
		
		/// 模块类型
		uint16_t type;

		/// 模块标志
		uint16_t flags;

		/// 模块名称
		wchar_t name[64];
		
		/// 模块版本
		wchar_t version[32];
		
		/// 模块作者
		wchar_t author[32];

		/// 模块描述
		wchar_t description[256];
	};
	
	class IExModuleUtils;
	class IExModule;

	/// 模块入口函数类型
	typedef HRESULT(APIENTRY* ExModuleEntryProc)(HMODULE module_handle, 
		const IExModuleUtils* module_utils, IExModule** r_module);
	
	////////////////////////////

	
	
}
