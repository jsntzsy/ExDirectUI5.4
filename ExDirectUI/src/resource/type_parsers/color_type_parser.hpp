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
			IUnknown* owner, ExVariant* V) override
		{
			if (type == ATOM_ALPHA) {
				return __super::ParseFromXmlNode(type, node, owner, V);
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
				ExParseToFloat(attr.value(), &opacity->disable_percent, &unit);
				if (unit == ExNumberUnit::Percent) { opacity->disable_percent /= 100.f; }
			}
			else { opacity->disable_percent = 0.5f; }

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,
			IUnknown* owner, ExVariant* V) override
		{
			DWORD alpha = ALPHA_OPAQUE;
			if (type == ATOM_ALPHA) {
				return_if_failed(ExVariantInit(V, VT_UI1));
				ExParseToConst(str, _KV_ALPHA_, _countof(_KV_ALPHA_), &alpha);
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

	////////////

	class ExStateColorTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExStateColorTypeParser>
	{
		inline static void SetDefaultColor(ExStateColorInfo* sc)
		{
			//根据需求决定要不要调用
			if (sc->disable == COLOR_UNDEFINE) { sc->disable = sc->normal; }
			if (sc->hover == COLOR_UNDEFINE) { sc->hover = sc->normal; }
			if (sc->press == COLOR_UNDEFINE) { sc->press = sc->normal; }
			if (sc->focus == COLOR_UNDEFINE) { sc->press = sc->focus; }
		}

	public:
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_STATE_COLOR));
			auto sc = V_STATE_COLOR(r_value);

			auto attr = node->attribute(L"normal");
			if (attr) { ExParseToColor(attr.value(), &sc->normal); }
			else { sc->normal = COLOR_UNDEFINE; }

			attr = node->attribute(L"hover");
			if (attr) { ExParseToColor(attr.value(), &sc->hover); }
			else { sc->hover = COLOR_UNDEFINE; }

			attr = node->attribute(L"press");
			if (attr) { ExParseToColor(attr.value(), &sc->press); }
			else { sc->press = COLOR_UNDEFINE; }

			attr = node->attribute(L"focus");
			if (attr) { ExParseToColor(attr.value(), &sc->focus); }
			else { sc->focus = COLOR_UNDEFINE; }

			attr = node->attribute(L"disable");
			if (attr) { ExParseToColor(attr.value(), &sc->disable); }
			else { sc->disable = COLOR_UNDEFINE; }

			SetDefaultColor(sc);
			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_STATE_COLOR));
			auto sc = V_STATE_COLOR(r_value);
			sc->normal = COLOR_UNDEFINE;
			sc->hover = COLOR_UNDEFINE;
			sc->press = COLOR_UNDEFINE;
			sc->focus = COLOR_UNDEFINE;
			sc->disable = COLOR_UNDEFINE;

			auto args = GetArgsMap(str);

			auto value = GetArg(args, ATOM_NORMAL);
			if (!value.empty()) { ExParseToColor(value.c_str(), &sc->normal); }
			value = GetArg(args, ATOM_HOVER);
			if (!value.empty()) { ExParseToColor(value.c_str(), &sc->hover); }
			value = GetArg(args, ATOM_PRESS);
			if (!value.empty()) { ExParseToColor(value.c_str(), &sc->press); }
			value = GetArg(args, ATOM_FOCUS);
			if (!value.empty()) { ExParseToColor(value.c_str(), &sc->focus); }
			value = GetArg(args, ATOM_DISABLE);
			if (!value.empty()) { ExParseToColor(value.c_str(), &sc->disable); }

			SetDefaultColor(sc);
			return S_OK;
		}

	};

}
