/**
 * @file type_parser.h
 * @brief 类型解析头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */
#pragma once

namespace pugi
{
	class xml_node;
}

namespace ExDirectUI
{

	
	////////////////////////////
	
	interface IExTypeParser
	{
		EXMETHOD HRESULT EXOBJCALL ParseFromXmlNode(EXATOM type, const pugi::xml_node* node, LPARAM lparam, ExVariant* r_value) PURE;
		EXMETHOD HRESULT EXOBJCALL ParseFromString(EXATOM type, LPCWSTR str, LPARAM lparam, ExVariant* r_value) PURE;
	};

	///////////////////////

	HRESULT EXAPI EXCALL ExParseFromXmlNode(const pugi::xml_node* node, LPARAM lparam, ExVariant* r_value);
	HRESULT EXAPI EXCALL ExParseFromString(EXATOM type, LPCWSTR str, LPARAM lparam, ExVariant* r_value);

}
