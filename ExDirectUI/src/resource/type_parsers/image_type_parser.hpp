/**
 * @file image_type_parser.hpp
 * @brief 图像类型解析对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-16
 * @copyright
 */
#pragma once
#include "src/resource/type_parser.h"

namespace ExDirectUI
{
	inline ExImageMode _ExImageParser_ConstToImageMode(LPCWSTR value)
	{
		static const DWORD _KV_IMAGE_MODE_[][2] = {
			{ATOM_DEFAULT, ExImageMode::Default},
			{ATOM_SCALE, ExImageMode::Scale},
			{ATOM_SCALE_CENTER, ExImageMode::ScaleCenter},
			{ATOM_SCALE_FILL, ExImageMode::ScaleFill},
			{ATOM_TILE, ExImageMode::Tile},
			{ATOM_MIRROR, ExImageMode::Mirror},
			{ATOM_CENTER, ExImageMode::Center},
			{ATOM_LEFT_TOP, ExImageMode::LeftTop},
			{ATOM_CENTER_TOP, ExImageMode::CenterTop},
			{ATOM_RIGHT_TOP, ExImageMode::RightTop},
			{ATOM_LEFT_MIDDLE, ExImageMode::LeftMiddle},
			{ATOM_CENTER_MIDDLE, ExImageMode::CenterMiddle},
			{ATOM_RIGHT_MIDDLE, ExImageMode::RightMiddle},
			{ATOM_LEFT_BOTTOM, ExImageMode::LeftBottom},
			{ATOM_CENTER_BOTTOM, ExImageMode::CenterBottom},
			{ATOM_RIGHT_BOTTOM, ExImageMode::RightBottom},
		};

		DWORD mode = ExImageMode::Default;
		ExParseToConst(value, _KV_IMAGE_MODE_, _countof(_KV_IMAGE_MODE_), &mode);
		return (ExImageMode)mode;
	}

	///////////////////////////

	EX_SAMPLE_TYPE_PARSER(Image, EVT_EXOBJECT, {
			auto image = V_EXOBJECT(V);
			return LoadPackageImage(owner, str, (IExImage**)&image);
		}
	);

	EX_SAMPLE_TYPE_PARSER(GridsImageInfo, EVT_GRIDS_IMAGE, {
			auto grids = V_GRIDS_IMAGE(V);
			return ExParseToGridsImageInfo(str, grids);
		}
	);

	//////////////////////

