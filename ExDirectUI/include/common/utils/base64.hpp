/**
 * @file base64.hpp
 * @brief base64编解码实现类头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 */
#pragma once
#include <string>
#include <vector>

namespace ExDirectUI
{
	class ExBase64
	{
	public:
		static std::wstring encode(const byte_t* data, size_t size) 
		{
			PARAM_CHECK_THROW(data);
			if (size == 0) return L"";

			const wchar_t* base64_chars =
				L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				L"abcdefghijklmnopqrstuvwxyz"
				L"0123456789+/";

			std::wstring encoded_string;
			int val = 0, bits = -6;
			
			const byte_t* ptr = data;
			for (size_t i = 0; i < size; i++)
			{
				const byte_t c = *ptr++;
				
				val = (val << 16) + c;
				bits += 16;
				while (bits >= 0) 
				{
					encoded_string.push_back(base64_chars[(val >> bits) & 0x3F]);
					bits -= 6;
				}
			}

			if (bits > -6) {
				encoded_string.push_back(base64_chars[((val << 16) >> (bits + 16)) & 0x3F]);
			}

			while (encoded_string.size() % 4 != 0)
			{
				encoded_string.push_back(L'=');
			}

			return encoded_string;
		}

		static std::vector<byte_t> decode(const std::wstring& encoded_string) 
		{
			const wchar_t* base64_chars =
				L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				L"abcdefghijklmnopqrstuvwxyz"
				L"0123456789+/";

			std::vector<int> decoding_table(256, -1);
			for (int i = 0; i < 64; ++i) 
			{
				decoding_table[base64_chars[i]] = i;
			}

			std::vector<byte_t> decoded_data;
			int val = 0, bits = -8;
			for (wchar_t c : encoded_string) {
				if (iswspace(c) || c == L'=')
				{
					continue;
				}

				throw_if_false(decoding_table[c] != -1, ES_INVALID, L"非法的base64字符串");

				val = (val << 6) + decoding_table[c];
				bits += 6;
				
				if (bits >= 0) 
				{
					decoded_data.push_back(wchar_t((val >> bits) & 0xFF));
					bits -= 8;
				}
			}

			return decoded_data;
		}
	};

}

