/**
 * @file variant.h
 * @brief 扩展变体类型头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-10
 * @copyright
 */
#pragma once
#include "resource/extend_type.h"

namespace ExDirectUI
{
	/// 变体类型
	typedef VARIANT ExVariant;

	/// 变体扩展类型
	enum ExVariantExtendType : VARTYPE
	{
		EVT_MARK = 0x0800,

		EVT_CHANNEL,
		EVT_COLOR,
		EVT_FLAGS,
		EVT_EXOBJECT,

		EVT_DATA,
		
		EVT_NUMU,
		EVT_POINT,
		EVT_POINTF,
		EVT_POINTU,
		EVT_RECT,
		EVT_RECTF,
		EVT_RECTU,

		EVT_FONT,
		EVT_GRIDS_IMAGE,
		EVT_DISPLAY_IMAGE,
		EVT_STATE_IMAGE,
		EVT_STATE_COLOR,

		EVT_ELE_STYLE,
		EVT_ELE_OPACITY,
		EVT_ELE_ANIMATION,
		EVT_ELE_SHADOW,

	};

	HRESULT EXAPI EXCALL ExVariantInit(ExVariant* variant, VARTYPE vt = VT_EMPTY);
	HRESULT EXAPI EXCALL ExVariantClear(ExVariant* variant);
	HRESULT EXAPI EXCALL ExVariantCopy(ExVariant* dest_variant, const ExVariant* src_variant);
	HRESULT EXAPI EXCALL ExVariantToString(ExVariant* variant, VARTYPE vt, BSTR* r_str);
	HRESULT EXAPI EXCALL ExVariantGetValuePtr(ExVariant* variant, void** r_value);

}

#define EVT_IS(vt)				((EVT_MARK & vt) == EVT_MARK)
#define V_ISEXT(X)				EVT_IS(V_VT(X))

#define V_PTR(X,type)			((type*)V_BYREF(X))

#define V_CHANNEL(X)			((EXCHANNEL)V_UI1(X))
#define V_COLOR(X)				((EXARGB)V_UI4(X))
#define V_FLAGS(X)				((DWORD)V_UI4(X))
#define V_EXOBJECT(X)			((IExObject*)V_UNKNOWN(X))

#define V_DATA(X)				V_PTR(X,ExData)
#define V_NUMU(X)				((ExNumU*)&V_CY(X))
#define V_POINT(X)				((ExPoint*)&V_CY(X))
#define V_POINTF(X)				((ExPointF*)&V_CY(X))
#define V_POINTU(X)				V_PTR(X,ExPointU)
#define V_RECT(X)				V_PTR(X,ExRect)
#define V_RECTF(X)				V_PTR(X,ExRectF)
#define V_RECTU(X)				V_PTR(X,ExRectU)

#define V_FONT(X)				V_PTR(X,ExFontInfo)
#define V_GRIDS_IMAGE(X)		V_PTR(X,ExGridsImageInfo)
#define V_DISPLAY_IMAGE(X)		V_PTR(X,ExDisplayImageInfo)
#define V_STATE_IMAGE(X)		V_PTR(X,ExStateImageInfo)
#define V_STATE_COLOR(X)		V_PTR(X,ExStateColorInfo)

#define V_ELE_STYLE(X)			V_PTR(X,ExEleStyleInfo)
#define V_ELE_OPACITY(X)		((ExEleOpacityInfo*)&V_CY(X))
#define V_ELE_ANIMATION(X)		V_PTR(X,ExEleAnimationInfo)
#define V_ELE_SHADOW(X)			V_PTR(X,ExEleShadowInfo)





