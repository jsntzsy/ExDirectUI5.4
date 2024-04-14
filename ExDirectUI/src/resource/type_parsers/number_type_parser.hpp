/**
 * @file number_type_parser.hpp
 * @brief 数值类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	EX_SAMPLE_TYPE_PARSER(Byte, VT_UI1, return ExParseToByte(str, (byte_t*)&V_UI1(V)));
	
	EX_SAMPLE_TYPE_PARSER(Int32, VT_I4, return ExParseToInt32(str, (int32_t*)&V_I4(V)));
	EX_SAMPLE_TYPE_PARSER(UInt32, VT_UI4, return ExParseToUInt32(str, (uint32_t*)&V_UI4(V)));

	EX_SAMPLE_TYPE_PARSER(Int64, VT_I8, return ExParseToInt64(str, (int64_t*)&V_I4(V)));
	EX_SAMPLE_TYPE_PARSER(UInt64, VT_UI8, return ExParseToUInt64(str, (uint64_t*)&V_UI4(V)));

	EX_SAMPLE_TYPE_PARSER(Float, VT_R4, return ExParseToFloat(str, &V_R4(V)));
	EX_SAMPLE_TYPE_PARSER(Double, VT_R8, return ExParseToDouble(str, &V_R8(V)));

	EX_SAMPLE_TYPE_PARSER(NumberU, EVT_NUMU, {
		auto num = V_NUMU(V);
		return ExParseToFloat(str, &num->value, &num->unit);
		}
	);
	
}
