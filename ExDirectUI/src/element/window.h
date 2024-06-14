/**
 * @file window.h
 * @brief 皮肤窗口内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-05-04
 * @copyright
 */
#pragma once
#include "src/element/element.h"
#include "common/object_impl.hpp"
#include "element/window.h"

namespace ExDirectUI
{
	struct ExWindowFlags
	{
		bool validate : 1;
	};


	class ExWindow : public ExElementBase<ExObjectImpl<IExWindow>>
	{
	protected:
		static LRESULT CALLBACK OnMessageDispatch(void* ptr, uint32_t message, WPARAM wparam, LPARAM lparam);
		virtual LRESULT EXOBJCALL OnMessage(uint32_t message, WPARAM wparam, LPARAM lparam, bool& r_handled);

	public:

	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExElement);
		EX_INTERFACE_ENTRY(IExWindow);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD BSTR EXOBJCALL ToString() const override
		{
			return ExSysAllocStringFormat(
				L"ExWindow(window_handle: 0x%p)",
				m_handle
			);
		}

		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return (void*)m_handle;
			default: return __super::GetContext(index);
			}
		}


	public:
		ExWindow(HWND window, DWORD style);
		virtual EXOBJCALL ~ExWindow();

		EXMETHOD LRESULT EXOBJCALL SendMessage(uint32_t message, WPARAM wparam, LPARAM lparam) override;
		EXMETHOD LRESULT EXOBJCALL PostMessage(uint32_t message, WPARAM wparam, LPARAM lparam) override;
		EXMETHOD LRESULT EXOBJCALL DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam) override;
		EXMETHOD LRESULT EXOBJCALL DispatchMessage(uint32_t message, WPARAM wparam, LPARAM lparam) override;

		EXMETHOD bool EXOBJCALL IsVisible() override;
		EXMETHOD HRESULT EXOBJCALL SetVisible(bool visible) override;
		EXMETHOD bool EXOBJCALL IsEnable() override;
		EXMETHOD HRESULT EXOBJCALL SetEnable(bool enable) override;
		EXMETHOD bool EXOBJCALL IsFocus() override;
		EXMETHOD HRESULT EXOBJCALL SetFocus() override;
		EXMETHOD HRESULT EXOBJCALL KillFocus() override;

		EXMETHOD bool EXOBJCALL IsRedrawable() override;
		EXMETHOD HRESULT EXOBJCALL SetRedrawable(bool redrawable) override;
		EXMETHOD HRESULT EXOBJCALL Invalidate(RECT* invalidate_rect) override;
		EXMETHOD HRESULT EXOBJCALL Update() override;

		EXMETHOD HRESULT EXOBJCALL GetAttribute(int index, LONG_PTR* r_value) override;
		EXMETHOD HRESULT EXOBJCALL SetAttribute(int index, LONG_PTR r_value, LONG_PTR* r_old_value) override;

		EXMETHOD uint32_t EXOBJCALL GetTextLength() override;
		EXMETHOD HRESULT EXOBJCALL GetText(wchar_t* r_text, uint32_t text_length) override;
		EXMETHOD HRESULT EXOBJCALL GetTextBSTR(BSTR* r_text) override;
		EXMETHOD HRESULT EXOBJCALL SetText(LPCWSTR text, bool redraw = true) override;

		EXMETHOD HRESULT EXOBJCALL GetFont(IExFont** r_font) override;
		EXMETHOD HRESULT EXOBJCALL SetFont(IExFont* font, bool redraw = true) override;
		EXMETHOD HRESULT EXOBJCALL SetFontFromInfo(ExFontInfo font_info) override;

		EXMETHOD HRESULT EXOBJCALL GetColor(int index, EXARGB* r_color) override;
		EXMETHOD HRESULT EXOBJCALL SetColor(int index, EXARGB color, bool redraw = true) override;

		EXMETHOD HRESULT EXOBJCALL GetBackgroundImage(ExDisplayImageInfo* r_image) override;
		EXMETHOD HRESULT EXOBJCALL SetBackgroundImage(ExDisplayImageInfo* image, bool redraw) override;
		EXMETHOD HRESULT EXOBJCALL SetBackgroundImageState(bool play, int frame, bool update) override;

		EXMETHOD HRESULT EXOBJCALL GetTheme(IExTheme** r_theme) override;
		EXMETHOD HRESULT EXOBJCALL SetTheme(IExTheme* theme) override;

		EXMETHOD HRESULT EXOBJCALL FindChild(IExElement** r_element, IExElement* ele_after = nullptr,
			EXATOM atom_class = EXATOM_UNKNOWN, LPCWSTR text = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL FindChildEx(IExElement** r_element, IExElement* ele_after = nullptr,
			ExElementMatchProc match_proc = nullptr, LPARAM lparam = 0) override;
		EXMETHOD HRESULT EXOBJCALL EnumChildren(ExElementEnumProc enum_proc, LPARAM lparam = 0) override;

		EXMETHOD HRESULT EXOBJCALL GetParent(IExElement** r_parent) override;
		EXMETHOD HRESULT EXOBJCALL GetParentEx(IExElement** r_parent, IExElement** r_owner) override;
		EXMETHOD HRESULT EXOBJCALL GetRelationElement(ExElementRelation relation, IExElement** r_element) override;
		EXMETHOD HRESULT EXOBJCALL GetElementByID(EXATOM id, IExElement** r_element) override;
		EXMETHOD HRESULT EXOBJCALL GetElementByNodeID(EXATOM node_id, IExElement** r_element) override;
		EXMETHOD HRESULT EXOBJCALL GetElementByName(LPCWSTR name, IExElement** r_element) override;
		EXMETHOD HRESULT EXOBJCALL GetElementFromPoint(long x, long y, DWORD flags, IExElement** r_element) override;

		EXMETHOD HRESULT EXOBJCALL Move(long left, long top, long width, long height, bool redraw = true) override;
		EXMETHOD HRESULT EXOBJCALL SetPos(IExElement* ele_after, long left, long top, long width, long height, DWORD flags) override;

		EXMETHOD HRESULT EXOBJCALL GetRect(ExRect* r_rect) override;
		EXMETHOD HRESULT EXOBJCALL GetRectEx(ExRect* r_rect, int from) override;
		EXMETHOD HRESULT EXOBJCALL GetClientRect(ExRect* r_client_rect) override;

		EXMETHOD HRESULT EXOBJCALL Show(DWORD mode) override;
		EXMETHOD HRESULT EXOBJCALL ShowEx(DWORD mode) override;

		EXMETHOD HRESULT EXOBJCALL SetTrayIcon(HICON icon, LPCWSTR tips) override;
		EXMETHOD HRESULT EXOBJCALL PopupTrayIcon(LPCWSTR text, LPCWSTR title, DWORD flags) override;



		protected:
			virtual bool EXOBJCALL OnWindowReady(uint32_t message, WPARAM wparam, LPARAM lparam, LRESULT& r_result);
			virtual bool EXOBJCALL OnWindowDestroy(uint32_t message, WPARAM wparam, LPARAM lparam, LRESULT& r_result);


	protected:
		HWND m_handle{};
		void* m_thunk_data{};
		WNDPROC m_old_proc{};
		ExWindowFlags m_flags{};


	};

}
