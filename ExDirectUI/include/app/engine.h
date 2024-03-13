/**
 * @file engine.h
 * @brief 引擎管理头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 引擎标识
	enum ExEngineFlags
	{
		EX_ENGINE_FLAG_NONE = 0x00000000,				///< 引擎标识：无
		EX_ENGINE_FLAG_DEBUG = 0x01000000,				///< 引擎标识：调试模式

		EX_ENGINE_FLAG_DEFULT = -1,						///< 引擎标识：默认
	};

	/// 模块加载模式
	enum ExModuleLoadFileMode
	{
		EX_ENGINE_MODULE_LOAD_FROM_HANDLE,				///< 模块加载：自句柄
		EX_ENGINE_MODULE_LOAD_FROM_FILE,				///< 模块加载：自文件信息
		EX_ENGINE_MODULE_LOAD_FROM_FILE_PTR,			///< 模块加载：自文件信息指针
		EX_ENGINE_MODULE_LOAD_FROM_ENTRY,				///< 模块加载：自入口函数
	};

	/// 模块加载文件集
	union ExModuleLoadFileSet
	{
		void* unknown;									///< 未知
		HMODULE* handles;								///< 模块句柄数组
		ExModuleFileInfo* files;						///< 模块文件信息数组
		ExModuleFileInfo** files_ptr;					///< 模块文件信息指针数组
		ExModuleEntryProc* entries;						///< 模块入口函数数组
	};


	/// 引擎初始化信息
	struct ExEngineInitInfo
	{
		/// 实例句柄
		HINSTANCE instance;

		/// 初始化标识
		DWORD flags;

		/// 模块加载模式
		ExModuleLoadFileMode module_load_mode;

		/// 模块数量
		uint32_t module_count;

		/// 模块加载文件集
		ExModuleLoadFileSet modules;

		/// 模块加载器 @可空(空则使用默认加载器)
		ExModuleFileLoader* module_loader;



	};


	////////////////

	/**
	 * @brief 引擎_初始化
	 * 初始化ExDirectUI引擎
	 * @param init_info 初始化信息
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExEngineInit(const ExEngineInitInfo* init_info);

	/**
	 * @brief 引擎_反初始化
	 * 反初始化ExDirectUI引擎
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExEngineUnInit();

	/**
	 * @brief 引擎_查询标识
	 * 查询引擎当前是否包含指定标识
	 * @param flag 引擎标识
	 * @return 返回是否包含指定标识
	 */
	bool EXAPI EXCALL ExEngineQueryFlag(DWORD flag);
}
