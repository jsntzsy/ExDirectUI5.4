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
	inline HRESULT _ExParser_LoadPackageImage(IUnknown* owner, LPCWSTR path, IExImage** r_image)
	{
		HRESULT hr;
		ExPackageItemInfo item{};
		return_if_failed(_ExParser_GetPackageItem(owner, path, &item));
		if (item.type == ExPackageItemType::ImageData) {
			hr = ExImageCreateFromImageData(item.data, r_image);
		}
		else if (item.type == ExPackageItemType::Icon) {
			HICON icon = NULL;
			return_if_failed(ExImageLoadObject(item.data, item.size, IMAGE_ICON, 0, (HGDIOBJ*)&icon));
			hr = ExImageCreateFromHICON(icon, r_image);
			DestroyIcon(icon);
		}
		else { hr = ExImageCreateFromMemory(item.data, item.size, r_image); }
		return hr;
	}

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
			return _ExParser_LoadPackageImage(owner, str, (IExImage**)&image);
		}
	);

	EX_SAMPLE_TYPE_PARSER(GridsImageInfo, EVT_GRIDS_IMAGE, {
			auto grids = V_GRIDS_IMAGE(V);
			return ExParseToGridsImageInfo(str, grids);
		}
	);

	//////////////////////

	class ExDisplayImageTypeParser : public IExTypeParser, public ExSingleton<ExDisplayImageTypeParser>
	{
	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			auto image = V_DISPLAY_IMAGE(r_value);

			auto attr = node->attribute(L"file");
			handle_if_false(attr, EE_NOEXISTS, L"文件属性不存在");

			handle_if_failed(
				_ExParser_LoadPackageImage(owner, attr.value(), &image->image),
				L"加载图像失败"
			);

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
			auto image = V_DISPLAY_IMAGE(r_value);
			auto args = _ExParser_GetArgsMap(str);
			auto it = args.find(ATOM_FILE);
			handle_if_false(it != args.end(), EE_NOEXISTS, L"文件属性不存在");
			handle_if_failed(
				_ExParser_LoadPackageImage(owner, it->second.c_str(), &image->image),
				L"加载图像失败"
			);

			it = args.find(ATOM_SRC);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->src); }
			else {
				image->src = { 0, 0, (float)image->image->GetWidth(), (float)image->image->GetHeight() };
			}

			it = args.find(ATOM_PADDING);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->padding); }
			else { image->padding = { 0, 0, 0, 0 }; }

			it = args.find(ATOM_MODE);
			if (it != args.end()) {
				image->image_mode = _ExImageParser_ConstToImageMode(it->second.c_str());
			}
			else { image->image_mode = ExImageMode::Default; }

			it = args.find(ATOM_SHOW);
			if (it != args.end()) {
				ExParseToUInt32(it->second.c_str(), (uint32_t*)&image->show_mode);
			}
			else { image->show_mode = ExImageShowMode::None; }

			it = args.find(ATOM_GRIDS);
			if (it != args.end()) {
				ExParseToGridsImageInfo(it->second.c_str(), &image->grids);
				image->image_mode = ExImageMode::Grids;
			}
			else { image->grids = {}; }

			it = args.find(ATOM_ALPHA);
			if (it != args.end()) { ExParseToByte(it->second.c_str(), &image->alpha); }
			else { image->alpha = ALPHA_OPAQUE; }

			return S_OK;
		}
	};

	/////////////////////

	class ExStateImageTypeParser : public IExTypeParser, public ExSingleton<ExStateImageTypeParser>
	{
	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{
			auto image = V_STATE_IMAGE(r_value);

			auto attr = node->attribute(L"file");
			handle_if_false(attr, EE_NOEXISTS, L"文件属性不存在");

			handle_if_failed(
				_ExParser_LoadPackageImage(owner, attr.value(), &image->image),
				L"加载图像失败"
			);

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
			auto image = V_STATE_IMAGE(r_value);
			auto args = _ExParser_GetArgsMap(str);
			auto it = args.find(ATOM_FILE);
			handle_if_false(it != args.end(), EE_NOEXISTS, L"文件属性不存在");
			handle_if_failed(
				_ExParser_LoadPackageImage(owner, it->second.c_str(), &image->image),
				L"加载图像失败"
			);

			it = args.find(ATOM_NORMAL);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->normal); }
			else {
				image->normal = { 0, 0, (float)image->image->GetWidth(), (float)image->image->GetHeight() };
			}

			it = args.find(ATOM_HOVER);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->hover); }
			else { image->hover = image->normal; }

			it = args.find(ATOM_PRESS);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->press); }
			else { image->press = image->normal; }

			it = args.find(ATOM_FOCUS);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->focus); }
			else { image->focus = image->normal; }

			it = args.find(ATOM_FOCUS_BLEND);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->focus_blend); }
			else { image->focus_blend = {}; }

			it = args.find(ATOM_DISABLE);
			if (it != args.end()) { ExParseToRectF(it->second.c_str(), &image->disable); }
			else { image->disable = {}; }

			it = args.find(ATOM_MODE);
			if (it != args.end()) {
				image->image_mode = _ExImageParser_ConstToImageMode(it->second.c_str());
			}
			else { image->image_mode = ExImageMode::Default; }

			it = args.find(ATOM_GRIDS);
			if (it != args.end()) { 
				ExParseToGridsImageInfo(it->second.c_str(), &image->grids);
				image->image_mode = ExImageMode::Grids;
			}
			else { image->grids = {}; }

			it = args.find(ATOM_ALPHA);
			if (it != args.end()) { ExParseToByte(it->second.c_str(), &image->alpha); }
			else { image->alpha = ALPHA_OPAQUE; }

			return S_OK;
		}
	};


}
