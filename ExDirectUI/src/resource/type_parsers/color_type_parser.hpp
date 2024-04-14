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
#include "resource/theme.h"
#include "common/string.hpp"

namespace ExDirectUI
{
	EX_SAMPLE_TYPE_PARSER(Color, EVT_COLOR, return ExParseToColor(str, (EXARGB*)&V_UI4(V)));
	EX_SAMPLE_TYPE_PARSER(Channel, VT_UI1, return ExParseToByte(str, (byte_t*)&V_UI1(V)));

	///////////

	class ExOpacityTypeParser : public ExStandardTypeParser,
		public ExSingleton<ExChannelTypeParser>
	{
	public:
		inline static const DWORD _KV_ALPHA_[3][2] = {
			{ATOM_TRANSPARENT, ALPHA_TRANSPARENT},
			{ATOM_OPAQUE, ALPHA_OPAQUE},
			{ATOM_HALF, ALPHA_HALF},
		};

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			LPARAM lparam, ExVariant* V) override
		{
			if (type == ATOM_ALPHA) {
				return __super::ParseFromXmlNode(type, node, lparam, V);
			}
			
			return_if_failed(ExVariantInit(V, EVT_ELE_OPACITY));
			
			auto opacity = V_ELE_OPACITY(V);
			
			pugi::xml_attribute attr = node->attribute(L"normal");
			DWORD alpha = ALPHA_OPAQUE;
			if (attr) {
				ExParseToConst(attr.value(), _KV_ALPHA_,
					_countof(_KV_ALPHA_), &alpha);
			}
			opacity->normal = (EXCHANNEL)alpha;

			attr = node->attribute(L"disable");
			if (attr) {
				uint8_t unit = 0;
				ExParseToFloat(attr.value(), &opacity->disable_percent,&unit);
				if (unit == ExNumberUnit::Percent) { opacity->disable_percent /= 100.f; }
			}
			else { opacity->disable_percent = 0.5f; }

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			LPARAM lparam, ExVariant* V) override
		{
			DWORD alpha = ALPHA_OPAQUE;
			if (type == ATOM_ALPHA) {
				return_if_failed(ExVariantInit(V, VT_UI1));
				ExParseToConst(str, _KV_ALPHA_, 
					_countof(_KV_ALPHA_), &alpha);
				V_UI1(V) = (EXCHANNEL)alpha;
			}
			else {
				return_if_failed(ExVariantInit(V, EVT_ELE_OPACITY));
				auto opacity = V_ELE_OPACITY(V);

				wstring normal, disable;
				ExString::slice(str, L" ", normal, disable);

				if (normal.size() != 0) {
					ExParseToConst(normal.c_str(), _KV_ALPHA_, 
						_countof(_KV_ALPHA_), &alpha);
				}
				opacity->normal = (EXCHANNEL)alpha;

				if (disable.size() != 0) {
					uint8_t unit = 0;
					ExParseToFloat(disable.c_str(), &opacity->disable_percent, &unit);
					if (unit == ExNumberUnit::Percent) { opacity->disable_percent /= 100.f; }
				}
				else { opacity->disable_percent = 0.5f; }

			}
			return S_OK;
		}
	};

}
