/**
 * @file logic_type_parser.hpp
 * @brief 逻辑类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	
	EX_SAMPLE_TYPE_PARSER(Bool, VT_BOOL, {
			bool value = false;
			HRESULT hr = ExParseToBool(str, &value);
			V_BOOL(V) = value ? VARIANT_TRUE : VARIANT_FALSE;
			return hr;
		}
	);
	
}
