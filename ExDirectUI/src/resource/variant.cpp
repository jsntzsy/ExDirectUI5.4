/**
 * @file variant.cpp
 * @brief 扩展变体类型实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-10
 * @copyright
 */

#include "stdafx.h"
#include "resource/variant.h"
#include "resource/string_parser.h"
#include "common/mem_pool.hpp"
#include "common/string.hpp"

namespace ExDirectUI
{
	
	//后期可能改为内存池分配(超过指定尺寸的按普通申请,小于某个尺寸则用内存池)
#define V_NEW(size)			exalloc(size)
#define V_FREE(ptr,size)	exfree(ptr)

	HRESULT EXAPI EXCALL ExVariantInit(ExVariant* variant, VARTYPE vt)
	{
		CHECK_PARAM(variant);

		VariantInit(variant);

		try
		{
			//TODO: 这里的V_NEW如果失败,应当throw

			//MARK:需要在这里判断是否需要分配额外内存
			//如果是扩展格式,要在检查下需不需要分配对应成员值的存储空间
			if (EVT_IS(vt)) {
				switch (vt) {

				case EVT_DATA: V_BYREF(variant) = V_NEW(sizeof(ExData)); break;
				case EVT_POINTU: V_BYREF(variant) = V_NEW(sizeof(ExPointU)); break;
				case EVT_RECT: V_BYREF(variant) = V_NEW(sizeof(ExRect)); break;
				case EVT_RECTF: V_BYREF(variant) = V_NEW(sizeof(ExRectF)); break;
				case EVT_RECTU: V_BYREF(variant) = V_NEW(sizeof(ExRectU)); break;
				case EVT_FONT: V_BYREF(variant) = V_NEW(sizeof(ExFontInfo)); break;
				case EVT_GRIDS_IMAGE: V_BYREF(variant) = V_NEW(sizeof(ExGridsImageInfo)); break;
				case EVT_DISPLAY_IMAGE: V_BYREF(variant) = V_NEW(sizeof(ExDisplayImageInfo)); break;
				case EVT_STATE_IMAGE: V_BYREF(variant) = V_NEW(sizeof(ExStateImageInfo)); break;
				case EVT_STATE_COLOR: V_BYREF(variant) = V_NEW(sizeof(ExStateColorInfo)); break;
				case EVT_ELE_STYLE:  V_BYREF(variant) = V_NEW(sizeof(ExEleStyleInfo)); break;
				case EVT_ELE_ANIMATION:  V_BYREF(variant) = V_NEW(sizeof(ExEleAnimationInfo)); break;
				case EVT_ELE_SHADOW: V_BYREF(variant) = V_NEW(sizeof(ExEleShadowInfo)); break;

				}
			}

			//设置变体类型
			V_VT(variant) = vt;
			return S_OK;
		}
		catch_default({});
	}

	HRESULT EXAPI EXCALL ExVariantClear(ExVariant* variant)
	{
		CHECK_PARAM(variant);

		//MARK:这里判断是否需要释放额外分配的内存
		//如果是扩展格式,则先检查是否要释放对应的成员值存储空间
		//以及处理是否要释放相关结构里的引用和申请的内存
		if (V_ISEXT(variant) && V_BYREF(variant) != nullptr) {

			switch (V_VT(variant))
			{
			case EVT_EXOBJECT: V_EXOBJECT(variant)->Release(); break;

			case EVT_DATA:
				ExDataFree(V_DATA(variant));
				V_FREE(V_DATA(variant), sizeof(ExData));
				break;

			case EVT_POINTU: V_FREE(V_POINTU(variant), sizeof(ExPointU)); break;
			case EVT_RECT: V_FREE(V_RECT(variant), sizeof(ExRect)); break;
			case EVT_RECTF: V_FREE(V_RECTF(variant), sizeof(ExRectF)); break;
			case EVT_RECTU: V_FREE(V_RECTU(variant), sizeof(ExRectU)); break;
			case EVT_FONT: V_FREE(V_FONT(variant), sizeof(ExFontInfo)); break;
			case EVT_GRIDS_IMAGE: V_FREE(V_GRIDS_IMAGE(variant), sizeof(ExGridsImageInfo)); break;
			case EVT_DISPLAY_IMAGE:
				SAFE_RELEASE(V_DISPLAY_IMAGE(variant)->image);
				V_FREE(V_DISPLAY_IMAGE(variant), sizeof(ExDisplayImageInfo));
				break;
			case EVT_STATE_IMAGE:
				SAFE_RELEASE(V_STATE_IMAGE(variant)->image);
				V_FREE(V_STATE_IMAGE(variant), sizeof(ExStateImageInfo));
				break;
			case EVT_STATE_COLOR: V_FREE(V_STATE_COLOR(variant), sizeof(ExStateColorInfo)); break;
			case EVT_ELE_STYLE:  V_FREE(V_ELE_STYLE(variant), sizeof(ExEleStyleInfo)); break;
			case EVT_ELE_ANIMATION:  V_FREE(V_ELE_ANIMATION(variant), sizeof(ExEleAnimationInfo)); break;
			case EVT_ELE_SHADOW: {
				const auto shadow = V_ELE_SHADOW(variant);
				if (shadow->type == ExEleShadowType::Texture) {
					SAFE_RELEASE(shadow->info.texture.image);
				}
				V_FREE(shadow, sizeof(ExEleShadowInfo));
			}break;
			}
			V_BYREF(variant) = nullptr;
		}

		return VariantClear(variant);
	}

