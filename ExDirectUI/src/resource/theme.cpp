/**
 * @file theme.cpp
 * @brief 主题对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */

#include "stdafx.h"
#include "resource/theme.h"
#include "resource/package.h"
#include "src/resource/theme.h"
#include "common/object_impl.hpp"
#include "common/string.hpp"

#ifdef new
#undef new
#endif // new

#include "../ThirdParts/pugixml/pugixml.hpp"

namespace ExDirectUI
{
	ExTheme::ExTheme(const IExPackage* package)
	{
		ExPackageInfo info{};
		throw_if_failed(package->GetPackageInfo(&info), L"获取数据包信息失败");
		throw_if_false(info.type == ExPackageType::ThemePackage, EE_UNMATCH, L"数据包不是主题包");

		ExPackageItemInfo xml_file{};
		throw_if_failed(
			package->GetItemInfo(ExAtom(L"theme.xml"), &xml_file),
			L"获取主题描述文件失败"
		);

		pugi::xml_document xml;
		auto result = xml.load_buffer(xml_file.data, xml_file.size);
		throw_if_false(result, EE_FORMAT, L"解析主题描述文件失败");

		pugi::xml_node root = xml.child(L"Theme");
		throw_if_false(root, EE_FORMAT, L"主题描述文件格式错误");

#pragma region 解析主题包信息

		pugi::xml_attribute attr = root.attribute(L"name");
		throw_if_false(attr, EE_FORMAT, L"缺少必要的属性");
		wcscpy_s(m_info.name, attr.value());

		attr = root.attribute(L"version");
		if (attr) { wcscpy_s(m_info.version, attr.value()); }
		else { m_info.version[0] = L'\0'; }

		attr = root.attribute(L"author");
		if (attr) { wcscpy_s(m_info.author, attr.value()); }
		else { m_info.author[0] = L'\0'; }

		attr = root.attribute(L"description");
		if (attr) { wcscpy_s(m_info.description, attr.value()); }
		else { m_info.description[0] = L'\0'; }

#pragma endregion

		auto children = root.children();
		for (auto ele : children) {
			try
			{
				ParseElementNode(package, ele);
			}
			catch_continue({});
		}

	}

	ExTheme::~ExTheme()
	{
		for (auto ele : m_classes) {
			for (auto attr : ele.second.attributes) {
				ExVariantClear(&attr.second);
			}
		}
	}

	void EXOBJCALL ExTheme::ParseElementNode(const IExPackage* package, pugi::xml_node& node)
	{
		ExThemeElementClassInfo ele{};
		ele.name = node.name();
		ele.atom = ExAtom(ele.name.c_str());

		auto attr = node.attribute(L"base");
		if (attr) {
			EXATOM atom_base = ExAtom(attr.value());
			auto base_ele = m_classes.find(atom_base);
			if (base_ele != m_classes.end()) {
				ele.base = &base_ele->second;
			}
		}

		auto attrs = node.children();
		for (auto anode : attrs) {
			attr = anode.attribute(L"name");
			if (attr) {
				EXATOM atom_name = ExAtom(attr.value());
				ExVariant value{};
				if (SUCCEEDED(ExParseFromXmlNode(&anode, (IUnknown*)package, &value))) {
					ele.attributes[atom_name] = value;
				}
				else {
					std::wstringstream ss;
					ss << L"未知的主题属性: ";
					anode.print(ss);
					ExDbgOutput(ss.str().c_str());
				}
			}
		}

		m_classes[ele.atom] = ele;
	}

	HRESULT EXOBJCALL ExTheme::GetInfo(ExThemeInfo* r_info) const
	{
		CHECK_PARAM(r_info);
		*r_info = m_info;
		return S_OK;
	}

	bool EXOBJCALL ExTheme::HasClass(EXATOM atom_class) const
	{
		CHECK_PARAM_RET(atom_class != EXATOM_UNKNOWN, false);
		return m_classes.find(atom_class) != m_classes.end();
	}

	bool EXOBJCALL ExTheme::HasAttribute(EXATOM atom_class, EXATOM atom_attr, bool base) const
	{
		CHECK_PARAM_RET(atom_class != EXATOM_UNKNOWN, false);
		CHECK_PARAM_RET(atom_attr != EXATOM_UNKNOWN, false);

		auto it = m_classes.find(atom_class);
		if (it == m_classes.end()) { return false; }

		auto cls_info = &it->second;
		do {

			if (cls_info->attributes.find(atom_attr) != cls_info->attributes.end()) {
				return true;
			}

			cls_info = cls_info->base;
		} while (base && cls_info);
		return false;
	}

	HRESULT EXOBJCALL ExTheme::GetAttribute(EXATOM atom_class, EXATOM atom_attr, ExVariant* r_attr, bool base) const
	{
		CHECK_PARAM(atom_class != EXATOM_UNKNOWN);
		CHECK_PARAM(atom_attr != EXATOM_UNKNOWN);
		CHECK_PARAM(r_attr);

		auto cls = m_classes.find(atom_class);
		handle_if_false(cls != m_classes.end(), EE_NOEXISTS, L"未找到指定的元素类");

		auto cls_info = &cls->second;
		do {
			auto attr = cls_info->attributes.find(atom_attr);
			if (attr != cls_info->attributes.end()) {
				*r_attr = attr->second;
				return S_OK;
			}
			cls_info = cls_info->base;
		} while (base && cls_info);

		handle_ex(EE_NOEXISTS, L"未找到指定的属性");
	}

