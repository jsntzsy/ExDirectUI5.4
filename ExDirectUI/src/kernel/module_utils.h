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
#include "kernel/module.h"
#include "common/interfaces/unknown_impl.hpp"

namespace ExDirectUI
{
	struct ExModuleInfo;
	class IExModule;

	class ExModuleUtils : public ExUnknownImpl<IExModuleUtils>, public ExSingleton<ExModuleUtils>
	{
	public:
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IExModuleUtils);
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE_END();
		

		EXSTDMETHOD DecodeImageFile(LPCWSTR file, IExDecodeImage** r_image) override;
		EXSTDMETHOD DecodeImageMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) override;

	public:
		static HRESULT EXOBJCALL Group(uint16_t type, IExModule* instance) MAYTHROW;
		static HRESULT EXOBJCALL UnGroup(IExModule* instance) MAYTHROW;
		static HRESULT EXOBJCALL NessaryCheck() MAYTHROW;
	};
}