	HRESULT EXAPI EXCALL ExVariantCopy(ExVariant* dest_variant, const ExVariant* src_variant)
	{
		CHECK_PARAM(dest_variant);
		CHECK_PARAM(src_variant);

		//如果不是扩展类型,则直接用API复制
		if (!V_ISEXT(src_variant)) {
			handle_if_failed(
				VariantCopy(dest_variant, src_variant),
				L"复制变体类型失败"
			);
			return S_OK;
		}

		//按源变体类型初始化目标变体
		return_if_failed(ExVariantInit(dest_variant, V_VT(src_variant)));

		//MARK:这里判断是否需要额外操作
		//检查是否需要额外操作
		switch (V_VT(src_variant))
		{
			//case EVT_CHANNEL:
			//case EVT_COLOR:
			//case EVT_FLAGS:
		case EVT_EXOBJECT:
			V_BYREF(dest_variant) = V_BYREF(src_variant);
			if (V_EXOBJECT(dest_variant)) { V_EXOBJECT(dest_variant)->AddRef(); }
			break;

		case EVT_DATA: *V_DATA(dest_variant) = *V_DATA(src_variant); break;

			//case EVT_NUMU:
			//case EVT_POINT:
			//case EVT_POINTF: break;
		case EVT_POINTU: *V_POINTU(dest_variant) = *V_POINTU(src_variant); break;

		case EVT_RECT: *V_RECT(dest_variant) = *V_RECT(src_variant); break;
		case EVT_RECTF: *V_RECTF(dest_variant) = *V_RECTF(src_variant); break;
		case EVT_RECTU: *V_RECTU(dest_variant) = *V_RECTU(src_variant); break;

		case EVT_FONT: *V_FONT(dest_variant) = *V_FONT(src_variant); break;
		case EVT_GRIDS_IMAGE: *V_GRIDS_IMAGE(dest_variant) = *V_GRIDS_IMAGE(src_variant); break;
		case EVT_DISPLAY_IMAGE:
			*V_DISPLAY_IMAGE(dest_variant) = *V_DISPLAY_IMAGE(src_variant);
			if (V_DISPLAY_IMAGE(dest_variant)->image) {
				V_DISPLAY_IMAGE(dest_variant)->image->AddRef();
			}
			break;
		case EVT_STATE_IMAGE:
			*V_STATE_IMAGE(dest_variant) = *V_STATE_IMAGE(src_variant);
			if (V_STATE_IMAGE(dest_variant)->image) {
				V_STATE_IMAGE(dest_variant)->image->AddRef();
			}
			break;
		case EVT_STATE_COLOR: *V_STATE_COLOR(dest_variant) = *V_STATE_COLOR(src_variant); break;

		case EVT_ELE_STYLE: *V_ELE_STYLE(dest_variant) = *V_ELE_STYLE(src_variant); break;
		case EVT_ELE_OPACITY: *V_ELE_OPACITY(dest_variant) = *V_ELE_OPACITY(src_variant); break;
		case EVT_ELE_ANIMATION: *V_ELE_ANIMATION(dest_variant) = *V_ELE_ANIMATION(src_variant); break;
		case EVT_ELE_SHADOW:
			*V_ELE_SHADOW(dest_variant) = *V_ELE_SHADOW(src_variant);
			if (V_ELE_SHADOW(dest_variant)->type == ExEleShadowType::Texture) {
				if (V_ELE_SHADOW(dest_variant)->info.texture.image) {
					V_ELE_SHADOW(dest_variant)->info.texture.image->AddRef();
				}
			}
			break;


		default:  //默认情况直接拷贝整个结构体
			*dest_variant = *src_variant;
			break;
		}

		return S_OK;
	}

