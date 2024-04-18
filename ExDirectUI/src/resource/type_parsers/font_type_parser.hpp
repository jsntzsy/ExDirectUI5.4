/**
 * @file font_type_parser.hpp
 * @brief 字体类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-16
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"
#include "src/drawing/render/factory.h"
#include "atom.h"

namespace ExDirectUI
{
	class ExFontTypeParser : public IExTypeParser, public ExSingleton<ExFontTypeParser>
	{
	public:
		inline static const DWORD _KV_FONT_STYLE_[][2] = {
			{ATOM_NORMAL, ExFontStyle::Normal},
			{ATOM_BOLD, ExFontStyle::Bold},
			{ATOM_ITALIC, ExFontStyle::Italic},
			{ATOM_UNDERLINE, ExFontStyle::UnderLine},
			{ATOM_STRIKEOUT, ExFontStyle::StrikeOut},
		};

		inline DWORD ConstToStyle(LPCWSTR str)
		{
			if (!str || !*str) { return ExFontStyle::Normal; }
			DWORD style = ExFontStyle::Normal;
			ExParseToConsts(str, _KV_FONT_STYLE_, _countof(_KV_FONT_STYLE_), &style);
			return style;
		}


		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			auto font = V_FONT(r_value);
			*font = g_drawing_default_font;

			auto attr = node->attribute(L"size");
			if (attr) { font->size = attr.as_int(font->size); }

			attr = node->attribute(L"style");
			if (attr) { font->style = ConstToStyle(attr.value()); }

			attr = node->attribute(L"weight");
			if (attr) { font->SetWeight(attr.as_int(FW_NORMAL)); }

			attr = node->attribute(L"name");
			if (attr) {
				font->SetName(attr.as_string(font->name));

				attr = node->attribute(L"file");
				if (attr) {
					ExPackageItemInfo item{};
					handle_if_failed(
						_ExParser_GetPackageItem(owner, attr.value(), &item),
						L"获取字体文件失败"
					);
					handle_if_failed(
						ExFontLoadFile(item.data, item.size, &font->file_atom),
						L"加载字体文件失败"
					);
				}
			}

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			auto font = V_FONT(r_value);
			*font = g_drawing_default_font;

			auto args = _ExParser_GetArgsMap(str);
			LPCWSTR value = _ExParser_GetArg(args, ATOM_NAME);
			if (value) {
				font->SetName(value);

				value = _ExParser_GetArg(args, ATOM_FILE);
				if (value) {
					ExPackageItemInfo item{};
					return_if_failed(
						_ExParser_GetPackageItem(owner, value, &item)
					);
					return_if_failed(
						ExFontLoadFile(item.data, item.size, &font->file_atom)
					);
				}
			}

			value = _ExParser_GetArg(args, ATOM_SIZE);
			if (value) {
				font->size = wcstoul(value, nullptr, 10);
			}

			value = _ExParser_GetArg(args, ATOM_STYLE);
			if (value) {
				font->style = ConstToStyle(value);
			}

			value = _ExParser_GetArg(args, ATOM_WEIGHT);
			if (value) {
				font->SetWeight(wcstoul(value, nullptr, 10));
			}

			return S_OK;
		}

	};







}
