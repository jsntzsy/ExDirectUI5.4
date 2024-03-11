/**
 * @file dllmain.cpp
 * @brief 入口头文件
 * @author EternalZSY
 * @version 5.3.0.0
 * @date 2024-03-10
 * @copyright
 */

#include "stdafx.h"
#include "common/interfaces/object_impl.hpp"

BOOL APIENTRY DllMain(HMODULE module_handle, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

namespace ExDirectUI
{
	EXINTERFACE("C22E5E7A-8C28-4D49-8AC0-F81AA8DA67D7") IAdd : public IExObject
	{
		EXSTDMETHOD Add(int a) PURE;
	};

	EXINTERFACE("C22E5E7A-8C28-4D49-8AC0-F81AA8DA67D8") ISub : public IExObject
	{
		EXSTDMETHOD Sub(int a) PURE;
	};

	class CMath : public ExObjectBase, public IAdd, public ISub
	{
	public:
		EX_INTERFACE_MAP_BEGIN();
		EX_INTERFACE_UNK(IAdd);
		EX_INTERFACE_TO(IAdd);
		EX_INTERFACE_TO(ISub);
		EX_INTERFACE_MAP_END();

		EX_INTERFACE_MULTI_IMPLIMENT();

	public:
		HRESULT Add(int a) noexcept override
		{
			m_nValue += a;
			return S_OK;
		}

		HRESULT Sub(int a) noexcept override
		{
			m_nValue -= a;
			return S_OK;
		}

	private:
		int m_nValue{ 0 };
	};


	HRESULT EXAPI APIENTRY ExDbgEntry(HINSTANCE instance)
	{
		//无需初始化
		CMath* p = new CMath();
		ISub* sub = nullptr;
		p->QueryInterface(&sub);
		sub->Sub(10);


		return S_OK;
	}
}
