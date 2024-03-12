/**
 * @file unknown_impl.hpp
 * @brief IUnknown接口包装定义实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 */

#pragma once
#include "ExDirectUI.h"

namespace ExDirectUI
{
	template<typename T>
	class ExUnknownImpl : public T
	{
	public:
		ExUnknownImpl() :m_ref(1) {}
		virtual ~ExUnknownImpl() {}

		STDMETHOD_(ULONG, AddRef)(void) override
		{
			return InterlockedIncrement(&m_ref);
		}
		STDMETHOD_(ULONG, Release)(void) override
		{
			long ref = InterlockedDecrement(&m_ref);
			if (ref == 0) { delete this; }
			return ref;
		}

	private:
		long m_ref;

	};

	template<typename T, typename T_FOR_DESTROY>
	class ExUnknownImplEx : public ExUnknownImpl<T>
	{
	public:
		STDMETHOD_(ULONG, Release)(void) override
		{
			long ref = InterlockedDecrement(&__super::m_ref);
			if (ref == 0) { delete static_cast<T>(this); }
			return ref;
		}
	};

#define EX_DECLEAR_INTERFACE_BEGIN() STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override { 

#define EX_DECLEAR_INTERFACE(INTERFACE)					\
		if (IsEqualIID(riid, __uuidof(INTERFACE))) {	\
			*ppvObject = this;							\
			this->AddRef();								\
			return S_OK; }		

#define EX_DECLEAR_INTERFACE_2(_GUID_)					\
		if (IsEqualIID(riid, _GUID_)) {					\
			*ppvObject = this;							\
			this->AddRef();								\
			return S_OK; }							

#define EX_DECLEAR_INTERFACE_EX(_GUID_,PTR)				\
		if (IsEqualIID(riid, _GUID_)) {					\
			*ppvObject = PTR;							\
			this->AddRef();								\
			return S_OK; }							

#define EX_DECLEAR_INTERFACE_END()  return E_NOINTERFACE; }								\
		template<class T> inline 														\
		HRESULT EXOBJCALL QueryInterface(T** ppvObject) {								\
			return this->QueryInterface(__uuidof(T),(void**)ppvObject);					\
		}																				

}

