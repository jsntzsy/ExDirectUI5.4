/**
 * @file style_type_parser.hpp
 * @brief 风格类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-18
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	class ExEleStyleTypeParser : public IExTypeParser, public ExSingleton<ExEleStyleTypeParser>
	{
		inline DWORD _ToCommonStyle(LPCWSTR str) {
			uint32_t value = 0;
			ExParseToUInt32(str, &value);
			return value;
		}
		inline DWORD _ToControlStyle(LPCWSTR str) {
			uint32_t value = 0;
			ExParseToUInt32(str, &value);
			return value;
		}
		inline DWORD _ToTextFormat(LPCWSTR str) {
			uint32_t value = 0;
			ExParseToUInt32(str, &value);
			return value;
		}

	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			auto style = V_ELE_STYLE(r_value);

			auto attr = node->attribute(L"common");
			if (attr) { style->common = _ToCommonStyle(attr.value()); }

			attr = node->attribute(L"control");
			if (attr) { style->control = _ToControlStyle(attr.value()); }

			attr = node->attribute(L"text-format");
			if (attr) { style->text_format = _ToTextFormat(attr.value()); }

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			auto style = V_ELE_STYLE(r_value);
			auto args = _ExParser_GetArgsMap(str);

			auto it = args.find(ATOM_COMMON);
			if (it != args.end()) {
				style->common = _ToCommonStyle(it->second.c_str());
			}

			it = args.find(ATOM_CONTROL);
			if (it != args.end()) {
				style->common = _ToControlStyle(it->second.c_str());
			}

			it = args.find(ATOM_TEXT_FORMAT);
			if (it != args.end()) {
				style->common = _ToTextFormat(it->second.c_str());
			}

			return S_OK;
		}

	};

	/////////////////////

	class ExShadowTypeParser : public IExTypeParser, ExSingleton<ExShadowTypeParser>
	{
		inline DWORD _ToShadowType(LPCWSTR str)
		{
			uint32_t type = 0;
			ExParseToUInt32(str, &type);
			return type;
		}

	public:
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			auto shadow = V_ELE_SHADOW(r_value);

			auto attr = node->attribute(L"type");
			if (attr) { shadow->type = _ToShadowType(attr.value()); }

			if (shadow->type == ExEleShadowType::Param) {
				auto& param = shadow->info.param;
				param.size = node->attribute(L"size").as_float();
				if (param.size > 0) {

					attr = node->attribute(L"normal");
					if (attr) { ExParseToColor(attr.value(), &param.normal); }
					else { param.normal = COLOR_UNDEFINE; }

					attr = node->attribute(L"active");
					if (attr) { ExParseToColor(attr.value(), &param.active); }
					else { param.active = param.normal; }

					attr = node->attribute(L"offset");
					if (attr) { ExParseToPointF(attr.value(), &param.offset); }
					else { param.offset = {}; }

					attr = node->attribute(L"padding");
					if (attr) { ExParseToRectF(attr.value(), &param.padding); }
					else { param.padding = {}; }
				}
				else { shadow->type = ExEleShadowType::None; }
			}
			else if (shadow->type == ExEleShadowType::Texture) {
				auto& texture = shadow->info.texture;

				attr = node->attribute(L"file");
				handle_if_false(attr, EE_NOEXISTS, L"未找到图像文件");
				return_if_failed(
					_ExParser_LoadPackageImage(owner, attr.value(), &texture.image)
				);

				if (texture.image) {
					attr = node->attribute(L"normal");
					if (attr) { ExParseToRectF(attr.value(), &texture.src_normal); }
					else {
						texture.src_normal = {
							0,0,
							(float)texture.image->GetWidth(),
							(float)texture.image->GetHeight()
						};
					}

					attr = node->attribute(L"active");
					if (attr) { ExParseToRectF(attr.value(), &texture.src_active); }
					else { texture.src_active = texture.src_normal; }

					attr = node->attribute(L"alpha-normal");
					if (attr) { ExParseToByte(attr.value(), &texture.alpha_normal); }
					else { texture.alpha_normal = ALPHA_OPAQUE; }

					attr = node->attribute(L"alpha-active");
					if (attr) { ExParseToByte(attr.value(), &texture.alpha_active); }
					else { texture.alpha_active = ALPHA_OPAQUE; }

					attr = node->attribute(L"grids");
					if (attr) {
						ExParseToGridsImageInfo(attr.value(), &texture.grids);
					}
					else { texture.grids = {}; }

				}
			}

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			auto shadow = V_ELE_SHADOW(r_value);
			auto args = _ExParser_GetArgsMap(str);

			auto it = args.find(ATOM_TYPE);
			if (it != args.end()) {
				shadow->type = _ToShadowType(it->second.c_str());

				//TODO

			}
			else { shadow->type = ExEleShadowType::None; }

			return S_OK;
		}

	};

}