	HRESULT EXOBJCALL ExTheme::GetAttributeData(EXATOM atom_class, EXATOM atom_attr, const void** r_attr_data,
		VARTYPE* r_attr_type, bool base) const
	{
		CHECK_PARAM(r_attr_data);
		
		ExVariant var{};
		return_if_failed(GetAttribute(atom_class, atom_attr, &var, base));
		return_if_failed(ExVariantGetValuePtr(&var, (void**)r_attr_data), *r_attr_data = nullptr);
		if (r_attr_type) { *r_attr_type = V_VT(&var); }

		return S_OK;
	}

	HRESULT EXOBJCALL ExTheme::CopyAttribute(EXATOM atom_class, EXATOM atom_attr, ExVariant* r_attr, bool base) const
	{
		CHECK_PARAM(atom_class != EXATOM_UNKNOWN);
		CHECK_PARAM(atom_attr != EXATOM_UNKNOWN);
		CHECK_PARAM(r_attr);

		auto cls = m_classes.find(atom_class);
		handle_if_false(cls != m_classes.end(), EE_NOEXISTS, L"未找到指定的元素类");

		auto cls_info = &cls->second;
		do {
			auto attr = cls_info->attributes.find(atom_attr);
			if (attr != cls_info->attributes.end()) {
				return ExVariantCopy(r_attr, &attr->second);
			}
			cls_info = cls_info->base;
		} while (base && cls_info);

		handle_ex(EE_NOEXISTS, L"未找到指定的属性");
	}

	HRESULT EXOBJCALL ExTheme::DrawAttribute(IExCanvas* canvas, float left, float top,
		float right, float bottom, EXATOM atom_class, EXATOM atom_attr, DWORD state, DWORD draw_mode, bool base) const
	{
		CHECK_PARAM(atom_class != EXATOM_UNKNOWN);
		CHECK_PARAM(atom_attr != EXATOM_UNKNOWN);
		CHECK_PARAM(canvas);

		ExVariant var{};
		return_if_failed(GetAttribute(atom_class, atom_attr, &var, base));
		return ExVariantDraw(canvas, left, top, right, bottom, &var, state, draw_mode, 0);
	}

	HRESULT EXOBJCALL ExTheme::EnumClasses(ExThemeEnumClassProc proc, LPARAM lparam) const
	{
		CHECK_PARAM(proc);
		for (auto& cls : m_classes) {
			EXATOM base_atom = cls.second.base ? cls.second.base->atom : EXATOM_UNKNOWN;
			if (!proc((IExTheme*)this, cls.first, cls.second.name.c_str(), base_atom, lparam)) {
				return S_FALSE;
			}
		}
		return S_OK;
	}

	HRESULT EXOBJCALL ExTheme::EnumAttributes(EXATOM class_atom, ExThemeEnumAttributeProc proc, DWORD base_mode, LPARAM lparam) const
	{
		CHECK_PARAM(class_atom != EXATOM_UNKNOWN);
		CHECK_PARAM(proc);

		auto cls = m_classes.find(class_atom);
		handle_if_false(cls != m_classes.end(), EE_NOEXISTS, L"未找到指定的元素类");

		//base_mode 0:只枚举当前类的属性 1:从当前类向上枚举所有基类 2:从先组基类向下枚举所有属性

		//生成需要枚举的属性类列表
		std::list<const ExThemeElementClassInfo*> classes;
		auto cls_info = &cls->second;
		do {
			if (base_mode == 1) {
				classes.push_back(cls_info);
			}
			else if (base_mode == 2) {
				classes.push_front(cls_info);
			}
			else {
				classes.push_back(cls_info);
				break;
			}

			cls_info = cls_info->base;
		} while (cls_info);

		//枚举属性
		for (auto cls_info : classes) {
			for (auto& attr : cls_info->attributes) {
				if (!proc((IExTheme*)this, class_atom, attr.first, &attr.second, lparam)) {
					return S_FALSE;
				}
			}
		}

		return S_OK;
	}

	////////////////////////////


	HRESULT EXAPI EXCALL ExThemeLoad(const byte_t* package_data, uint32_t package_size, const byte_t* key, uint32_t key_size, IExTheme** r_theme)
	{
		CHECK_PARAM(package_data);
		CHECK_PARAM(r_theme);

		ExAutoPtr<IExPackage> package;
		handle_if_failed(
			ExPackageLoad(package_data, package_size, key, key_size, &package),
			L"加载数据包失败"
		);
		try
		{
			ExAutoPtr<ExTheme> theme = new ExTheme(package);
			return theme->QueryInterface(r_theme);
		}
		catch_default({});
	}

	HRESULT EXAPI EXCALL ExThemeLoadFromPackage(const IExPackage* package, IExTheme** r_theme)
	{
		try
		{
			ExAutoPtr<ExTheme> theme = new ExTheme(package);
			return theme->QueryInterface(r_theme);
		}
		catch_default({});
	}

}


