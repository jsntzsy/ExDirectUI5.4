/**
 * @file variant.h
 * @brief 扩展变体类型头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-10
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
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
		EVT_POINTU,
		EVT_RECTU,

	};

	union ExVariantExtendValues
	{
		ExNumU numu_;
		ExPointF point_;
		ExRectF rect_;
		ExPointU pointu_;
		ExRectU rectu_;
	};

	HRESULT EXAPI EXCALL ExVariantInit(VARIANT* variant, VARTYPE vt = VT_EMPTY);
	HRESULT EXAPI EXCALL ExVariantClear(VARIANT* variant);
	HRESULT EXAPI EXCALL ExVariantCopy(VARIANT* dest_variant, const VARIANT* src_variant);
	HRESULT EXAPI EXCALL ExVariantToString(VARIANT* variant, VARTYPE vt, BSTR* r_str);
	HRESULT EXAPI EXCALL ExVariantGetValuePtr(VARIANT* variant, void** r_value);

}

#define IS_EVT(VT)		((VT & 0x0800) == EVT_BEGIN)
#define V_ISEVT(X)		(IS_EVT(V_VT(X)))

#define IS_EVTR(VT)		((VT & 0xF800) == EVT_EXTEND_MARK)
#define V_ISEVTR(X)		(IS_EVTR(V_VT(X)))

#define V_EX(X)			((ExDirectUI::ExVariantExtendValues*)V_BYREF(X))
#define V_EXF(X,F)		(((ExDirectUI::ExVariantExtendValues*)V_BYREF(X))->F)

#define V_EXOBJ(X)		((ExDirectUI::IExObject*)V_UNION(X, punkVal))


