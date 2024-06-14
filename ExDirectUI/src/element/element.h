/**
 * @file element.h
 * @brief UI元素基类内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once
#include "element/element.h"
#include "element/event.hpp"

namespace ExDirectUI
{
	struct ExElementProperties
	{
		int type;	// -1为map,0为无,1为array

		union
		{
			std::vector<LONG_PTR>* arr;
			std::unordered_map<EXATOM, LONG_PTR>* map;
			void* values;
		};

		ExElementProperties(int count = 0)
		{
			SetCount(count);
		}

		void SetCount(int count) MAYTHROW
		{
			Reset();

			if (type > 0) { arr = new std::vector<LONG_PTR>(type); }
			else if (type < 0) { map = new std::unordered_map<EXATOM, LONG_PTR>(); }
			else { values = nullptr; }
		}

		void Reset()
		{
			if (values == nullptr) { return; }
			if (type == 1) { delete arr; }
			else if (type == -1) { delete map; }
			values = nullptr;
		}
	};



	template<class T>
	class ExElementBase : public T
	{
	protected:
		virtual LRESULT EXOBJCALL OnMessage(uint32_t message, WPARAM wparam, LPARAM lparam, bool& r_handled)
		{
			r_handled = false;
			return this->DefMessage(message, wparam, lparam);
		}

	public:
		EXMETHOD LRESULT EXOBJCALL DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam) override
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
				if (m_properties.type < 0) {
					auto it = m_properties.map->find(wparam);
					if (it == m_properties.map->end()) { return S_FALSE; }
					m_properties.map->erase(it);
				}
				else if (m_properties.type > 0) {
					int index = wparam;
					if (m_properties.type <= index) { return S_FALSE; }
					(*m_properties.arr)[index] = 0;
				}
				else { return E_NOTIMPL; }
				return S_OK;
			}


			}
			return 0;
		}
		EXMETHOD HRESULT EXOBJCALL BroadcastMessageToChildren(uint32_t message, WPARAM wparam,
			LPARAM lparam, bool async, ExElementBroadcastFilterProc filter_proc, LPARAM lparam_filter) override
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

		EXMETHOD HRESULT EXOBJCALL AddEventHandler(uint32_t event,
			ExElementEventHandlerProc event_proc, DWORD flags = 0) override
		{
			CHECK_PARAM(event != 0);
			CHECK_PARAM(event_proc);

			handle_if_false(m_events[event].AddHandler(event_proc), EE_EXISTS, L"事件处理函数已存在");
			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL RemoveEventHandler(uint32_t event,
			ExElementEventHandlerProc event_proc) override
		{
			CHECK_PARAM(event != 0);
			CHECK_PARAM(event_proc);

			handle_if_false(m_events[event].RemoveHandler(event_proc), EE_NOEXISTS, L"事件处理函数不存在");
			return S_OK;
		}

		EXMETHOD HRESULT EXOBJCALL NotifyEvent(uint32_t event, WPARAM wparam, LPARAM lparam, LRESULT* r_result) override
		{
			CHECK_PARAM(event != 0);

			auto it = m_events.find(event);
			if (it == m_events.end()) { return S_FALSE; }

			LRESULT result = it->second.Invoke(this, m_id, event, wparam, lparam);
			if (r_result) { *r_result = result; }

			return S_OK;
		}

		inline LONG_PTR* EXOBJCALL GetPropertyPtr(EXATOM id)
		{
			if (m_properties.type < 0) {
				return &(*m_properties.map)[id];
			}
			else if (m_properties.type > 0) {
				int index = id;
				if (m_properties.arr->size() <= index) { return nullptr; }
				return &(*m_properties.arr)[index];
			}
			return nullptr;
		}

		EXMETHOD HRESULT EXOBJCALL InitPropertyList(int property_count) override
		{
			m_properties.SetCount(property_count);
			return S_OK;
		}
		EXMETHOD uint32_t EXOBJCALL GetPropertyCount() override
		{
			if (m_properties.type < 0) {
				return (uint32_t)m_properties.map->size();
			}
			else if (m_properties.type > 0) {
				return (uint32_t)m_properties.arr->size();
			}
			return 0;
		}
		EXMETHOD HRESULT EXOBJCALL GetProperty(EXATOM id, LONG_PTR* r_value) override
		{
			CHECK_PARAM(r_value);
			return (HRESULT)DispatchMessage(EWM_GETPROP, id, (LPARAM)r_value);
		}
		EXMETHOD HRESULT EXOBJCALL SetProperty(EXATOM id, LONG_PTR value) override
		{
			auto old_value = GetPropertyPtr(id);
			handle_if_false(old_value, E_FAIL, L"获取原始值失败");

			LONG_PTR values[2] = { *old_value, value };
			return (HRESULT)DispatchMessage(EWM_SETPROP, id, (LPARAM)values);
		}

		EXMETHOD HRESULT EXOBJCALL RemoveProperty(EXATOM id) override
		{
			auto value = GetPropertyPtr(id);
			handle_if_false(value, E_FAIL, L"获取当前值失败");
			return (HRESULT)DispatchMessage(EWM_REMOVEPROP, id, (LPARAM)*value);
		}
		EXMETHOD HRESULT EXOBJCALL EnumProperty(ExElementPropertyEnumProc enum_proc, LPARAM lparam) override
		{
			CHECK_PARAM(enum_proc);

			if (m_properties.type < 0) {
				for (auto& it : *m_properties.map) {
					if (enum_proc(this, it.first, it.second, lparam)) { return S_FALSE; }
				}
			}
			else if (m_properties.type > 0) {
				auto &arr = *m_properties.arr;
				for (uint32_t i = 0; i < (size_t)arr.size(); i++) {
					if (enum_proc(this, i, arr[i], lparam)) { return S_FALSE; }
				}
			}
			else { return E_NOTIMPL; }

			return S_OK;
		}

	protected:
		int m_id{};

		std::unordered_map<uint32_t, ExEventDelegate> m_events;
		ExElementProperties m_properties{};

		ExElementBase* m_ele_parent = nullptr;
		ExElementBase* m_ele_child_first = nullptr;
		ExElementBase* m_ele_child_last = nullptr;
		ExElementBase* m_ele_prev = nullptr;
		ExElementBase* m_ele_next = nullptr;

	};


#define EX_MESSAGE_MAP_BEGIN()																					\
		LRESULT _RESULT_ = 0;																					\
		switch(message) {																							


#define EX_MESSAGE_MAP_END()																					\
		}																										\
		if(r_handled == false) { _RESULT_ = __super::OnMessage(message, wparam, lparam, r_handled); }			\
		return _RESULT_;


#define EX_MESSAGE_MAP_ON(msg, todo)		case msg: { todo; r_handled = true; }break;
#define EX_MESSAGE_MAP_INVOKE(msg, fn)		case msg: { r_handled = fn(message,wparam,lparam, _RESULT_); }break;
#define EX_MESSAGE_MAP_ARGS(msg, fn, ...)	case msg: { _RESULT_ = fn(__VA_ARGS__); r_handled = true; }break;


}