	class ExDisplayImageTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExDisplayImageTypeParser>
	{
	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_DISPLAY_IMAGE));
			auto image = V_DISPLAY_IMAGE(r_value);

			auto attr = node->attribute(L"file");
			handle_if_false(attr, EE_NOEXISTS, L"文件属性不存在");
			return_if_failed(LoadPackageImage(owner, attr.value(), &image->image));

			attr = node->attribute(L"src");
			if (attr) { ExParseToRectF(attr.value(), &image->src); }
			else {
				image->src = { 0, 0, (float)image->image->GetWidth(), (float)image->image->GetHeight() };
			}

			attr = node->attribute(L"padding");
			if (attr) { ExParseToRectF(attr.value(), &image->padding); }
			else { image->padding = { 0, 0, 0, 0 }; }

			attr = node->attribute(L"mode");
			image->image_mode = _ExImageParser_ConstToImageMode(attr.value());

			attr = node->attribute(L"show");
			ExParseToUInt32(attr.value(), (uint32_t*)&image->show_mode);

			attr = node->attribute(L"grids");
			if (attr) {
				ExParseToGridsImageInfo(attr.value(), &image->grids);
				image->image_mode = ExImageMode::Grids;
			}
			else { image->grids = {}; }

			attr = node->attribute(L"alpha");
			if (attr) { ExParseToByte(attr.value(), &image->alpha); }
			else { image->alpha = ALPHA_OPAQUE; }

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_DISPLAY_IMAGE));
			auto image = V_DISPLAY_IMAGE(r_value);
			auto args = GetArgsMap(str);

			LPCWSTR value = GetArg(args, ATOM_FILE);
			handle_if_false(value, EE_NOEXISTS, L"文件属性不存在");
			return_if_failed(LoadPackageImage(owner, value, &image->image));

			value = GetArg(args, ATOM_SRC);
			if (value) { ExParseToRectF(value, &image->src); }
			else {
				image->src = { 0, 0, (float)image->image->GetWidth(), (float)image->image->GetHeight() };
			}

			value = GetArg(args, ATOM_PADDING);
			if (value) { ExParseToRectF(value, &image->padding); }
			else { image->padding = { 0, 0, 0, 0 }; }

			value = GetArg(args, ATOM_MODE);
			if (value) {
				image->image_mode = _ExImageParser_ConstToImageMode(value);
			}
			else { image->image_mode = ExImageMode::Default; }

			value = GetArg(args, ATOM_SHOW);
			if (value) {
				ExParseToUInt32(value, (uint32_t*)&image->show_mode);
			}
			else { image->show_mode = ExImageShowMode::None; }

			value = GetArg(args, ATOM_GRIDS);
			if (value) {
				ExParseToGridsImageInfo(value, &image->grids);
				image->image_mode = ExImageMode::Grids;
			}
			else { image->grids = {}; }

			value = GetArg(args, ATOM_ALPHA);
			if (value) { ExParseToByte(value, &image->alpha); }
			else { image->alpha = ALPHA_OPAQUE; }

			return S_OK;
		}
	};

	/////////////////////

	class ExStateImageTypeParser : public IExTypeParser, public ExTypeParserHelper,
		public ExSingleton<ExStateImageTypeParser>
	{
	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_STATE_IMAGE));
			auto image = V_STATE_IMAGE(r_value);

			auto attr = node->attribute(L"file");
			handle_if_false(attr, EE_NOEXISTS, L"文件属性不存在");
			return_if_failed(LoadPackageImage(owner, attr.value(), &image->image));

			attr = node->attribute(L"normal");
			if (attr) { ExParseToRectF(attr.value(), &image->normal); }
			else {
				image->normal = { 0, 0, (float)image->image->GetWidth(), (float)image->image->GetHeight() };
			}

			attr = node->attribute(L"hover");
			if (attr) { ExParseToRectF(attr.value(), &image->hover); }
			else { image->hover = image->normal; }

			attr = node->attribute(L"press");
			if (attr) { ExParseToRectF(attr.value(), &image->press); }
			else { image->press = image->normal; }

			attr = node->attribute(L"focus");
			if (attr) { ExParseToRectF(attr.value(), &image->focus); }
			else { image->focus = image->normal; }

			attr = node->attribute(L"focus-blend");
			if (attr) { ExParseToRectF(attr.value(), &image->focus_blend); }
			else { image->focus_blend = {}; }

			attr = node->attribute(L"disable");
			if (attr) { ExParseToRectF(attr.value(), &image->disable); }
			else { image->disable = image->normal; }

			attr = node->attribute(L"mode");
			if (attr) { image->image_mode = _ExImageParser_ConstToImageMode(attr.value()); }
			else { image->image_mode = ExImageMode::Default; }

			attr = node->attribute(L"grids");
			if (attr) {
				ExParseToGridsImageInfo(attr.value(), &image->grids);
				image->image_mode = ExImageMode::Grids;
			}
			else { image->grids = {}; }

			attr = node->attribute(L"alpha");
			if (attr) { ExParseToByte(attr.value(), &image->alpha); }
			else { image->alpha = ALPHA_OPAQUE; }

			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{
			return_if_failed(ExVariantInit(r_value, EVT_STATE_IMAGE));
			auto image = V_STATE_IMAGE(r_value);
			auto args = GetArgsMap(str);
			auto value = GetArg(args, ATOM_FILE);
			handle_if_false(value, EE_NOEXISTS, L"文件属性不存在");
			handle_if_failed(
				LoadPackageImage(owner, value, &image->image),
				L"加载图像失败"
			);

			value = GetArg(args, ATOM_NORMAL);
			if (value) { ExParseToRectF(value, &image->normal); }
			else {
				image->normal = { 0, 0, (float)image->image->GetWidth(), (float)image->image->GetHeight() };
			}

			value = GetArg(args, ATOM_HOVER);
			if (value) { ExParseToRectF(value, &image->hover); }
			else { image->hover = image->normal; }

			value = GetArg(args, ATOM_PRESS);
			if (value) { ExParseToRectF(value, &image->press); }
			else { image->press = image->normal; }

			value = GetArg(args, ATOM_FOCUS);
			if (value) { ExParseToRectF(value, &image->focus); }
			else { image->focus = image->normal; }

			value = GetArg(args, ATOM_FOCUS_BLEND);
			if (value) { ExParseToRectF(value, &image->focus_blend); }
			else { image->focus_blend = {}; }

			value = GetArg(args, ATOM_DISABLE);
			if (value) { ExParseToRectF(value, &image->disable); }
			else { image->disable = {}; }

			value = GetArg(args, ATOM_MODE);
			if (value) {
				image->image_mode = _ExImageParser_ConstToImageMode(value);
			}
			else { image->image_mode = ExImageMode::Default; }

			value = GetArg(args, ATOM_GRIDS);
			if (value) {
				ExParseToGridsImageInfo(value, &image->grids);
				image->image_mode = ExImageMode::Grids;
			}
			else { image->grids = {}; }

			value = GetArg(args, ATOM_ALPHA);
			if (value) { ExParseToByte(value, &image->alpha); }
			else { image->alpha = ALPHA_OPAQUE; }

			return S_OK;
		}
	};


}
