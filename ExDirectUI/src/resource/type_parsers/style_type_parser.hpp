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
	class ExEleStyleTypeParser : public IExTypeParser, ExTypeParserHelper,
		public ExSingleton<ExEleStyleTypeParser>
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
			return_if_failed(ExVariantInit(r_value, EVT_ELE_STYLE));
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
			return_if_failed(ExVariantInit(r_value, EVT_ELE_STYLE));
			auto style = V_ELE_STYLE(r_value);
			auto args = GetArgsMap(str);

			auto value = GetArg(args, ATOM_COMMON);
			if (value) { style->common = _ToCommonStyle(value); }

			value = GetArg(args, ATOM_CONTROL);
			if (value) { style->common = _ToControlStyle(value); }

			value = GetArg(args, ATOM_TEXT_FORMAT);
			if (value) { style->common = _ToTextFormat(value); }

			return S_OK;
		}

	};

	/////////////////////

	class ExShadowTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExShadowTypeParser>
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
			return_if_failed(ExVariantInit(r_value, EVT_ELE_SHADOW));
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
				return_if_failed(LoadPackageImage(owner, attr.value(), &texture.image));

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

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			auto shadow = V_ELE_SHADOW(r_value);
			auto args = GetArgsMap(str);

			auto value = GetArg(args, ATOM_TYPE);
			if (value) {
				shadow->type = _ToShadowType(value);

				if (shadow->type == ExEleShadowType::Param) {
					auto& param = shadow->info.param;
					value = GetArg(args, ATOM_SIZE);
					if (value) { param.size = wcstof(value, nullptr); }
					if (param.size > 0) {
						value = GetArg(args, ATOM_NORMAL);
						if (value) { ExParseToColor(value, &param.normal); }
						else { param.normal = COLOR_UNDEFINE; }

						value = GetArg(args, ATOM_ACTIVE);
						if (value) { ExParseToColor(value, &param.active); }
						else { param.active = param.normal; }

						value = GetArg(args, ATOM_OFFSET);
						if (value) { ExParseToPointF(value, &param.offset); }
						else { param.offset = {}; }

						value = GetArg(args, ATOM_PADDING);
						if (value) { ExParseToRectF(value, &param.padding); }
						else { param.padding = {}; }
					}
					else { shadow->type = ExEleShadowType::None; }
				}
				else if (shadow->type == ExEleShadowType::Texture) {
					auto& texture = shadow->info.texture;

					value = GetArg(args, ATOM_FILE);
					handle_if_false(value, EE_NOEXISTS, L"未找到图像文件");
					return_if_failed(LoadPackageImage(owner, value, &texture.image));

					value = GetArg(args, ATOM_NORMAL);
					if (value) { ExParseToRectF(value, &texture.src_normal); }
					else {
						texture.src_normal = {
							0,0,
							(float)texture.image->GetWidth(),
							(float)texture.image->GetHeight(),
						};
					}

					value = GetArg(args, ATOM_ACTIVE);
					if (value) { ExParseToRectF(value, &texture.src_active); }
					else { texture.src_active = texture.src_normal; }

					value = GetArg(args, ATOM_ALPHA_NORMAL);
					if (value) { ExParseToByte(value, &texture.alpha_normal); }
					else { texture.alpha_normal = ALPHA_OPAQUE; }

					value = GetArg(args, ATOM_ALPHA_ACTIVE);
					if (value) { ExParseToByte(value, &texture.alpha_active); }
					else { texture.alpha_active = texture.alpha_normal; }

					value = GetArg(args, ATOM_GRIDS);
					if (value) { ExParseToGridsImageInfo(value, &texture.grids); }
					else { texture.grids = {}; }
				}
				else { shadow->type = ExEleShadowType::None; }
			}
			else { shadow->type = ExEleShadowType::None; }

			return S_OK;
		}

	};

}
