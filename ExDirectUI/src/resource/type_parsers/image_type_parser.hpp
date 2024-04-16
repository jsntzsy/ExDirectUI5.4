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

	inline DWORD _ExImageParser_ConstToImageMode(LPCWSTR value)
	{
		static const DWORD _KV_IMAGE_MODE_ [][2] = {
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
		return mode;
	}

	///////////////////////////

	EX_SAMPLE_TYPE_PARSER(Image, EVT_EXOBJECT, {
			auto image = V_EXOBJECT(V);
			return _ExParser_LoadPackageImage(owner, str, (IExImage**)&image);
		}
	);

	class ExDisplayImageTypeParser : public IExTypeParser, public ExSingleton<ExDisplayImageInfo>
	{
	public:
		
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node,
			IUnknown* owner, ExVariant* r_value) override
		{

		}
		
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, IUnknown* owner,
			ExVariant* r_value) override
		{

		}
	};

}
