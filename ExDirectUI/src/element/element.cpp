/**
 * @file element.cpp
 * @brief UI元素基类实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */

#include "stdafx.h"
#include "element/element.h"
#include "src/element/element.h"
#include "element/event.hpp"

namespace ExDirectUI
{
	LRESULT EXOBJCALL ExElementBase::OnMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		return this->DefMessage(message, wparam, lparam);
	}

	LRESULT EXOBJCALL ExElementBase::DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
	{
		switch (message)
		{
		case EWM_GETPROP: {
			auto value_ptr = GetPropertyPtr(wparam);
			handle_if_false(value_ptr, E_FAIL, L"获取属性值指针失败");
			*((LONG_PTR*)lparam) = *value_ptr;
			return S_OK;
		}
		case EWM_SETPROP: {
			auto value_ptr = GetPropertyPtr(wparam);
			auto values = (LONG_PTR*)lparam;
			handle_if_false(value_ptr, E_FAIL, L"获取属性值指针失败");
			*value_ptr = values[1];
			return S_OK;
		}
		case EWM_REMOVEPROP: {
			if (m_properties_count < 0) {
				auto it = m_properties.map.find(wparam);
				if (it == m_properties.map.end()) { return S_FALSE; }
				m_properties.map.erase(it);
			}
			else if (m_properties_count > 0) {
				int index = wparam;
				if (m_properties_count <= index) { return S_FALSE; }
				m_properties.arr[index] = 0;
			}
			else { return E_NOTIMPL; }
			return S_OK;
		}


		}
		return 0;
	}

	HRESULT EXOBJCALL ExElementBase::BroadcastMessageToChildren(uint32_t message, WPARAM wparam,
		LPARAM lparam, bool async, ExElementBroadcastFilterProc filter_proc, LPARAM lparam_filter)
	{
		ExElementBase* element = m_ele_child_first;

		while (element) {

			if (!filter_proc || filter_proc(this, message, element, EXATOM_UNKNOWN, nullptr,
				lparam_filter)) {

				if (async) { element->PostMessage(message, wparam, lparam); }
				else { element->SendMessage(message, wparam, lparam); }
			}

			element = element->m_ele_next;
		}
		return S_OK;
	}

	HRESULT EXOBJCALL ExElementBase::AddEventHandler(uint32_t event, ExElementEventHandlerProc event_proc, DWORD flags)
	{
		CHECK_PARAM(event != 0);
		CHECK_PARAM(event_proc);

		handle_if_false(m_events[event].AddHandler(event_proc), EE_EXISTS, L"事件处理函数已存在");
		return S_OK;
	}

	HRESULT EXOBJCALL ExElementBase::RemoveEventHandler(uint32_t event, ExElementEventHandlerProc event_proc)
	{
		CHECK_PARAM(event != 0);
		CHECK_PARAM(event_proc);

		handle_if_false(m_events[event].RemoveHandler(event_proc), EE_NOEXISTS, L"事件处理函数不存在");
		return S_OK;
	}

	HRESULT EXOBJCALL ExElementBase::NotifyEvent(uint32_t event, WPARAM wparam, LPARAM lparam, LRESULT* r_result)
	{
		CHECK_PARAM(event != 0);

		auto it = m_events.find(event);
		if (it == m_events.end()) { return S_FALSE; }

		LRESULT result = it->second.Invoke(this, m_id, event, wparam, lparam);
		if (r_result) { *r_result = result; }

		return S_OK;
	}

	inline LONG_PTR* EXOBJCALL ExElementBase::GetPropertyPtr(EXATOM id)
	{
		if (m_properties_count < 0) {
			return &m_properties.map[id];
		}
		else if (m_properties_count > 0) {
			int index = id;
			if (m_properties_count <= index) { return nullptr; }
			return &m_properties.arr[index];
		}
		return nullptr;
	}

	HRESULT EXOBJCALL ExElementBase::InitPropertyList(int property_count)
	{
		if (m_properties_count < 0) {
			m_properties.map.clear();
		}
		else if (m_properties_count > 0) {
			m_properties.arr.clear();
		}

		if (property_count > 0) {
			m_properties.arr = std::vector<LONG_PTR>((size_t)property_count);
		}
		else if (property_count < 0) {
			m_properties.map = std::unordered_map<EXATOM, LONG_PTR>();
		}

		m_properties_count = property_count;
		return S_OK;
	}

	uint32_t EXOBJCALL ExElementBase::GetPropertyCount()
	{
		if (m_properties_count < 0) {
			return m_properties.map.size();
		}
		else if (m_properties_count > 0) {
			return m_properties.arr.size();
		}
		return 0;
	}

	HRESULT EXOBJCALL ExElementBase::GetProperty(EXATOM id, LONG_PTR* r_value)
	{
		CHECK_PARAM(r_value);
		return (HRESULT)DispatchMessage(EWM_GETPROP, id, (LPARAM)r_value);
	}

	HRESULT EXOBJCALL ExElementBase::SetProperty(EXATOM id, LONG_PTR value)
	{
		auto old_value = GetPropertyPtr(id);
		handle_if_false(old_value, E_FAIL, L"获取原始值失败");

		LONG_PTR values[2] = { *old_value, value };
		return (HRESULT)DispatchMessage(EWM_SETPROP, id, (LPARAM)values);
	}

	HRESULT EXOBJCALL ExElementBase::RemoveProperty(EXATOM id)
	{
		auto value = GetPropertyPtr(id);
		handle_if_false(value, E_FAIL, L"获取当前值失败");
		return (HRESULT)DispatchMessage(EWM_REMOVEPROP, id, (LPARAM)*value);
	}

	HRESULT EXOBJCALL ExElementBase::EnumProperty(ExElementPropertyEnumProc enum_proc, LPARAM lparam)
	{
		CHECK_PARAM(enum_proc);

		if (m_properties_count < 0) {
			for (auto& it : m_properties.map) {
				if (enum_proc(this, it.first, it.second, lparam)) { return S_FALSE; }
			}
		}
		else if (m_properties_count > 0) {
			for (uint32_t i = 0; i < (size_t)m_properties.arr.size(); i++) {
				if (enum_proc(this, i, m_properties.arr[i], lparam)) { return S_FALSE; }
			}
		}
		else { return E_NOTIMPL; }

		return S_OK;
	}

	HRESULT EXOBJCALL ExElementBase::GetTheme(IExTheme** r_theme)
	{
		CHECK_PARAM(r_theme);
		handle_if_false(m_theme, E_HANDLE, L"主题对象为空");
		return m_theme->QueryInterface(r_theme);
	}

	HRESULT EXOBJCALL ExElementBase::SetTheme(IExTheme* theme)
	{
		CHECK_PARAM(theme);

		if (m_theme != theme) {
			SendMessage(EWM_UPDATE_THEME, 0, (LPARAM)theme);
			m_theme = theme;
		}
		return S_OK;
	}

}


