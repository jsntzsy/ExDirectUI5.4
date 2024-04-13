/**
 * @file data_type_parser.hpp
 * @brief 数据类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	EX_SAMPLE_TYPE_PARSER(Data, EVT_DATA, {
		DWORD type = ExDataParseType::Hex;
		HRESULT hr = ExParseToData(str, &V_EXF(V, data_), &type);
		if (FAILED(hr)) { return hr; }
		//后处理
		

		return hr;
		}
	);

	EX_SAMPLE_TYPE_PARSER(Char, VT_UI2, return ExParseToWChar(str, (wchar_t*)&V_UI2(V)));

}
