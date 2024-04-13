/**
 * @file string_parser.h
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

	EXENUM(ExDataParseType)
	{
		Hex,
		Base64,
		File,
		Web,
		String,
		Utf8,
		Data,
		Resource,
		Custom,
	};

	inline LPCWSTR ExNumberUnitToString(uint8_t unit)
	{
		switch (unit)
		{
		case ExNumberUnit::None: return L"";
		case ExNumberUnit::PX: return L"px";
		case ExNumberUnit::DPI: return L"dpi";
		case ExNumberUnit::PT: return L"pt";
		case ExNumberUnit::Percent: return L"%";
		case ExNumberUnit::Star: return L"*";
		case ExNumberUnit::Alert: return L"!";
		case ExNumberUnit::Question: return L"?";
		case ExNumberUnit::Sharp: return L"#";
		default: return L"";
		}
	}

	//////////////////

	HRESULT EXAPI EXCALL ExParseToBool(LPCWSTR str, bool* r_value);
	HRESULT EXAPI EXCALL ExParseToByte(LPCWSTR str, byte_t* r_value);
	HRESULT EXAPI EXCALL ExParseToWChar(LPCWSTR str, wchar_t* r_value);
	HRESULT EXAPI EXCALL ExParseToUInt32(LPCWSTR str, uint32_t* r_value);
	HRESULT EXAPI EXCALL ExParseToInt32(LPCWSTR str, int32_t* r_value);
	HRESULT EXAPI EXCALL ExParseToUInt64(LPCWSTR str, uint64_t* r_value);
	HRESULT EXAPI EXCALL ExParseToInt64(LPCWSTR str, int64_t* r_value);
	HRESULT EXAPI EXCALL ExParseToFloat(LPCWSTR str, float* r_value, uint8_t* r_unit = nullptr);
	HRESULT EXAPI EXCALL ExParseToDouble(LPCWSTR str, double* r_value, uint8_t* r_unit = nullptr);
	HRESULT EXAPI EXCALL ExParseToColor(LPCWSTR str, EXARGB* r_value);

	HRESULT EXAPI EXCALL ExParseToPointF(LPCWSTR str, ExPointF* r_value, uint8_t r_units[2] = nullptr);
	HRESULT EXAPI EXCALL ExParseToRectF(LPCWSTR str, ExRectF* r_value, uint8_t r_units[4] = nullptr);
	HRESULT EXAPI EXCALL ExParseToData(LPCWSTR str, ExData* r_value, DWORD* r_type = nullptr);

	HRESULT EXAPI EXCALL ExParseToConst(LPCWSTR str, const DWORD key_values[][2], uint32_t count, DWORD* r_values);
	HRESULT EXAPI EXCALL ExParseToConsts(LPCWSTR str, const DWORD key_values[][2], uint32_t count, DWORD* r_values);
}

