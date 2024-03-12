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
#include "common/interfaces/object_impl.hpp"

namespace ExDirectUI
{
	struct ExModuleInfo;
	class IExModule;

	class ExModuleUtils : public IExModuleUtils, public ExSingleton<ExModuleUtils>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IExModuleUtils);
		EX_INTERFACE_ENTRY(IUnknown);
		EX_END_INTERFACE_MAP();
		

		EXSTDMETHOD DecodeImageFile(LPCWSTR file, IExDecodeImage** r_image) override;
		EXSTDMETHOD DecodeImageMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) override;

	public:
		static HRESULT EXOBJCALL Group(uint16_t type, IExModule* instance);
		static HRESULT EXOBJCALL UnGroup(IExModule* instance);
	};
}
