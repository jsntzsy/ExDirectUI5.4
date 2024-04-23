/**
 * @file type_parser.h
 * @brief 类型解析内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once
#include "common/singleton.hpp"
#include "common/string.hpp"
#include "resource/package.h"

#ifdef new
#undef new
#endif
#include "../ThirdParts/pugixml/pugixml.hpp"

#define _map_get_or_default(map, key, def)	(map.find(key) != map.end() ? map[key] : def)

namespace ExDirectUI
{
	class ExTypeParserHelper
	{
	public:
		static std::unordered_map<EXATOM, std::wstring> GetArgsMap(LPCWSTR str, 
			LPCWSTR arg_delim = L" ", LPCWSTR kv_delim = L":")
		{
			std::unordered_map<EXATOM, std::wstring> map;
			auto args = ExString::split(str, arg_delim);
			wstring key, value;
			for (auto& arg : args) {
				if (ExString::slice(arg, kv_delim, key, value)) {
					EXATOM atom_key = ExAtom(key.c_str());
					if (atom_key != EXATOM_UNKNOWN) {
						map[atom_key] = value;
					}
				}
			}
			return map;
		}

		inline static std::wstring GetArg(pugi::xml_node* node, LPCWSTR attr_name, std::wstring default_value = L"")
		{
			auto attr = node->attribute(attr_name);
			if (attr) { return attr.value(); }
			else { return default_value; }
		}

		inline static std::wstring GetArg(std::unordered_map<EXATOM, wstring> map, EXATOM atom, std::wstring default_value = L"")
		{
			auto it = map.find(atom);
			if (it != map.end()) { return it->second; }
			else { return default_value; }
		}

		inline static HRESULT GetPackageItem(IUnknown* owner, LPCWSTR path, ExPackageItemInfo* r_item)
		{
			if (!owner) { return E_INVALIDARG; }
			if (path == nullptr || *path == L'\0') { return E_INVALIDARG; }

			ExAutoPtr<IExPackage> package;
			return_if_failed(owner->QueryInterface(&package));
			EXATOM key = package->FindItem(path);
			if (key == EXATOM_UNKNOWN) { return EE_NOEXISTS; }
			return package->GetItemInfo(key, r_item);
		}

		inline static HRESULT LoadPackageImage(IUnknown* owner, LPCWSTR path, IExImage** r_image)
		{
			HRESULT hr;
			ExPackageItemInfo item{};
			return_if_failed(GetPackageItem(owner, path, &item));
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

	};

	class ExStandardTypeParser : public IExTypeParser, public ExTypeParserHelper
	{
	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node, IUnknown* owner, ExVariant* r_value) override
		{
			LPCWSTR value = L"";
			pugi::xml_attribute attr = node->attribute(L"value");
			if (attr) { value = attr.value(); }
			else { value = node->text().get(); }
			return this->ParseFromString(type, value, owner, r_value);
		}

	};

	////////////////////

#define EX_SAMPLE_TYPE_PARSER(name, vt, todo) 													\
	class Ex##name##TypeParser : public ExStandardTypeParser,									\
		public ExSingleton<Ex##name##TypeParser> {												\
	public:																						\
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,					\
			IUnknown* owner, ExVariant* V) override {											\
			return_if_failed(ExVariantInit(V,vt));												\
			todo;																				\
			return S_OK;																		\
		}																						\
	};																							


#define EX_CONSTS_TYPE_PARSER(name, multiple, count, todo) 										\
	class Ex##name##TypeParser : public ExStandardTypeParser,									\
		public ExSingleton<Ex##name##TypeParser> {												\
		static const DWORD _KV_[count][2];														\
	public:																						\
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,					\
			IUnknown* owner, ExVariant* V) override {											\
			return_if_failed(ExVariantInit(V,EVT_FLAGS), ExVariantClear(V));					\
			todo;																				\
			if (multiple) { return ExParseToConsts(str, _KV_, count, (DWORD*)&V_UI4(V)); }		\
			else { return ExParseToConst(str, _KV_, count, (DWORD*)&V_UI4(V)); }				\
			return S_OK; 																		\
		}																						\
	};																							\
	const DWORD Ex##name##TypeParser::_KV_[count][2] =															




	//////////////////////


}
