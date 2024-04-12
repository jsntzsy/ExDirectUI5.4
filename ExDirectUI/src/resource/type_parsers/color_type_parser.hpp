/**
 * @file color_type_parser.hpp
 * @brief 颜色类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	EX_SAMPLE_TYPE_PARSER(Color, EVT_COLOR, return ExParseToColor(str, (EXARGB*)&V_UI4(V)));

	///////////

	class ExChannelTypeParser : public ExStandardTypeParser,
		public ExSingleton<ExChannelTypeParser>
	{
	public:
		inline static const DWORD _KV_ALPHA_[3][2] = {
			{ATOM_TRANSPARENT, ALPHA_TRANSPARENT},
			{ATOM_OPAQUE, ALPHA_OPAQUE},
			{ATOM_HALF, ALPHA_HALF},
		};

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			LPARAM lparam, ExVariant* V) override
		{
			HRESULT hr = ExVariantInit(V, VT_UI1);
			if (FAILED(hr)) { return hr; }

			//如果是透明度则先做一个常量值的判断
			if (type == ATOM_OPACITY || type == ATOM_ALPHA) {
				DWORD alpha = 0;
				hr = ExParseToConst(str, _KV_ALPHA_, 3, &alpha);
				V_UI1(V) = (uint8_t)alpha;
			}
			else { hr = ExParseToByte(str, (byte_t*)&V_UI1(V)); }

			return hr;
		}
	};

}
