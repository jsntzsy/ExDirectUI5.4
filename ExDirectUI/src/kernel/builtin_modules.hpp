/**
 * @file builtin_modules.hpp
 * @brief 内置模块加载头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */
#pragma once
#include "builtin_modules_define.h"
#include "include/kernel/module.h"

#define EX_DECLEAR_BUILTIN_MODULE_ENTRY(EntryName, File) \
	HRESULT APIENTRY EntryName(HMODULE module_handle, IExModuleUtils* utils, \
	ExModuleInfo* r_info, IExModule** r_module);\
	__pragma(comment(lib, File))


namespace ExDirectUI
{
	void EXCALL _ExModule_LoadFromEntry(HMODULE module_handle, ExModuleEntryProc entry) MAYTHROW;

#pragma region 内置模块声明

#ifdef EX_BUILT_MODULE_IMAGEDECODER_1_ENTRY
	EX_DECLEAR_BUILTIN_MODULE_ENTRY(EX_BUILT_MODULE_IMAGEDECODER_1_ENTRY,
		EX_BUILT_MODULE_IMAGEDECODER_1_FILE);
#endif

#ifdef EX_BUILT_MODULE_IMAGEDECODER_2_ENTRY
	EX_DECLEAR_BUILTIN_MODULE_ENTRY(EX_BUILT_MODULE_IMAGEDECODER_2_ENTRY,
		EX_BUILT_MODULE_IMAGEDECODER_2_FILE);
#endif

#ifdef EX_BUILT_MODULE_IMAGEDECODER_3_ENTRY
	EX_DECLEAR_BUILTIN_MODULE_ENTRY(EX_BUILT_MODULE_IMAGEDECODER_3_ENTRY,
		EX_BUILT_MODULE_IMAGEDECODER_3_FILE);
#endif

#ifdef EX_BUILT_MODULE_RENDER_ENTRY
	EX_DECLEAR_BUILTIN_MODULE_ENTRY(EX_BUILT_MODULE_RENDER_ENTRY,
		EX_BUILT_MODULE_RENDER_FILE);
#endif

#pragma endregion

	// 加载内置模块
	void _ExModule_LoadBuiltinModules(bool preload) {

#ifdef EX_CFG_MODULE_ENABLE_BUILTIN

		struct _ExBuiltinModule_
		{
			ExModuleEntryProc entry;
			bool preload;
		};

#pragma region 内置模块信息表声明

		_ExBuiltinModule_ modules[] = {
#ifdef EX_BUILT_MODULE_IMAGEDECODER_1_ENTRY
			{ EX_BUILT_MODULE_IMAGEDECODER_1_ENTRY, EX_BUILT_MODULE_IMAGEDECODER_1_PRELOAD },
#endif
#ifdef EX_BUILT_MODULE_IMAGEDECODER_2_ENTRY
			{ EX_BUILT_MODULE_IMAGEDECODER_2_ENTRY, EX_BUILT_MODULE_IMAGEDECODER_2_PRELOAD },
#endif
#ifdef EX_BUILT_MODULE_IMAGEDECODER_3_ENTRY
			{ EX_BUILT_MODULE_IMAGEDECODER_3_ENTRY, EX_BUILT_MODULE_IMAGEDECODER_3_PRELOAD },
#endif
#ifdef EX_BUILT_MODULE_RENDER_ENTRY
			{ EX_BUILT_MODULE_RENDER_ENTRY, EX_BUILT_MODULE_RENDER_PRELOAD },
#endif
			{nullptr,false}
		};

#pragma endregion

		for (auto& m : modules) {
			if (m.entry && m.preload == preload) {
				try
				{
					_ExModule_LoadFromEntry(nullptr, m.entry);
				}
				catch_ignore({ e.handle(true); });
			}
		}

#endif // EX_CFG_MODULE_ENABLE_BUILTIN
	}
}