	HRESULT EXAPI EXCALL ExVariantToString(ExVariant* variant, VARTYPE vt, BSTR* r_str)
	{
		CHECK_PARAM(variant);
		CHECK_PARAM(r_str);

		//默认值处理
		if (vt == VT_EMPTY) { vt = V_VT(variant); }

		//如果不是扩展类型,则按系统默认处理
		if (!EVT_IS(vt)) {
			ExVariant tmp{};
			handle_if_failed(
				VariantChangeType(&tmp, variant, 0, VT_BSTR),
				L"转换变体类型为字符串失败"
			);

			//这里直接返回,所以不需要Clear这个tmp
			*r_str = V_BSTR(&tmp);
			return S_OK;
		}

		//MARK:这里需要根据成员值转换为字符串
		wstring str;
		switch (vt)
		{
		case EVT_CHANNEL:
			str = ExString::format(L"%u", V_CHANNEL(variant));
			break;
		case EVT_COLOR:
			str = ExString::format(L"#08X", V_COLOR(variant));
			break;
		case EVT_FLAGS:
			str = ExString::format(L"0x08X", V_FLAGS(variant));
			break;
		case EVT_EXOBJECT:
			if (V_EXOBJECT(variant)) {
				str = V_EXOBJECT(variant)->ToString();
			}break;

		case EVT_DATA: {
			const auto data = V_DATA(variant);
			str = ExString::format(L"0x%p[%xu]", data->data, data->size);
		}break;

		case EVT_NUMU: {
			const auto nu = V_NUMU(variant);
			str = ExString::format(L"%f%s", nu->value,
				ExNumberUnitToString(V_NUMU(variant)->unit)
			);
		}break;

		case EVT_POINT: {
			const auto pt = V_POINT(variant);
			str = ExString::format(L"%d,%d", pt->x, pt->y);
		}break;
		case EVT_POINTF: {
			const auto pt = V_POINTF(variant);
			str = ExString::format(L"%f,%f", pt->x, pt->y);
		}break;
		case EVT_POINTU: {
			const auto pt = V_POINTU(variant);
			str = ExString::format(L"%f%s,%f%s",
				pt->x, ExNumberUnitToString(pt->units[0]),
				pt->y, ExNumberUnitToString(pt->units[1])
			);
		}break;

		case EVT_RECT: {
			const auto rc = V_RECT(variant);
			str = ExString::format(L"%d,%d,%d,%d",
				rc->left, rc->top, rc->right, rc->bottom
			);
		}break;
		case EVT_RECTF: {
			const auto rc = V_RECTF(variant);
			str = ExString::format(L"%f,%f,%f,%f",
				rc->left, rc->top, rc->right, rc->bottom
			);
		}break;
		case EVT_RECTU: {
			const auto rc = V_RECTU(variant);
			str = ExString::format(L"%f%s,%f%s,%f%s,%f%s",
				rc->left, ExNumberUnitToString(rc->units[0]),
				rc->top, ExNumberUnitToString(rc->units[1]),
				rc->right, ExNumberUnitToString(rc->units[2]),
				rc->bottom, ExNumberUnitToString(rc->units[3])
			);
		}break;

		case EVT_FONT: {
			const auto font = V_FONT(variant);
			str = ExString::format(
				L"FontInfo: %s %u 0x%04X 0x%08X",
				font->name,
				font->size,
				font->style,
				font->file_atom
			);
		}break;

		case EVT_GRIDS_IMAGE: {
			const auto gi = V_GRIDS_IMAGE(variant);
			str = ExString::format(
				L"GridsImageInfo: %f,%f,%f,%f 0x%08X",
				gi->left,
				gi->top,
				gi->right,
				gi->bottom,
				gi->flags
			);
		}break;

		case EVT_DISPLAY_IMAGE: {
			const auto di = V_DISPLAY_IMAGE(variant);
			str = ExString::format(
				L"DisplayImage: 0x%p (%f,%f,%f,%f) (%f,%f,%f,%f) %0x04X %0x04X %u",
				di->image,
				_expand_rect_(di->src),
				_expand_rect_(di->padding),
				di->image_mode,
				di->show_mode,
				di->alpha
			);
		}break;

		case EVT_STATE_IMAGE: {
			const auto si = V_STATE_IMAGE(variant);
			str = ExString::format(
				L"StateImage: 0x%p (%f,%f,%f,%f) %u",
				si->image,
				_expand_rect_(si->normal),
				si->alpha
			);
		}break;
		case EVT_STATE_COLOR: {
			const auto sc = V_STATE_COLOR(variant);
			str = ExString::format(
				L"StateColor: #08X #08X #08X #08X #08X",
				sc->normal,
				sc->hover,
				sc->press,
				sc->focus,
				sc->disable
			);
		}break;

		case EVT_ELE_STYLE: {
			const auto style = V_ELE_STYLE(variant);
			str = ExString::format(
				L"Style: 0x%08X 0x%08X 0x%08X",
				style->common,
				style->control,
				style->text_format
			);
		}break;

		case EVT_ELE_OPACITY: {
			const auto op = V_ELE_OPACITY(variant);
			str = ExString::format(
				L"Opacity: %u %f%%",
				op->normal,
				op->disable_percent * 100.f
			);
		}break;

		case EVT_ELE_ANIMATION: {
			const auto ani = V_ELE_ANIMATION(variant);
			str = ExString::format(
				L"Animation: %04X %u %u",
				ani->type,
				ani->duration,
				ani->frame_count
			);
		}break;

		case EVT_ELE_SHADOW: {
			//const auto shadow = V_ELE_SHADOW(variant);
			//if (shadow->type == ExEleShadowType::None) {
			//	str = L"ExShadowInfo: None";
			//}
			//else if (shadow->type == ExEleShadowType::Param) {
			//	const auto p = &shadow->info.param;
			//	str = ExString::format(
			//		L"",
			//		p->normal,
			//		p->padding
			//	);
			//}

		}break;

		default: handle_ex(E_NOTIMPL, L"不支持的变体类型"); break;
		}

		*r_str = SysAllocString(str.c_str());
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExVariantGetValuePtr(ExVariant* variant, void** r_value)
	{
		CHECK_PARAM(variant);
		CHECK_PARAM(r_value);

		bool isref = V_ISBYREF(variant);
		bool isarray = V_ISARRAY(variant);
		bool isvector = V_ISVECTOR(variant);

		//MARK:这里根据成员类型拿到对应成员的指针
		switch (V_VT(variant))
		{
		case VT_EMPTY: *r_value = nullptr; break;
		case VT_NULL: *r_value = nullptr; break;
		case VT_I2:*r_value = &V_I2(variant); break;
		case VT_I4:*r_value = &V_I4(variant); break;
		case VT_R4:*r_value = &V_R4(variant); break;
		case VT_R8:*r_value = &V_R8(variant); break;
		case VT_CY:*r_value = &V_CY(variant); break;
		case VT_DATE: *r_value = &V_DATE(variant); break;
		case VT_BSTR: *r_value = V_BSTR(variant); break;
		case VT_DISPATCH: *r_value = V_DISPATCH(variant); break;
		case VT_ERROR:  *r_value = &V_ERROR(variant); break;
		case VT_BOOL: *r_value = &V_BOOL(variant); break;
		case VT_VARIANT: *r_value = V_VARIANTREF(variant); break;
		case VT_UNKNOWN: *r_value = V_UNKNOWN(variant); break;
		case VT_DECIMAL: *r_value = &V_DECIMAL(variant); break;
		case VT_I1: *r_value = &V_I1(variant); break;
		case VT_UI1: *r_value = &V_UI1(variant); break;
		case VT_UI2:*r_value = &V_UI2(variant); break;
		case VT_UI4:*r_value = &V_UI4(variant); break;
		case VT_I8:*r_value = &V_I8(variant); break;
		case VT_UI8:*r_value = &V_UI8(variant); break;
		case VT_INT:*r_value = &V_INT(variant); break;
		case VT_UINT:*r_value = &V_UINT(variant); break;
		case VT_VOID:*r_value = V_BYREF(variant); break;
		case VT_HRESULT:*r_value = &V_UI4(variant); break;
		case VT_PTR: *r_value = V_BYREF(variant); break;
		case VT_SAFEARRAY: *r_value = V_ARRAY(variant); break;
		case VT_CARRAY: *r_value = V_ARRAY(variant); break;
		case VT_USERDEFINED: *r_value = V_BYREF(variant); break;
		case VT_LPSTR: *r_value = V_BYREF(variant); break;
		case VT_LPWSTR: *r_value = V_BYREF(variant); break;
		case VT_RECORD: *r_value = V_RECORD(variant); break;
		case VT_INT_PTR:  *r_value = &V_INT_PTR(variant); break;
		case VT_UINT_PTR: *r_value = &V_UINT_PTR(variant); break;
		case VT_FILETIME: *r_value = V_BYREF(variant); break;
			//case VT_BLOB: *r_value = &V_BLOB(variant); break;
			//case VT_STREAM: 
			//case VT_STORAGE:
			//case VT_STREAMED_OBJECT:
			//case VT_STORED_OBJECT:
			//case VT_BLOB_OBJECT:
			//case VT_CF:
			//case VT_CLSID:
			//case VT_VERSIONED_STREAM:

			///////////////////////////////////////////
			// 下面为扩展类型
			///////////////////////////////////////////

		case EVT_CHANNEL: *r_value = &V_UI1(variant); break;
		case EVT_COLOR: *r_value = &V_UI4(variant); break;
		case EVT_FLAGS: *r_value = &V_UI4(variant); break;
		case EVT_EXOBJECT: *r_value = V_EXOBJECT(variant); break;
		case EVT_DATA: *r_value = V_DATA(variant); break;
		case EVT_NUMU: *r_value = V_NUMU(variant); break;
		case EVT_POINT: *r_value = V_POINT(variant); break;
		case EVT_POINTF: *r_value = V_POINTF(variant); break;
		case EVT_POINTU: *r_value = V_POINTU(variant); break;
		case EVT_RECT: *r_value = V_RECT(variant); break;
		case EVT_RECTF: *r_value = V_RECTF(variant); break;
		case EVT_RECTU: *r_value = V_RECTU(variant); break;
		case EVT_FONT: *r_value = V_FONT(variant); break;
		case EVT_GRIDS_IMAGE: *r_value = V_GRIDS_IMAGE(variant); break;
		case EVT_DISPLAY_IMAGE: *r_value = V_DISPLAY_IMAGE(variant); break;
		case EVT_STATE_IMAGE: *r_value = V_STATE_IMAGE(variant); break;
		case EVT_STATE_COLOR: *r_value = V_STATE_COLOR(variant); break;
		case EVT_ELE_STYLE: *r_value = V_ELE_STYLE(variant); break;
		case EVT_ELE_OPACITY: *r_value = V_ELE_OPACITY(variant); break;
		case EVT_ELE_ANIMATION: *r_value = V_ELE_ANIMATION(variant); break;
		case EVT_ELE_SHADOW: *r_value = V_ELE_SHADOW(variant); break;

		default:
			*r_value = nullptr;
			handle_ex(E_NOTIMPL, L"不支持的变体类型");
			break;
		}

		return S_OK;
	}
	

	/////////////////////////

	inline ExRectF& _StateToRectF(ExStateImageInfo* image, DWORD state)
	{
		return image->normal;
	}

	inline EXARGB _StateToColor(ExStateColorInfo* colors, DWORD state)
	{
		return colors->normal;
	}

	inline EXARGB _StateToColor(ExEleShadowInfo* shadow, DWORD state)
	{
		return shadow->info.param.normal;
	}

	inline ExRectF& _StateToRectF(ExEleShadowInfo* shadow, DWORD state, EXCHANNEL& r_alpha)
	{
		r_alpha = shadow->info.texture.alpha_normal;
		return shadow->info.texture.src_normal;
	}

	
	HRESULT EXAPI EXCALL ExVariantDraw(IExCanvas* canvas, float left, float top, float right, float bottom, ExVariant* variant, DWORD state, LPARAM lparam)
	{
		CHECK_PARAM(canvas);
		CHECK_PARAM(variant);

		HRESULT hr;
		switch (V_VT(variant))
		{
		case EVT_COLOR: {
			EXARGB color = V_COLOR(variant);
			ExAutoPtr<IExSolidBrush> brush;
			return_if_failed(ExSolidBrushCreate(color, &brush), L"创建画刷失败");
			hr = canvas->FillRect(brush, left, top, right, bottom);
		}break;
		case EVT_DISPLAY_IMAGE: {
			auto image = V_DISPLAY_IMAGE(variant);
			if (image->image_mode == ExImageMode::Grids && !image->grids.IsEmpty()) {
				hr = canvas->DrawGridsImagePart(
					image->image, left, top, right, bottom,
					_expand_rect_(image->src),
					&image->grids, image->alpha
				);
			}
			else {
				hr = canvas->DrawImagePartRect(
					image->image, left, top, right, bottom,
					_expand_rect_(image->src),
					image->image_mode, image->alpha
				);
			}
		}break;
		case EVT_STATE_IMAGE: {
			auto image = V_STATE_IMAGE(variant);
			ExRectF& src = _StateToRectF(image, state);

			if (image->image_mode == ExImageMode::Grids && !image->grids.IsEmpty()) {
				hr = canvas->DrawGridsImagePart(
					image->image, left, top, right, bottom,
					_expand_rect_(src),
					&image->grids, image->alpha
				);
			}
			else {
				hr = canvas->DrawImagePartRect(
					image->image, left, top, right, bottom,
					_expand_rect_(src),
					image->image_mode, image->alpha
				);
			}
		}break;
		case EVT_STATE_COLOR: {
			auto colors = V_STATE_COLOR(variant);
			EXARGB color = _StateToColor(colors, state);

			if (ALPHA(color) != ALPHA_TRANSPARENT) {
				ExAutoPtr<IExSolidBrush> brush;
				throw_if_failed(ExSolidBrushCreate(color, &brush), L"创建画刷失败");
				hr = canvas->FillRect(brush, left, top, right, bottom);
			}
			else { hr = S_FALSE; }
		}break;
		case EVT_ELE_SHADOW: {
			auto shadow = V_ELE_SHADOW(variant);
			if (shadow->type == ExEleShadowType::Param) {
				auto& param = shadow->info.param;
				EXARGB color = _StateToColor(shadow, state);
				if (ALPHA(color) != ALPHA_TRANSPARENT && param.size > 0) {
					ExAutoPtr<IExSolidBrush> brush;
					throw_if_failed(ExSolidBrushCreate(color, &brush), L"创建画刷失败");
					hr = canvas->DrawShadow(
						brush, left, top, right, bottom,
						param.size, 0, 0, 0, 0,
						param.offset.x, param.offset.y
					);
				}
				else { hr = S_FALSE; }
			}
			else if (shadow->type == ExEleShadowType::Texture) {
				auto& texture = shadow->info.texture;
				EXCHANNEL alpha = ALPHA_OPAQUE;
				ExRectF& src = _StateToRectF(shadow, state, alpha);
				
				if (!texture.grids.IsEmpty()) {
					hr = canvas->DrawGridsImagePart(
						texture.image, left, top, right, bottom,
						_expand_rect_(src),
						&texture.grids, alpha
					);
				}
				else {
					hr = canvas->DrawImagePartRect(
						texture.image, left, top, right, bottom,
						_expand_rect_(src),
						ExImageMode::Scale, alpha
					);
				}
			}
			else { hr = S_FALSE; }
		}break;
			


		case EVT_EXOBJECT:
			//这里在做判断
			break;
		default:
			handle_if_failed(E_NOTIMPL, L"属性不支持绘制");
		}
		return hr;
	}

}


