/**
 * @file module.i.h
 * @brief 模块接口头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once
#include "object.i.h"

namespace ExDirectUI
{
	struct ExModuleInfo;

	EXINTERFACE("D3888235-E4C6-4E0F-B26D-B85CA5202B11") IExModule : public IUnknown
	{
		EXMETHOD EXATOM EXOBJCALL GetId() PURE;
		EXMETHOD uint16_t EXOBJCALL GetType() PURE;
		EXMETHOD void EXOBJCALL GetInfo(ExModuleInfo* r_info) PURE;
		EXMETHOD HRESULT EXOBJCALL Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result) PURE;
	};

	EXINTERFACE("4BB3ED43-DD75-4679-882B-3E137D592496") IExModuleUtils : public IUnknown
	{

	};

}
