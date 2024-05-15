/**
 * @file object.i.h
 * @brief 对象类接口定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once
#include <Unknwn.h>
#include "define.h"

namespace ExDirectUI
{
	EXINTERFACE("23867CF7-9092-4821-A156-F768331D63DA") IExObject : public IUnknown
	{
		EXMETHOD BSTR EXOBJCALL ToString() const PURE;
		EXMETHOD void* EXOBJCALL GetContext(int index) const PURE;
	};
}

