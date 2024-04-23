/**
 * @file string_parser.cpp
 * @brief 文本解析实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-09
 * @copyright
 */

#include "stdafx.h"
#include "resource/string_parser.h"
#include "common/string.hpp"

#include "common/base64.hpp"

namespace ExDirectUI
{

	inline uint8_t _ExParse_GetUnit(LPCWSTR unit)
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

		//支持 单个字符 或者 \[ucode(10或0x16进制)] 或者 \\

		if (*str == L'\\') {
			if (str[1] != '\\' && str[1] != L'\0') {
				*r_value = (wchar_t)wcstoul(str, nullptr, 0);
			}
			else { *r_value = L'\\'; }
		}
		else { *r_value = *str; }

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

	HRESULT EXAPI EXCALL ExParseToFloat(LPCWSTR str, float* r_value, uint8_t* r_unit)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		wchar_t* end_of_number = nullptr;
		*r_value = wcstof(str, &end_of_number);
		if (r_unit) { *r_unit = _ExParse_GetUnit(end_of_number); }

		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToDouble(LPCWSTR str, double* r_value, uint8_t* r_unit)
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

	HRESULT EXAPI EXCALL ExParseToPointF(LPCWSTR str, ExPointF* r_value, uint8_t r_units[2])
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
			r_units[0] = _ExParse_GetUnit(end_of_number[0]);
			r_units[1] = (args.size() == 1) ?
				r_units[0] : _ExParse_GetUnit(end_of_number[1]);
		}

		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToRectF(LPCWSTR str, ExRectF* r_value, uint8_t r_units[4])
	{
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
			r_units[0] = _ExParse_GetUnit(end_of_number[0]);
			if (args.size() == 4) {
				r_units[1] = _ExParse_GetUnit(end_of_number[1]);
				r_units[2] = _ExParse_GetUnit(end_of_number[2]);
				r_units[3] = _ExParse_GetUnit(end_of_number[3]);
			}
			else if (args.size() == 2) {
				r_units[1] = _ExParse_GetUnit(end_of_number[1]);

				r_units[2] = r_units[0];
				r_units[3] = r_units[1];
			}
			else if (args.size() == 1) {
				r_units[1] = r_units[0];
				r_units[2] = r_units[0];
				r_units[3] = r_units[0];
			}
		}

		return S_OK;
	}

	inline byte_t _ExParse_2CharToByte(wchar_t low, wchar_t high)
	{
		byte_t byte = 0;

		if (high == L'\0') { byte = 0; }
		else if (high >= L'0' && high <= L'9') { byte = high - L'0'; }
		else if (high >= L'A' && high <= L'F') { byte = high - L'A' + 10; }
		else if (high >= L'a' && high <= L'f') { byte = high - L'a' + 10; }
		byte <<= 4;

		if (low == L'\0') {}
		else if (low >= L'0' && low <= L'9') { byte |= low - L'0'; }
		else if (low >= L'A' && low <= L'F') { byte |= low - L'A' + 10; }
		else if (low >= L'a' && low <= L'f') { byte |= low - L'a' + 10; }

		return byte;
	}

	HRESULT EXAPI EXCALL ExParseToData(LPCWSTR str, ExData* r_value, DWORD* r_type)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		//空字符串则直接返回
		if (*str == L'\0') {
			r_value->data = nullptr;
			r_value->size = 0;
			if (r_type) { *r_type = ExDataParseType::Hex; }
			return S_FALSE;
		}

		/*
			支持的格式：
			b: base64
			f: 文件名
			s: unicode字符串
			u: utf8字符串
			d: 10进制逗号分割字符串
			r: 资源id (并不会读取真实数据,只会将r_value.data设置为资源id,value.size设置为0,不需要释放)
			c: 自定义数据 (并不会读取真实数据,只会将r_value.data设置为字符串,value.size设置为0,不需要释放)
			//TODO:这里自定义数据的获取方式考虑改为调用回调函数

			其他情况按16进制字符串处理
		*/

		DWORD type = ExDataParseType::Hex;
		int len = lstrlenW(str);

		//?:
		if (len >= 2 && str[1] == L':') {
			wchar_t header = str[0];
			CharLowerBuffW(&header, 1);

			if (header == L'b') {
				try {
					ExBase64 b64;
					const auto data = b64.decode(str + 2);
					handle_if_failed(
						ExDataCopy(r_value, data.data(), data.size()),
						L"复制数据失败"
					);
					type = ExDataParseType::Base64;
				}
				catch_default({});
			}
			else if (header == L'f') {
				handle_if_failed(ExDataReadFile(str + 2, r_value), L"读取文件失败");
				type = ExDataParseType::File;
			}
			else if (header == L's') {
				handle_if_failed(
					ExDataCopy(r_value,
						(byte_t*)(str + 2), (len - 2) * sizeof(wchar_t)
					), L"复制数据失败"
				);
				type = ExDataParseType::String;
			}
			else if (header == L'u') {
				auto s = ExString::w2a(str + 2, CP_UTF8);
				handle_if_failed(
					ExDataCopy(r_value, (byte_t*)s.data(), s.size()),
					L"复制数据失败"
				);
				type = ExDataParseType::Utf8;
			}
			else if (header == L'd') {

				std::vector<byte_t> data;

				//找到起始位置
				LPWSTR p = (LPWSTR)str + 2;
				while (p) {
					//将它转为10进制数值,拿到数值后面的字符串指针
					data.push_back(
						(byte_t)wcstoul(p, &p, 10)
					);

					//找下一个逗号,找到了的话p就指到逗号后一个字符
					if (p) { p = wcsstr(p, L","); }
					if (p) { p += 1; }
				}

				//全部解析完毕后,生成数据块
				handle_if_failed(
					ExDataCopy(r_value, data.data(), data.size()),
					L"复制数据失败"
				);
				type = ExDataParseType::Data;
			}
			else if (header == L'r') {
				r_value->data = (byte_t*)(str + 2);
				r_value->size = 0;
				type = ExDataParseType::Resource;
			}
			else if (header == L'c') {
				r_value->data = (byte_t*)(str + 2);
				r_value->size = 0;
				type = ExDataParseType::Custom;
			}
			else { handle_ex(E_INVALIDARG, L"不支持的数据格式"); }
		}

		if (type == ExDataParseType::Hex) {
			//其他情况按16进制字符串处理(两位一个字节,不分割)

			uint32_t max_size = (uint32_t)ceil(len / 2.0f);
			uint32_t real_size = 0;
			handle_if_failed(ExDataAlloc(max_size, r_value), L"申请内存失败");

			/*

			wchar_t h = 0, l = 0;
			while (true) {
				wchar_t ch = *p;

				//如果当前字符是空白,则结束这个字节
				if (iswspace(ch)) {
					r_value->data[real_size++] = _ExParse_2CharToByte(l, h);
					h = l = 0;
				}
				//如果当前字符是结束符
				else if (ch == L'\0') {
					if (l != 0) {
						r_value->data[real_size++] = _ExParse_2CharToByte(l, 0);
					}
					break;
				}
				else {
					h = l; l = ch;

					//如果低位和高位都有了,则算出这个字节
					if (h != 0) {
						r_value->data[real_size++] = _ExParse_2CharToByte(l, h);
						h = l = 0;
					}
				}

				p++;
			}
			*/

			const wchar_t* p = str;
			wchar_t char_buff[3]{};

			//循环整个字符串，直到结束符
			while (*p) {

				//如果当前字符是空白字符,则跳过
				if (iswspace(*p)) { p++; continue; }

				//复制出两个字符(如果是正常字符串,不会超界的)
				memcpy(char_buff, p, sizeof(wchar_t) * 2);

				//如果第二个字符是结束符或空白字符,则表示只有低位字符
				if (char_buff[1] == L'\0' || iswspace(char_buff[1])) {
					char_buff[1] = char_buff[0];
					char_buff[0] = L'0';
				}

				//将这两个字符按16进制转为字节数据
				r_value->data[real_size++] = (byte_t)wcstoul(char_buff, nullptr, 16);

				//如果遇到了结束符，则结束循环
				if (char_buff[1] == L'\0') { break; }
				p += 2;
			}

			//如果长度不一致,按真实长度调整
			if (real_size != max_size) {
				handle_if_failed(
					ExDataResize(r_value, real_size),
					L"调整数据块尺寸失败",
					ExDataFree(r_value)
				);
			}

			type = ExDataParseType::Hex;
		}

		if (r_type) { *r_type = type; }
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToConst(LPCWSTR str, const DWORD key_values[][2], uint32_t count, DWORD* r_values)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_values);

		//计算原子号
		EXATOM key = ExAtom(str);

		//遍历表
		if (key_values) {
			for (uint32_t i = 0; i < count; i++) {
				if (key == key_values[i][0]) {
					*r_values = key_values[i][1];
					return S_OK;
				}
			}
		}

		//如果遍历没找到,则按数值解析
		*r_values = (DWORD)wcstoul(str, nullptr, 0);
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExParseToConsts(LPCWSTR str, const DWORD key_values[][2], uint32_t count, DWORD* r_values)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_values);

		//如果少于一个键值,则直接按一个解析
		if (key_values == nullptr || count <= 1) {
			return ExParseToConst(str, key_values, count, r_values); 
		}

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

	HRESULT EXAPI EXCALL ExParseToGridsImageInfo(LPCWSTR str, ExGridsImageInfo* r_value)
	{
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		uint8_t units[4]{};
		return_if_failed(ExParseToRectF(str, (ExRectF*)r_value, units));
		r_value->flags = 0;

		for (int i = 0; i < 4; i++) {
			uint8_t u = ExGridsImageMode::Scale;
			if (units[i] == ExNumberUnit::Question) { u = ExGridsImageMode::LeftNone; }
			else if (units[i] == ExNumberUnit::Star) { u = ExGridsImageMode::LeftTile; }
			else if (units[i] == ExNumberUnit::Sharp) { u = ExGridsImageMode::LeftMirror; }

			r_value->flags |= u << i;
		}

		return S_OK;
	}

}


