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

namespace ExDirectUI
{
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
		do{
			
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
		float right, float bottom, EXATOM atom_class, EXATOM atom_attr, DWORD state, bool base) const
	{
		CHECK_PARAM(atom_class != EXATOM_UNKNOWN);
		CHECK_PARAM(atom_attr != EXATOM_UNKNOWN);
		CHECK_PARAM(canvas);

		ExVariant var{};
		return_if_failed(GetAttribute(atom_class, atom_attr, &var, base));
		return ExVariantDraw(canvas, left, top, right, bottom, &var, state, 0);
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

}


