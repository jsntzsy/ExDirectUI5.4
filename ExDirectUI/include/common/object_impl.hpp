﻿/**
 * @file object_impl.hpp
 * @brief 对象接口实现类头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once
#include "object.i.h"
#include <sstream>

#define EX_BEGIN_INTERFACE_MAP()											\
	EXMETHOD HRESULT EXOBJCALL QueryInterface(REFIID riid, void** ppvObject) override {	

#define EX_END_INTERFACE_MAP()										\
		ppvObject = nullptr;										\
		return E_NOINTERFACE;										\
	}																\
	template<class QI>												\
	inline HRESULT EXOBJCALL QueryInterface(QI** ppv) {				\
		return this->QueryInterface(__uuidof(QI), (void**)ppv);		\
	}

#define EX_INTERFACE_ENTRY(Interface)						\
	if (IsEqualIID(riid, __uuidof(Interface))) {		\
		*ppvObject = static_cast<Interface*>(this);		\
		static_cast<Interface*>(this)->AddRef();		\
		return S_OK;									\
	}

#define EX_INTERFACE_ENTRY2(Interface,InterfaceType)		\
	if (IsEqualIID(riid, __uuidof(Interface))) {			\
		*ppvObject = static_cast<InterfaceType*>(this);		\
		static_cast<InterfaceType*>(this)->AddRef();		\
		return S_OK;										\
	}

#define EX_INTERFACE_MULTI_IMPLIMENT()										\
	EXMETHOD ULONG EXOBJCALL AddRef() override {							\
		return ExObjectBase::AddRef();										\
	}																		\
	EXMETHOD ULONG EXOBJCALL Release() override {							\
		return ExObjectBase::Release();										\
	}
	

namespace ExDirectUI
{
	template<class Interface>
	class ExObjectImpl : public Interface
	{
	public:
		virtual ~ExObjectImpl() {}

	public:

		// IUnknown

		EXMETHOD ULONG EXOBJCALL AddRef() override
		{
			return InterlockedIncrement(&m_object_ref_count);
		}
		EXMETHOD ULONG EXOBJCALL Release() override
		{
			ULONG ref_count = InterlockedDecrement(&m_object_ref_count);
			if (ref_count == 0) {
				FinalRelease();
			}
			return ref_count;
		}

		// IExObject

		EXMETHOD std::wstring EXOBJCALL ToString() const override 
		{
			return L"ExObject()";
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			//FIXME: 这里应当触发一个异常

			return nullptr;
		}



	protected:
		EXMETHOD void EXOBJCALL FinalRelease() { delete this; }
	private:
		ULONG m_object_ref_count{ 1 };
	};

	using ExObjectBase = ExObjectImpl<IExObject>;
}
