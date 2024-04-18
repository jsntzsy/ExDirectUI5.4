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
			auto it = args.find(ATOM_NAME);
			if (it != args.end()) {
				font->SetName(it->second.c_str());

				it = args.find(ATOM_FILE);
				if (it != args.end()) {
					ExPackageItemInfo item{};
					handle_if_failed(
						_ExParser_GetPackageItem(owner, it->second.c_str(), &item),
						L"获取字体文件失败"
					);
					handle_if_failed(
						ExFontLoadFile(item.data, item.size, &font->file_atom),
						L"加载字体文件失败"
					);
				}
			}

			it = args.find(ATOM_SIZE);
			if (it != args.end()) {
				font->size = wcstoul(it->second.c_str(), nullptr, 10);
			}

			it = args.find(ATOM_STYLE);
			if (it != args.end()) {
				font->style = ConstToStyle(it->second.c_str());
			}

			it = args.find(ATOM_WEIGHT);
			if (it != args.end()) {
				font->SetWeight(wcstoul(it->second.c_str(), nullptr, 10));
			}

			return S_OK;
		}

	};







}
