/**
 * @file parse.cpp
 * @brief 文本解析实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-09
 * @copyright
 */

#include "stdafx.h"
#include "resource/parse.h"
#include "common/string.hpp"

namespace ExDirectUI
{
	inline ExNumberUnit _ExParse_GetUnit(LPCWSTR unit)
	{
		//空则直接返回
		if (!unit || unit[0] == L'\0') { return ExNumberUnit::None; }

		//转小写
		wchar_t buffer[16]{};
		wcsncpy_s(buffer, unit, 15);
		CharLowerW(buffer);

		//跟待选项判断
		if (wcsstr(buffer, L"px")) { return ExNumberUnit::PX; }
		else if (wcsstr(buffer, L"dp")) { return ExNumberUnit::DPI; }
		else if (wcsstr(buffer, L"pt")) { return ExNumberUnit::PT; }
		else if (wcsstr(buffer, L"%")) { return ExNumberUnit::Percent; }
		else if (wcsstr(buffer, L"*")) { return ExNumberUnit::Star; }
		else if (wcsstr(buffer, L"!")) { return ExNumberUnit::Alert; }
		else if (wcsstr(buffer, L"?")) { return ExNumberUnit::Question; }
		else if (wcsstr(buffer, L"#")) { return ExNumberUnit::Sharp; }

		//默认则返回无
		return ExNumberUnit::None;
	}

	//////////////////

