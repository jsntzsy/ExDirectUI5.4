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
	//TODO: 考虑直接采用各类指针来管理，不统一采用一个大的ExVariantExtendValues
	//因为可能不一定所有存储都需要这么大空间，会造成内存的浪费

	ExMemPool<ExVariantExtendValues> m_variant_pool;

	HRESULT EXAPI EXCALL ExVariantInit(ExVariant* variant, VARTYPE vt)
	{
		CHECK_PARAM(variant);

		//先由系统初始化
		VariantInit(variant);

		//如果是扩展指针内容,则生成对应的扩展块
		if (IS_EVTR(vt)) {
			auto ext = m_variant_pool.Alloc();
			handle_if_false(ext, E_OUTOFMEMORY, L"分配内存失败");
			ZeroMemory(ext, sizeof(*ext));
			V_BYREF(variant) = ext;
			V_VT(variant) = vt;
		}

		//设置初始类型
		V_VT(variant) = vt;
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExVariantClear(ExVariant* variant)
	{
		CHECK_PARAM(variant);

		//如果是扩展类型
		if (V_ISEVT(variant)) {

			//根据类型判断是否要释放对应指针
			switch (V_VT(variant))
			{
			case EVT_OBJECT:
				SAFE_RELEASE(V_UNKNOWN(variant));
				break;
			case EVT_DATA:
				ExDataFree(&V_EXF(variant, data_));
				break;

			}

			//如果是扩展指针内容,则释放对应扩展块
			if (V_ISEVTR(variant) && V_EX(variant)) {
				m_variant_pool.Free(V_EX(variant));
				V_BYREF(variant) = nullptr;
				V_VT(variant) = VT_NULL;
				//return S_OK;
			}
		}

		//由操作系统清空
		return VariantClear(variant);
	}
	HRESULT EXAPI EXCALL ExVariantCopy(ExVariant* dest_variant, const ExVariant* src_variant)
	{
		CHECK_PARAM(dest_variant);
		CHECK_PARAM(src_variant);

		//保存当前类型
		VARTYPE vt = V_VT(src_variant);
		ExVariant var = *src_variant;

		//如果是扩展内容
		if (IS_EVT(vt)) {

			//根据类型判断是否额外处理
			switch (vt)
			{
			case EVT_OBJECT: V_VT(&var) = VT_UNKNOWN; break;	//以IUnknown处理
			case EVT_COLOR:
			case EVT_FLAGS: V_VT(&var) = VT_UI4; break;

			}

			//如果是扩展指针内容,则复制扩展块
			if (IS_EVTR(vt) && V_EX(src_variant)) {

				//先初始化
				handle_if_failed(ExVariantInit(dest_variant), L"初始化目标变体变量失败");

				//复制扩展块内容
				CopyMemory(V_EX(dest_variant), V_EX(src_variant), sizeof(ExVariantExtendValues));

				//根据类型判断是否还需要复制对应的指针
				switch (vt)
				{
				case EVT_DATA:
					if (V_EXF(src_variant, data_).data) {
						return ExDataCopy(
							&V_EXF(dest_variant, data_),
							&V_EXF(src_variant, data_)
						);
					}
					break;
					
				}

				//不用系统继续处理了
				return S_OK;
			}
		}

		//使用系统处理复制，并将类型设置回去
		handle_if_failed(VariantCopy(dest_variant, &var), L"变体对象复制失败");
		V_VT(dest_variant) = vt;

		return S_OK;
	}
	HRESULT EXAPI EXCALL ExVariantToString(ExVariant* variant, VARTYPE vt, BSTR* r_str)
	{
		CHECK_PARAM(variant);
		CHECK_PARAM(r_str);

		if (vt == VT_EMPTY) { vt = V_VT(variant); }

		//如果是扩展内容，则根据类型进行转换
		if (IS_EVT(vt)) {
			std::wstring str;

			switch (vt)
			{
			case EVT_COLOR:
				str = ExString::format(L"#08X", V_UI4(variant));
				break;
			case EVT_FLAGS:
				str = ExString::format(L"0x08X", V_UI4(variant));
				break;
			case EVT_OBJECT:
				if (V_EXOBJ(variant)) { str = V_EXOBJ(variant)->ToString(); }
				break;
			}

			if (IS_EVTR(vt) && V_EX(variant)) {

				switch (vt)
				{
				case EVT_NUMU: {
					auto& n = V_EXF(variant, numu_);
					str = ExString::format(L"%f%s", n.value, ExNumberUnitToString(n.unit));
				}break;
				case EVT_POINT: {
					auto& pt = V_EXF(variant, point_);
					str = ExString::format(L"%d,%d", _expand_point_(pt));
				}break;
				case EVT_RECT: {
					auto& rc = V_EXF(variant, rect_);
					str = ExString::format(L"%d,%d,%d,%d", _expand_rect_(rc));
				}break;
				case EVT_POINTF: {
					auto& pt = V_EXF(variant, pointf_);
					str = ExString::format(L"%f,%f", _expand_point_(pt));
				}break;
				case EVT_RECTF: {
					auto& rc = V_EXF(variant, rectf_);
					str = ExString::format(L"%f,%f,%f,%f", _expand_rect_(rc));
				}break;
				case EVT_POINTU: {
					auto& pt = V_EXF(variant, pointu_);
					str = ExString::format(L"%f%s,%f%s",
						pt.x, ExNumberUnitToString(pt.units[0]),
						pt.y, ExNumberUnitToString(pt.units[1])
					);
				}break;
				case EVT_RECTU: {
					auto& rc = V_EXF(variant, rectu_);
					str = ExString::format(L"%f%s,%f%s,%f%s,%f%s",
						rc.left, ExNumberUnitToString(rc.units[0]),
						rc.top, ExNumberUnitToString(rc.units[1]),
						rc.right, ExNumberUnitToString(rc.units[2]),
						rc.bottom, ExNumberUnitToString(rc.units[3])
					);
				}break;
				case EVT_DATA: {
					auto& data = V_EXF(variant, data_);
					str = ExString::format(L"0x%p[%zu]", data.data, data.size);
				}break;
				case EVT_ELE_OPACITY: {
					auto& opacity = V_EXF(variant, ele_opacity_);
					str = ExString::format(L"opacity: %u, disable: %f%",
						opacity.normal, opacity.disable_percent * 100.f);
				}break;
				}
			}

			*r_str = SysAllocString(str.c_str());
			handle_if_false(*r_str, E_OUTOFMEMORY, L"分配内存失败");
			return S_OK;
		}

		ExVariant var{};
		V_VT(&var) = vt;
		handle_if_failed(
			VariantChangeType(&var, variant, 0, VT_BSTR),
			L"变体类型转换失败"
		);

		//不用做清理,直接返回
		*r_str = var.bstrVal;
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExVariantGetValuePtr(ExVariant* variant, void** r_value)
	{
		CHECK_PARAM(variant);
		CHECK_PARAM(r_value);

		bool isref = V_ISBYREF(variant);
		bool isarray = V_ISARRAY(variant);
		bool isvector = V_ISVECTOR(variant);
		bool isex = V_ISEVT(variant);

		if (isex) {

			switch (V_VT(variant))
			{
			case EVT_COLOR: *r_value = &V_UI4(variant); break;
			case EVT_FLAGS: *r_value = &V_UI4(variant); break;
			case EVT_OBJECT: *r_value = V_UNKNOWN(variant); break;

			case EVT_NUMU: *r_value = &V_EXF(variant, numu_); break;
			case EVT_POINT: *r_value = &V_EXF(variant, point_); break;
			case EVT_RECT: *r_value = &V_EXF(variant, rect_); break;
			case EVT_POINTF: *r_value = &V_EXF(variant, pointf_); break;
			case EVT_RECTF: *r_value = &V_EXF(variant, rectf_); break;
			case EVT_POINTU: *r_value = &V_EXF(variant, pointu_); break;
			case EVT_RECTU: *r_value = &V_EXF(variant, rectu_); break;
			case EVT_ELE_OPACITY: *r_value = &V_EXF(variant, ele_opacity_); break;

			default:
				*r_value = nullptr;
				handle_ex(E_NOTIMPL, L"不支持的变体类型");
				break;
			}
		}


		switch (V_VT(variant) & VT_TYPEMASK)
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

		default:
			*r_value = nullptr;
			handle_ex(E_NOTIMPL, L"不支持的变体类型");
			break;
		}

		return S_OK;
	}
}


