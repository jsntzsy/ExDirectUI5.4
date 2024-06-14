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
#include <common/string.hpp>

namespace ExDirectUI
{
	LRESULT ExWindow::OnMessageDispatch(void* ptr, uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		bool handled = false;
		return static_cast<ExWindow*>(ptr)->OnMessage(message, wparam, lparam, handled);
	}

	LRESULT EXOBJCALL ExWindow::OnMessage(uint32_t message, WPARAM wparam, LPARAM lparam, bool& r_handled)
	{
		EX_MESSAGE_MAP_BEGIN();
		EX_MESSAGE_MAP_INVOKE(EWM_READY, OnWindowReady);
		EX_MESSAGE_MAP_INVOKE(WM_DESTROY, OnWindowDestroy);



		default:  ExDbgOutput(ExString::format(L"Message: 0x%04X  wParam: 0x%08X  lParam: 0x%08X \n", message, wparam, lparam).c_str());
		EX_MESSAGE_MAP_END();
	}

	LRESULT EXOBJCALL ExWindow::DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		return CallWindowProcW(m_old_proc, m_handle, message, wparam, lparam);
	}

	///////////

	ExWindow::ExWindow(HWND window, DWORD style)
	{
		m_thunk_data = ExThunkWindowAlloc(ExWindow::OnMessageDispatch, this);
		throw_if_false(m_thunk_data, E_OUTOFMEMORY, L"窗口回调转换代码申请失败");
		m_old_proc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)m_thunk_data);
		throw_if_false(m_old_proc, E_FAIL, L"窗口回调函数设置失败");


		m_flags.validate = true;
		m_handle = window;
		SendMessage(EWM_READY, 0, 0);
	}

	ExWindow::~ExWindow()
	{
		if (m_flags.validate) {
			DispatchMessage(WM_DESTROY, 0, 0);
		}
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
		bool handled = false;
		return this->OnMessage(message, wparam, lparam, handled);
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
		return false;
	}

	HRESULT EXOBJCALL ExWindow::SetRedrawable(bool redrawable)
	{
		//return SendMessage(WM_SETREDRAW, (WPARAM)redrawable, 0);
		return E_NOTIMPL;
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
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetAttribute(int index, LONG_PTR r_value, LONG_PTR* r_old_value)
	{
		return E_NOTIMPL;
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
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetFont(IExFont* font, bool redraw)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetFontFromInfo(ExFontInfo font_info)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetColor(int index, EXARGB* r_color)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetColor(int index, EXARGB color, bool redraw)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetBackgroundImage(ExDisplayImageInfo* r_image)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetBackgroundImage(ExDisplayImageInfo* image, bool redraw)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetBackgroundImageState(bool play, int frame, bool update)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetTheme(IExTheme** r_theme)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetTheme(IExTheme* theme)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::FindChild(IExElement** r_element, IExElement* ele_after, EXATOM atom_class, LPCWSTR text)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::FindChildEx(IExElement** r_element, IExElement* ele_after, ExElementMatchProc match_proc, LPARAM lparam)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::EnumChildren(ExElementEnumProc enum_proc, LPARAM lparam)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetParent(IExElement** r_parent)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetParentEx(IExElement** r_parent, IExElement** r_owner)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetRelationElement(ExElementRelation relation, IExElement** r_element)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetElementByID(EXATOM id, IExElement** r_element)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetElementByNodeID(EXATOM node_id, IExElement** r_element)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetElementByName(LPCWSTR name, IExElement** r_element)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetElementFromPoint(long x, long y, DWORD flags, IExElement** r_element)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::Move(long left, long top, long width, long height, bool redraw)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetPos(IExElement* ele_after, long left, long top, long width, long height, DWORD flags)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetRect(ExRect* r_rect)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetRectEx(ExRect* r_rect, int from)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::GetClientRect(ExRect* r_client_rect)
	{
		return E_NOTIMPL;
	}

	HRESULT ExWindow::Show(DWORD mode) noexcept
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::ShowEx(DWORD mode)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::SetTrayIcon(HICON icon, LPCWSTR tips)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExWindow::PopupTrayIcon(LPCWSTR text, LPCWSTR title, DWORD flags)
	{
		return E_NOTIMPL;
	}

	//////////////////////////

	bool EXOBJCALL ExWindow::OnWindowReady(uint32_t message, WPARAM wparam, LPARAM lparam, LRESULT& r_result)
	{
		ExDbgOutput(L"OnWindowReady");
		return false;
	}
	bool EXOBJCALL ExWindow::OnWindowDestroy(uint32_t message, WPARAM wparam, LPARAM lparam, LRESULT& r_result)
	{
		if (m_flags.validate) {
			if (m_old_proc) { SetWindowLongPtr(m_handle, GWLP_WNDPROC, (LONG_PTR)m_old_proc); }
			if (m_thunk_data) { ExThunkWindowFree(m_thunk_data); }
			m_flags.validate = false;
		}
		return false;
	}

}


