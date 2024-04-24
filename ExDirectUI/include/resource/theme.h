/**
 * @file theme.h
 * @brief 主题对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 主题包信息
	struct ExThemeInfo
	{
		DWORD flags;

		wchar_t name[64];

		wchar_t author[64];

		wchar_t version[32];

		wchar_t description[256];
	};

	interface IExTheme;
	typedef VARIANT ExVariant;

	typedef bool (CALLBACK* ExThemeEnumClassProc)(IExTheme* theme, EXATOM class_atom,
		LPCWSTR class_name, EXATOM base_class, LPARAM lparam);
	typedef bool (CALLBACK* ExThemeEnumAttributeProc)(IExTheme* theme, EXATOM class_atom,
		EXATOM attribute_atom, const ExVariant* value, LPARAM lparam);

	//////////////////////

	interface IExClass;

	EXINTERFACE("366C621E-6225-4458-B2A9-3788D8086920") IExTheme : public IExObject
	{
		EXMETHOD HRESULT EXOBJCALL GetInfo(ExThemeInfo * r_info) const PURE;

		EXMETHOD bool EXOBJCALL HasClass(EXATOM atom_class) const PURE;
		EXMETHOD bool EXOBJCALL HasAttribute(EXATOM atom_class , EXATOM atom_attr, bool base = false) const PURE;

		EXMETHOD HRESULT EXOBJCALL GetAttribute(EXATOM atom_class, EXATOM atom_attr,
			ExVariant* r_attr, bool base = false) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetAttributeData(EXATOM atom_class, EXATOM atom_attr,
			const void** r_attr_data, VARTYPE* r_attr_type = nullptr, bool base = false) const PURE;
		EXMETHOD HRESULT EXOBJCALL CopyAttribute(EXATOM atom_class, EXATOM atom_attr,
			ExVariant* r_attr, bool base = false) const PURE;
		EXMETHOD HRESULT EXOBJCALL DrawAttribute(IExCanvas* canvas, float left, float top,
			float right, float bottom, EXATOM atom_class, EXATOM atom_attr, DWORD state,
			DWORD draw_mode, bool base = false) const PURE;

		EXMETHOD HRESULT EXOBJCALL EnumClasses(ExThemeEnumClassProc proc, LPARAM lparam = 0) const PURE;
		EXMETHOD HRESULT EXOBJCALL EnumAttributes(EXATOM class_atom,
			ExThemeEnumAttributeProc proc, DWORD base_mode = 0, LPARAM lparam = 0) const PURE;

	};

	/////////////////////////////////

	HRESULT EXAPI EXCALL ExThemeLoad(const byte_t* package_data, uint32_t package_size,
		const byte_t* key, uint32_t key_size, IExTheme** r_theme);

	HRESULT EXAPI EXCALL ExThemeLoadFromPackage(const IExPackage* package, IExTheme** r_theme);

}
