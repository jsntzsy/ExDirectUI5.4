/**
 * @file parse.h
 * @brief 文本解析头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-09
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	EXENUM(ExNumberUnit)
	{
		None,
		PX,
		DPI,
		PT,
		Percent,
		Star,
		Alert,
		Question,
		Sharp
	};


	//////////////////

	HRESULT EXAPI EXCALL ExParseToBool(LPCWSTR str, bool* r_value);
	HRESULT EXAPI EXCALL ExParseToByte(LPCWSTR str, byte_t* r_value);
	HRESULT EXAPI EXCALL ExParseToWChar(LPCWSTR str, wchar_t* r_value);
	HRESULT EXAPI EXCALL ExParseToUInt32(LPCWSTR str, uint32_t* r_value);
	HRESULT EXAPI EXCALL ExParseToInt32(LPCWSTR str, int32_t* r_value);
	HRESULT EXAPI EXCALL ExParseToUInt64(LPCWSTR str, uint64_t* r_value);
	HRESULT EXAPI EXCALL ExParseToInt64(LPCWSTR str, int64_t* r_value);
	HRESULT EXAPI EXCALL ExParseToFloat(LPCWSTR str, float* r_value, DWORD* r_unit = nullptr);
	HRESULT EXAPI EXCALL ExParseToDouble(LPCWSTR str, double* r_value, DWORD* r_unit = nullptr);
	HRESULT EXAPI EXCALL ExParseToColor(LPCWSTR str, EXARGB* r_value);

	HRESULT EXAPI EXCALL ExParseToPointF(LPCWSTR str, ExPointF* r_value, ExPoint* r_units = nullptr);
	HRESULT EXAPI EXCALL ExParseToRectF(LPCWSTR str, ExRectF* r_value, ExRect* r_units = nullptr);
	HRESULT EXAPI EXCALL ExParseToData(LPCWSTR str, ExData* r_value, DWORD* r_type = nullptr);

	HRESULT EXAPI EXCALL ExParseToConst(LPCWSTR str, DWORD key_values[][2], uint32_t count, DWORD* r_values);
	HRESULT EXAPI EXCALL ExParseToConsts(LPCWSTR str, DWORD key_values[][2], uint32_t count, DWORD* r_values);
}