	HRESULT EXAPI EXCALL ExParseToBool(LPCWSTR str, bool* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		if (lstrcmpiW(str, L"true") == 0) { *r_value = true; }
		else if (lstrcmpiW(str, L"yes") == 0) { *r_value = true; }
		else if (lstrcmpiW(str, L"ok") == 0) { *r_value = true; }
		else { *r_value = wcstol(str, nullptr, 0) != 0; }
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToByte(LPCWSTR str, byte_t* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		*r_value = (byte_t)wcstoul(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToWChar(LPCWSTR str, wchar_t* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		//准备改为 支持数值和'c'这种形式
		*r_value = *str;
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToUInt32(LPCWSTR str, uint32_t* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		*r_value = (uint32_t)wcstoul(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToInt32(LPCWSTR str, int32_t* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		*r_value = (int32_t)wcstol(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToUInt64(LPCWSTR str, uint64_t* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		*r_value = (uint64_t)_wcstoui64(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToInt64(LPCWSTR str, int64_t* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		*r_value = (int64_t)_wcstoui64(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToFloat(LPCWSTR str, float* r_value, DWORD* r_unit)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		wchar_t* end_of_number = nullptr;
		*r_value = wcstof(str, &end_of_number);
		if (r_unit) { *r_unit = _ExParse_GetUnit(end_of_number); }

		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToDouble(LPCWSTR str, double* r_value, DWORD* r_unit)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		wchar_t* end_of_number = nullptr;
		*r_value = wcstod(str, &end_of_number);
		if (r_unit) { *r_unit = _ExParse_GetUnit(end_of_number); }

		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToColor(LPCWSTR str, EXARGB* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		EXCHANNEL ch[4] = {};	//r,g,b,a
		wstring s = ExString::trim_to(str);

		if (s[0] == L'#') {
			size_t len = s.size() - 1;
			//#RGB,#RGBA
			if (len == 3 || len == 4) {
				for (size_t i = 0; i <= 2; i++) {
					ch[i] = wcstoul(s.substr(1 + i, 1).c_str(), nullptr, 16);
					ch[i] |= ch[i] << 4;
				}
				if (len == 4) {
					ch[3] = wcstoul(s.substr(4, 1).c_str(), nullptr, 16);
					ch[3] |= ch[3] << 4;
				}
				else { ch[3] = ALPHA_OPAQUE; }
			}
			//#RRGGBB,#RRGGBBAA
			else if (len == 6 || len == 8) {
				for (size_t i = 0; i <= 2; i++) {
					ch[i] = wcstoul(s.substr(1 + i * 2, 2).c_str(), nullptr, 16);
				}
				if (len == 8) {
					ch[3] = wcstoul(s.substr(7, 2).c_str(), nullptr, 16);
				}
				else { ch[3] = ALPHA_OPAQUE; }
			}
			else { handle_ex(E_NOTIMPL, L"不支持此文本格式"); }
		}
		else if (s[0] == L'(') {
			//(r,g,b[,a])
			s = s.substr(1, s.size() - 2);
			auto args = ExString::split(s, L",");
			if (args.size() == 3 || args.size() == 4) {
				ch[3] = ALPHA_OPAQUE;
				for (size_t i = 0; i < args.size(); i++) {
					ch[i] = wcstoul(args[i].c_str(), nullptr, 10);
				}
			}
			else { handle_ex(E_NOTIMPL, L"不支持此文本格式"); }
		}
		else {
			//0xAARRGGBB,color_int
			*r_value = wcstoul(s.c_str(), nullptr, 0);
			return S_OK;
		}

		*r_value = MAKEARGB(ch[0], ch[1], ch[2], ch[3]);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToPointF(LPCWSTR str, ExPointF* r_value, ExPoint* r_units)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		//按逗号分割
		wchar_t* end_of_number[2]{};
		auto args = ExString::split(str, L",");

		//根据参数个数,解析到对应的成员中
		if (args.size() == 2) {
			r_value->x = wcstof(args[0].c_str(), &end_of_number[0]);
			r_value->y = wcstof(args[1].c_str(), &end_of_number[1]);
		}
		else if (args.size() == 1) {
			r_value->x = wcstof(args[0].c_str(), &end_of_number[0]);
			r_value->y = r_value->x;
		}
		else { handle_ex(E_NOTIMPL, L"不支持此文本格式"); }

		//解析单位
		if (r_units) {
			r_units->x = _ExParse_GetUnit(end_of_number[0]);
			r_units->y = (args.size() == 1) ?
				r_units->x : _ExParse_GetUnit(end_of_number[1]);
		}

		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToRectF(LPCWSTR str, ExRectF* r_value, ExRect* r_units)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		//按逗号分割
		wchar_t* end_of_number[4]{};
		auto args = ExString::split(str, L",");

		//根据参数个数,解析到对应的成员中
		if (args.size() == 4) {
			r_value->left = wcstof(args[0].c_str(), &end_of_number[0]);
			r_value->top = wcstof(args[1].c_str(), &end_of_number[1]);
			r_value->right = wcstof(args[2].c_str(), &end_of_number[2]);
			r_value->bottom = wcstof(args[3].c_str(), &end_of_number[3]);
		}
		else if (args.size() == 2) {
			r_value->left = wcstof(args[0].c_str(), &end_of_number[0]);
			r_value->top = wcstof(args[1].c_str(), &end_of_number[1]);
			r_value->right = r_value->left;
			r_value->bottom = r_value->top;
		}
		else if (args.size() == 1) {
			r_value->left = wcstof(args[0].c_str(), &end_of_number[0]);
			r_value->top = r_value->right = r_value->bottom = r_value->left;
		}
		else { handle_ex(E_NOTIMPL, L"不支持此文本格式"); }

		//解析单位
		if (r_units) {
			r_units->left = _ExParse_GetUnit(end_of_number[0]);
			if (args.size() == 4) {
				r_units->top = _ExParse_GetUnit(end_of_number[1]);
				r_units->right = _ExParse_GetUnit(end_of_number[2]);
				r_units->bottom = _ExParse_GetUnit(end_of_number[3]);
			}
			else if (args.size() == 2) {
				r_units->top = _ExParse_GetUnit(end_of_number[1]);

				r_units->right = r_units->left;
				r_units->bottom = r_units->top;
			}
			else if (args.size() == 1) {
				r_units->top = r_units->left;
				r_units->right = r_units->left;
				r_units->bottom = r_units->left;
			}
		}
		
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToData(LPCWSTR str, ExData* r_value, DWORD* r_type)
	{
		return S_FALSE;
	}

	HRESULT EXAPI EXCALL ExParseToConst(LPCWSTR str, DWORD key_values[][2], uint32_t count, DWORD* r_values)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(key_values);
		CHECK_PARAM(r_values);
		
		//计算原子号
		EXATOM key = ExAtom(str);

		//遍历表
		for (uint32_t i = 0; i < count; i++) {
			if (key == key_values[i][0]) {
				*r_values = key_values[i][1];
				return S_OK;
			}
		}
		
		//如果遍历没找到,则按数值解析
		*r_values = (DWORD)wcstoul(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToConsts(LPCWSTR str, DWORD key_values[][2], uint32_t count, DWORD* r_values)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(key_values);
		CHECK_PARAM(r_values);

		//如果只有一个键值,则直接按一个解析
		if (count <= 1) { return ExParseToConst(str, key_values, count, r_values); }
		
		//按逗号分割,如果没有逗号,则也按一个解析
		auto args = ExString::split(str, L",");
		if (args.size() <= 1) { return ExParseToConst(str, key_values, count, r_values); }
		
		//将键值对转换为map
		std::unordered_map<EXATOM, DWORD> kv_map;
		for (uint32_t i = 0; i < count; i++) {
			kv_map[key_values[i][0]] = key_values[i][1];
		}
		
		//遍历每个值,在表里找
		DWORD value = 0;
		for (auto& arg : args) {

			//去掉前后的空格,计算原子号
			ExString::trim(arg);
			EXATOM key = ExAtom(arg.c_str());

			//在表里找，找到则加入对应值,否则按数值解析
			auto it = kv_map.find(key);
			if (it != kv_map.end()) { value |= it->second; }
			else { value |= (DWORD)wcstoul(arg.c_str(), nullptr, 0); }
		}
		
		return S_OK;
	}

}


