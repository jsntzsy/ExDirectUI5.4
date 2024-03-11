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

}
