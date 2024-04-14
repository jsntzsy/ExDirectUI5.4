/**
 * @file type_parser.cpp
 * @brief 类型解析实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-12
 * @copyright
 */

#include "stdafx.h"
#include "resource/type_parser.h"
#include "src/resource/type_parser.h"
#include "atom.h"

#include "src/resource/type_parsers/xxx_type_parser.hpp"
#include "src/resource/type_parsers/number_type_parser.hpp"
#include "src/resource/type_parsers/logic_type_parser.hpp"
#include "src/resource/type_parsers/color_type_parser.hpp"
#include "src/resource/type_parsers/coordinate_type_parser.hpp"
#include "src/resource/type_parsers/data_type_parser.hpp"


namespace ExDirectUI
{
	std::unordered_map<EXATOM, IExTypeParser*> g_resource_type_parsers = {

		//数值类
		{ATOM_INT, ExInt32TypeParser::Instance()},
		{ATOM_UINT, ExUInt32TypeParser::Instance()},
		{ATOM_LONG, ExInt64TypeParser::Instance()},
		{ATOM_ULONG, ExUInt64TypeParser::Instance()},
		{ATOM_BYTE, ExByteTypeParser::Instance()},
		{ATOM_FLOAT, ExFloatTypeParser::Instance()},
		{ATOM_DOUBLE, ExDoubleTypeParser::Instance()},
		{ATOM_NUMBER, ExFloatTypeParser::Instance()},
		{ATOM_NUMBERU, ExNumberUTypeParser::Instance()},
		
		//逻辑类
		{ATOM_BOOL, ExBoolTypeParser::Instance()},
		
		//颜色类
		{ATOM_COLOR, ExColorTypeParser::Instance()},
		{ATOM_CHANNEL, ExChannelTypeParser::Instance()},
		{ATOM_ALPHA, ExOpacityTypeParser::Instance()},
		{ATOM_OPACITY, ExOpacityTypeParser::Instance()},
		
		//坐标系类
		{ATOM_POINT, ExPointSizeTypeParser::Instance()},
		{ATOM_SIZE, ExPointSizeTypeParser::Instance()},
		{ATOM_RECT, ExRectTypeParser::Instance()},
		{ATOM_POINTF, ExPointSizeFTypeParser::Instance()},
		{ATOM_SIZEF, ExPointSizeFTypeParser::Instance()},
		{ATOM_RECTF, ExRectFTypeParser::Instance()},
		{ATOM_POINTU, ExPointSizeUTypeParser::Instance()},
		{ATOM_SIZEU, ExPointSizeUTypeParser::Instance()},
		{ATOM_RECTU, ExRectUTypeParser::Instance()},
		
		//文本和数据类
		{ATOM_DATA, ExDataTypeParser::Instance()},
		{ATOM_CHAR, ExCharTypeParser::Instance()},
		

	};

	////////////////

	HRESULT EXAPI EXCALL ExParseFromXmlNode(const pugi::xml_node* node, LPARAM lparam, ExVariant* r_value)
	{
		CHECK_PARAM(node);
		CHECK_PARAM(r_value);

		//拿到节点名称(类型)
		LPCWSTR type_str = node->name();
		handle_if_false(type_str && *type_str, E_INVALIDARG, L"获取节点类型失败");

		//计算原子号,并查表
		EXATOM type = ExAtom(type_str);
		auto it = g_resource_type_parsers.find(type);
		handle_if_false(it != g_resource_type_parsers.end(), E_NOTIMPL, L"不支持的类型解析");

		//先默认它是空的
		V_VT(r_value) = VT_EMPTY;

		//解析
		HRESULT hr = it->second->ParseFromXmlNode(type, node, lparam, r_value);

		//如果解析失败,但是初始化了值,则清除掉
		if (FAILED(hr) && V_VT(r_value) != VT_EMPTY) {
			ExVariantClear(r_value);
		}

		return hr;
	}

	HRESULT EXAPI EXCALL ExParseFromString(EXATOM type, LPCWSTR str, LPARAM lparam, ExVariant* r_value)
	{
		CHECK_PARAM(type != EXATOM_UNKNOWN);
		CHECK_PARAM(str);
		CHECK_PARAM(r_value);

		//查表
		auto it = g_resource_type_parsers.find(type);
		handle_if_false(it != g_resource_type_parsers.end(), E_NOTIMPL, L"不支持的类型解析");

		//先默认它是空的
		V_VT(r_value) = VT_EMPTY;

		//解析
		HRESULT hr = it->second->ParseFromString(type, str, lparam, r_value);

		//如果解析失败,但是初始化了值,则清除掉
		if (FAILED(hr) && V_VT(r_value) != VT_EMPTY) {
			ExVariantClear(r_value);
		}

		return hr;
	}

}


