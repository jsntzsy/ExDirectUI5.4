/**
 * @file module_utils.h
 * @brief 模块辅助对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once
#include "common/utils/singleton.hpp"

namespace ExDirectUI
{
	struct ExModuleInfo;
	class IExModule;

	class ExModuleUtils : public ExSingleton<ExModuleUtils>
	{
	public:




	public:
		static void EXOBJCALL Group(const ExModuleInfo* info, IExModule* instance) MAYTHROW;
		static void EXOBJCALL UnGroup(IExModule* instance) MAYTHROW;
	private:

	};
}
