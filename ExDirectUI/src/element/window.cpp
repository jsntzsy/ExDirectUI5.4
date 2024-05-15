/**
 * @file window.cpp
 * @brief 皮肤窗口实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-05-04
 * @copyright
 */

#include "stdafx.h"
#include "element/window.h"
#include "src/element/window.h"
#include "src/element/thunk_window.h"

namespace ExDirectUI
{
	LRESULT ExWindow::OnMessageDispatch(void* ptr, uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		return static_cast<ExWindow*>(ptr)->OnMessage(message, wparam, lparam);
	}

	LRESULT EXOBJCALL ExWindow::OnMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{



		
		return 0;
	}

	LRESULT EXOBJCALL ExWindow::DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		return 0;
	}

	///////////

	ExWindow::ExWindow(HWND window, DWORD style)
	{
	}

	ExWindow::~ExWindow()
	{
	}

	LRESULT ExWindow::SendMessage(uint32_t message, WPARAM wparam, LPARAM lparam) 
	{
		return ::SendMessageW(m_handle, message, wparam, lparam);
	}

	LRESULT EXOBJCALL ExWindow::PostMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		return ::PostMessageW(m_handle, message, wparam, lparam);
	}

	LRESULT EXOBJCALL ExWindow::DispatchMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		return this->OnMessage(message, wparam, lparam);
	}

	bool EXOBJCALL ExWindow::IsVisible()
	{
		return ::IsWindowVisible(m_handle);
	}

	HRESULT EXOBJCALL ExWindow::SetVisible(bool visible)
	{
		return Show(visible ? SW_SHOW : SW_HIDE);
	}

	bool EXOBJCALL ExWindow::IsEnable()
	{
		return ::IsWindowEnabled(m_handle);
	}

	HRESULT EXOBJCALL ExWindow::SetEnable(bool enable)
	{
		return ::EnableWindow(m_handle, enable) ? S_OK : S_FALSE;
	}

	bool EXOBJCALL ExWindow::IsFocus()
	{
		return ::GetActiveWindow() == m_handle;
	}

	HRESULT EXOBJCALL ExWindow::SetFocus()
	{
		SetActiveWindow(m_handle);
		return S_OK;
	}

	HRESULT EXOBJCALL ExWindow::KillFocus()
	{
		SetActiveWindow(NULL);
		return S_OK;
	}

	bool EXOBJCALL ExWindow::IsRedrawable()
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetRedrawable(bool redrawable)
	{
		//return SendMessage(WM_SETREDRAW, (WPARAM)redrawable, 0);
	}

	HRESULT EXOBJCALL ExWindow::Invalidate(RECT* invalidate_rect)
	{
		return InvalidateRect(m_handle, invalidate_rect, false) ? S_OK : S_FALSE;
	}

	HRESULT EXOBJCALL ExWindow::Update()
	{
		return UpdateWindow(m_handle) ? S_OK : S_FALSE;
	}

	HRESULT EXOBJCALL ExWindow::GetAttribute(int index, LONG_PTR* r_value)
	{

	}

	HRESULT EXOBJCALL ExWindow::SetAttribute(int index, LONG_PTR r_value, LONG_PTR* r_old_value)
	{
		
	}

	uint32_t EXOBJCALL ExWindow::GetTextLength()
	{
		return ::GetWindowTextLengthW(m_handle);
	}

	HRESULT EXOBJCALL ExWindow::GetText(wchar_t* r_text, uint32_t text_length)
	{
		CHECK_PARAM(r_text);
		return ::GetWindowTextW(m_handle, r_text, text_length);
	}

	HRESULT EXOBJCALL ExWindow::GetTextBSTR(BSTR* r_text)
	{
		CHECK_PARAM(r_text);

		int len = ::GetWindowTextLengthW(m_handle);
		BSTR text = SysAllocStringLen(nullptr, len);
		handle_if_false(text, E_OUTOFMEMORY, L"分配字符串失败");
		
		::GetWindowTextW(m_handle, text, len);
		*r_text = text;

		return S_OK;
	}

	HRESULT EXOBJCALL ExWindow::SetText(LPCWSTR text, bool redraw)
	{
		return SetWindowTextW(m_handle, text) ? S_OK : S_FALSE;
	}

	HRESULT EXOBJCALL ExWindow::GetFont(IExFont** r_font)
	{

	}

	HRESULT EXOBJCALL ExWindow::SetFont(IExFont* font, bool redraw)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetFontFromInfo(ExFontInfo font_info)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetColor(int index, EXARGB* r_color)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetColor(int index, EXARGB color, bool redraw)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetBackgroundImage(ExDisplayImageInfo* r_image)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetBackgroundImage(ExDisplayImageInfo* image, bool redraw)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetBackgroundImageState(bool play, int frame, bool update)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetTheme(IExTheme** r_theme)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetTheme(IExTheme* theme)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::FindChild(IExElement** r_element, IExElement* ele_after, EXATOM atom_class, LPCWSTR text)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::FindChildEx(IExElement** r_element, IExElement* ele_after, ExElementMatchProc match_proc, LPARAM lparam)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::EnumChildren(ExElementEnumProc enum_proc, LPARAM lparam)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetParent(IExElement** r_parent)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetParentEx(IExElement** r_parent, IExElement** r_owner)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetRelationElement(ExElementRelation relation, IExElement** r_element)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetElementByID(EXATOM id, IExElement** r_element)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetElementByNodeID(EXATOM node_id, IExElement** r_element)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetElementByName(LPCWSTR name, IExElement** r_element)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetElementFromPoint(long x, long y, DWORD flags, IExElement** r_element)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::Move(long left, long top, long width, long height, bool redraw)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetPos(IExElement* ele_after, long left, long top, long width, long height, DWORD flags)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetRect(ExRect* r_rect)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetRectEx(ExRect* r_rect, int from)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::GetClientRect(ExRect* r_client_rect)
	{
		
	}

	HRESULT ExWindow::Show(DWORD mode) noexcept
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::ShowEx(DWORD mode)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::SetTrayIcon(HICON icon, LPCWSTR tips)
	{
		
	}

	HRESULT EXOBJCALL ExWindow::PopupTrayIcon(LPCWSTR text, LPCWSTR title, DWORD flags)
	{
		
	}

}


