﻿/**
 * @file data_type_parser.hpp
 * @brief 数据类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"
#include "resource/package.h"

namespace ExDirectUI
{
	EX_SAMPLE_TYPE_PARSER(Data, EVT_DATA, {

		auto data = V_DATA(V);
		DWORD type = ExDataParseType::Hex;
		HRESULT hr = ExParseToData(str, data, &type);
		if (FAILED(hr)) { return hr; }

		//后处理
		//这里可能还得做全局处理
		if (type == ExDataParseType::Resource)
		{
			ExAutoPtr<IExPackage> package;
			if (owner && SUCCEEDED(owner->QueryInterface(&package)))
			{

			}
			else { return E_INVALIDARG; }
		}
		else if (type == ExDataParseType::Custom)
		{

		}

		return hr;
		}
	);

	EX_SAMPLE_TYPE_PARSER(Char, VT_UI2, return ExParseToWChar(str, (wchar_t*)&V_UI2(V)));

}
