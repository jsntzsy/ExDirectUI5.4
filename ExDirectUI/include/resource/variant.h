/**
 * @file variant.h
 * @brief 扩展变体类型头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-10
 * @copyright
 */
#pragma once
#include "resource/theme.h"

namespace ExDirectUI
{
	/// 变体类型
	typedef VARIANT ExVariant;

	/// 变体扩展类型
	enum ExVariantExtendType : VARTYPE
	{
		EVT_BEGIN = 0x0800,

		EVT_COLOR = 0x0800,
		EVT_FLAGS,
		EVT_OBJECT,

		EVT_EXTEND_MARK = 0x4800,	// byref | begin_mark

		EVT_NUMU,
		EVT_POINT,
		EVT_RECT,
		EVT_POINTF,
		EVT_RECTF,
		EVT_POINTU,
		EVT_RECTU,

		EVT_DATA,

		EVT_ELE_OPACITY,
	};

	union ExVariantExtendValues
	{
		ExNumU numu_;
		ExPoint point_;
		ExRect rect_;
		ExPointF pointf_;
		ExRectF rectf_;
		ExPointU pointu_;
		ExRectU rectu_;
		
		ExData data_;
		ExEleOpacityInfo ele_opacity_;
	};

	HRESULT EXAPI EXCALL ExVariantInit(ExVariant* variant, VARTYPE vt = VT_EMPTY);
	HRESULT EXAPI EXCALL ExVariantClear(ExVariant* variant);
	HRESULT EXAPI EXCALL ExVariantCopy(ExVariant* dest_variant, const ExVariant* src_variant);
	HRESULT EXAPI EXCALL ExVariantToString(ExVariant* variant, VARTYPE vt, BSTR* r_str);
	HRESULT EXAPI EXCALL ExVariantGetValuePtr(ExVariant* variant, void** r_value);

}

#define IS_EVT(VT)		((VT & 0x0800) == EVT_BEGIN)
#define V_ISEVT(X)		(IS_EVT(V_VT(X)))

#define IS_EVTR(VT)		((VT & 0xF800) == EVT_EXTEND_MARK)
#define V_ISEVTR(X)		(IS_EVTR(V_VT(X)))

#define V_EX(X)			((::ExDirectUI::ExVariantExtendValues*)V_BYREF(X))
#define V_EXF(X,F)		(((::ExDirectUI::ExVariantExtendValues*)V_BYREF(X))->F)

#define V_EXOBJ(X)		((::ExDirectUI::IExObject*)V_UNION(X, punkVal))


