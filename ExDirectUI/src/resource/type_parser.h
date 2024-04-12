/**
 * @file type_parser.h
 * @brief 类型解析内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once
#include "common/singleton.hpp"

#ifdef new
#undef new
#endif
#include "../ThirdParts/pugixml/pugixml.hpp"

namespace ExDirectUI
{
	class ExStandardTypeParser : public IExTypeParser
	{
	public:

		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node, LPARAM lparam, ExVariant* r_value) override
		{
			LPCWSTR value = L"";
			pugi::xml_attribute attr = node->attribute(L"value");
			if (attr) { value = attr.value(); }
			else { value = node->text().get(); }
			return this->ParseFromString(type, value, lparam, r_value);
		}

	};

	////////////////////

#define EX_SAMPLE_TYPE_PARSER(name, vt, todo) 													\
	class Ex##name##TypeParser : public ExStandardTypeParser,									\
		public ExSingleton<Ex##name##TypeParser> {												\
	public:																						\
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,					\
			LPARAM lparam, ExVariant* V) override {												\
			return_if_failed(ExVariantInit(V,vt));												\
			todo;																				\
			return S_OK;																		\
		}																						\
	};																							


#define EX_CONSTS_TYPE_PARSER(name, multiple, count, todo) 										\
	class Ex##name##TypeParser : public ExStandardTypeParser,									\
		public ExSingleton<Ex##name##TypeParser> {												\
		static const DWORD _KV_[count][2];														\
	public:																						\
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str,					\
			LPARAM lparam, ExVariant* V) override {												\
			return_if_failed(ExVariantInit(V,EVT_FLAGS), ExVariantClear(V));					\
			todo;																				\
			if (multiple) { return ExParseToConsts(str, _KV_, count, (DWORD*)&V_UI4(V)); }		\
			else { return ExParseToConst(str, _KV_, count, (DWORD*)&V_UI4(V)); }				\
			return S_OK; 																		\
		}																						\
	};																							\
	const DWORD Ex##name##TypeParser::_KV_[count][2] =															




	//////////////////////


}
