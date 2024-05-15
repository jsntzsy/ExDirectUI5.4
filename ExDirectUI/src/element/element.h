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
		virtual LRESULT EXOBJCALL OnMessage(uint32_t message, WPARAM wparam, LPARAM lparam);

	public:
		EXMETHOD LRESULT EXOBJCALL DefMessage(uint32_t message, WPARAM wparam, LPARAM lparam) override;
		EXMETHOD HRESULT EXOBJCALL BroadcastMessageToChildren(uint32_t message, WPARAM wparam,
			LPARAM lparam, bool async, ExElementBroadcastFilterProc filter_proc, LPARAM lparam_filter) override;

		EXMETHOD HRESULT EXOBJCALL AddEventHandler(uint32_t event,
			ExElementEventHandlerProc event_proc, DWORD flags = 0) override;
		EXMETHOD HRESULT EXOBJCALL RemoveEventHandler(uint32_t event,
			ExElementEventHandlerProc event_proc) override;
		EXMETHOD HRESULT EXOBJCALL NotifyEvent(uint32_t event, WPARAM wparam, LPARAM lparam, LRESULT* r_result) override;

		inline LONG_PTR* EXOBJCALL GetPropertyPtr(EXATOM id);
		EXMETHOD HRESULT EXOBJCALL InitPropertyList(int property_count) override;
		EXMETHOD uint32_t EXOBJCALL GetPropertyCount() override;
		EXMETHOD HRESULT EXOBJCALL GetProperty(EXATOM id, LONG_PTR* r_value) override;
		EXMETHOD HRESULT EXOBJCALL SetProperty(EXATOM id, LONG_PTR value) override;
		EXMETHOD HRESULT EXOBJCALL RemoveProperty(EXATOM id) override;
		EXMETHOD HRESULT EXOBJCALL EnumProperty(ExElementPropertyEnumProc enum_proc, LPARAM lparam) override;

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

}
