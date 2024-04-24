/**
 * @file theme.h
 * @brief 主题对象内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */
#pragma once
#include "common/object_impl.hpp"

namespace ExDirectUI
{
	struct ExThemeElementClassInfo
	{
		EXATOM atom;
		std::wstring name;
		ExThemeElementClassInfo* base;
		std::unordered_map<EXATOM, ExVariant> attributes;
	};

	class ExTheme : public ExObjectImpl<IExTheme>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExTheme);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override;

		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return (void*)&m_classes;
			case 1: return (void*)&m_info;
			default: return __super::GetContext(index);
			}
		}

		ExTheme(const IExPackage* package);
		virtual ~ExTheme();

		void EXOBJCALL ParseElementNode(const IExPackage* package, pugi::xml_node& node) MAYTHROW;

	public:
		EXMETHOD HRESULT EXOBJCALL GetInfo(ExThemeInfo* r_info) const override;

		EXMETHOD bool EXOBJCALL HasClass(EXATOM atom_class) const override;
		EXMETHOD bool EXOBJCALL HasAttribute(EXATOM atom_class, EXATOM atom_attr, bool base = false) const override;

		EXMETHOD HRESULT EXOBJCALL GetAttribute(EXATOM atom_class, EXATOM atom_attr,
			ExVariant* r_attr, bool base = false) const override;
		EXMETHOD HRESULT EXOBJCALL GetAttributeData(EXATOM atom_class, EXATOM atom_attr,
			const void** r_attr_data, VARTYPE* r_attr_type = nullptr, bool base = false) const override;
		EXMETHOD HRESULT EXOBJCALL CopyAttribute(EXATOM atom_class, EXATOM atom_attr,
			ExVariant* r_attr, bool base = false) const override;
		EXMETHOD HRESULT EXOBJCALL DrawAttribute(IExCanvas* canvas, float left, float top,
			float right, float bottom, EXATOM atom_class, EXATOM atom_attr, DWORD state, 
			DWORD draw_mode, bool base = false) const override;

		EXMETHOD HRESULT EXOBJCALL EnumClasses(ExThemeEnumClassProc proc, LPARAM lparam = 0) const override;
		EXMETHOD HRESULT EXOBJCALL EnumAttributes(EXATOM class_atom,
			ExThemeEnumAttributeProc proc, DWORD base_mode = 0, LPARAM lparam = 0) const override;

	private:
		ExThemeInfo m_info{};
		std::unordered_map<EXATOM, ExThemeElementClassInfo> m_classes;
	};

}
