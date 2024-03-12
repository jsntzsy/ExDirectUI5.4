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
	};

	/// 引擎初始化信息
	struct ExEngineInitInfo
	{
		/// 实例句柄
		HINSTANCE instance;

		/// 初始化标识
		DWORD flags;

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
