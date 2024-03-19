/**
 * @file string.hpp
 * @brief 字符串功能实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#include <string>
#include <vector>
#include <sstream>  
#include <algorithm>   
#include <functional>   
#include <cctype>  
#include <locale>  

#include <Windows.h>

#include "config.h"

namespace ExDirectUI
{
	class ExString
	{
	public:
		static inline void ltrim(std::wstring& s) 
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
				return !isspace(ch);
				}));
		}

		static inline void rtrim(std::wstring& s) 
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
				return !isspace(ch);
				}).base(), s.end());
		}

		static inline void trim(std::wstring& s)
		{
			ltrim(s);
			rtrim(s);
		}

		static inline std::wstring ltrim_to(std::wstring str)
		{
			ltrim(str);
			return str;
		}

		static inline std::wstring rtrim_to(std::wstring str)
		{
			rtrim(str);
			return str;
		}

		static inline std::wstring trim_to(std::wstring str)
		{
			trim(str);
			return str;
		}

		static inline std::wstring left(std::wstring& str, size_t len)
		{
			return str.substr(0, len);
		}

		static inline std::wstring right(std::wstring& str, size_t len)
		{
			return str.substr(str.size() - len, len);
		}

		static inline std::wstring between(const std::wstring str,
			const std::wstring begin_str, std::wstring end_str, size_t start_pos = 0)
		{
			std::wstring ret;
			std::wstring::size_type begin_pos = str.find(begin_str, start_pos);
			if (begin_pos != std::wstring::npos) {
				begin_pos += begin_str.length();
				std::wstring::size_type end_pos = str.find(end_str, begin_pos);
				if (begin_pos != std::wstring::npos) {
					ret = str.substr(begin_pos, end_pos - begin_pos);
				}
			}
			return ret;
		}

		static inline int compare(const std::wstring& s1, const std::wstring& s2, bool no_case = false)
		{
			if (no_case) { return _wcsicmp(s1.c_str(), s2.c_str()); }
			else { return wcscmp(s1.c_str(), s2.c_str()); }
		}

		static inline std::wstring lower(const std::wstring& str)
		{
			std::wstring ret = str;
			CharLowerBuffW(ret.data(), (DWORD)ret.size());
			return ret;
		}

		static inline std::wstring upper(const std::wstring& str)
		{
			std::wstring ret = str;
			CharUpperBuffW(ret.data(), (DWORD)ret.size());
			return ret;
		}

		static std::wstring a2w(const std::string& str, int code_page_from = CP_ACP)
		{
			std::wstring result;
			int len = MultiByteToWideChar(code_page_from, 0, str.c_str(), (int)str.size(), nullptr, 0);
			result.resize(len + 1, L'\0');
			MultiByteToWideChar(code_page_from, 0, str.c_str(), (int)str.size(), result.data(), (int)result.length());
			return result;
		}

		static std::string w2a(const std::wstring& wstr, int code_page_to = CP_ACP)
		{
			std::string result;
			int len = WideCharToMultiByte(code_page_to, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
			result.resize(len + 1, '\0');
			WideCharToMultiByte(code_page_to, 0, wstr.c_str(), (int)wstr.size(),
				result.data(), (int)result.length(), nullptr, nullptr);
			return result;
		}

		static std::string a2a(const std::string& str, int code_page_from, int code_page_to)
		{
			if (code_page_from == code_page_to) { return str; }
			std::wstring tmp_str = a2w(str, code_page_from);
			return w2a(tmp_str, code_page_to);
		}

		static std::wstring format(const wchar_t* format, ...)
		{
			std::wstring result = L"";
			if (format != nullptr) {
				wchar_t buffer[EX_CFG_SIZEOF_FORMAT_BUF]{};
				LPWSTR buffer_ptr = buffer;

				va_list args;
				va_start(args, format);

				int len = _vscwprintf(format, args) + 1;
				if (len >= EX_CFG_SIZEOF_FORMAT_BUF) {
					buffer_ptr = new wchar_t[len];
				}

				if (buffer_ptr) {
					vswprintf_s(buffer, len, format, args);
					result = buffer;
					if (buffer_ptr != buffer) { delete[] buffer_ptr; }
				}

				va_end(args);
			}
			return result;
		}

		static std::wstring format(const wchar_t* format, va_list args)
		{
			std::wstring result = L"";
			if (format != nullptr) {
				wchar_t buffer[EX_CFG_SIZEOF_FORMAT_BUF]{};
				LPWSTR buffer_ptr = buffer;

				int len = _vscwprintf(format, args) + 1;
				if (len >= EX_CFG_SIZEOF_FORMAT_BUF) {
					buffer_ptr = new wchar_t[len];
				}

				if (buffer_ptr) {
					vswprintf_s(buffer, len, format, args);
					result = buffer;
					if (buffer_ptr != buffer) { delete[] buffer_ptr; }
				}
			}
			return result;
		}

		static std::wstring space(size_t len)
		{
			std::wstring ret;
			ret.resize(len, L' ');
			return ret;
		}

		static std::wstring repeat(const std::wstring& str, size_t count)
		{
			std::wstring ret;
			size_t str_len = str.size();
			ret.resize(str_len * count);
			for (size_t i = 0; i < count; i++) {
				ret.replace(i * str_len, str_len, str);
			}
			return ret;
		}

		static std::vector<size_t> find_all(const std::wstring& str, const std::wstring& sub_str)
		{
			std::vector<size_t> ret;
			size_t pos = str.find(sub_str, 0);
			while (pos != std::wstring::npos)
			{
				ret.push_back(pos);
				pos = str.find(sub_str, pos + 1);
			}
			return ret;
		}

		static std::wstring replace_all(const std::wstring& str, const std::wstring& old_value, const std::wstring& new_value)
		{
			std::wstring ret = str;
			std::wstring::size_type pos(0);
			while ((pos = str.find(old_value)) != std::wstring::npos) {
				ret.replace(pos, old_value.length(), new_value);
			}
			return ret;
		}

		//注意：当字符串为空时，也会返回带有一个空字符串项的数组
		static std::vector<std::wstring> split(const std::wstring& s, const wchar_t* delim)
		{
			std::vector<std::wstring> r_result;
			size_t last = 0;
			size_t index = s.find_first_of(delim, last);
			while (index != std::wstring::npos) {
				r_result.push_back(s.substr(last, index - last));
				last = index + 1;
				index = s.find_first_of(delim, last);
			}
			if (index - last > 0) {
				r_result.push_back(s.substr(last, index - last));
			}
			return r_result;
		}

		static bool slice(const std::wstring& s, const wchar_t* delim, std::wstring& left, std::wstring& right)
		{
			size_t index = s.find(delim);
			size_t delim_len = wcslen(delim);
			if (index != std::wstring::npos) {
				left = s.substr(0, index);
				right = s.substr(index + delim_len + 1);
				return true;
			}
			return false;
		}

	};


}