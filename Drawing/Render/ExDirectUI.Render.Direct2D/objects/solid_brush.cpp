/**
 * @file solid_brush.cpp
 * @brief 渲染器纯色画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */

#include "stdafx.h"
#include "objects/solid_brush.h"

namespace ExDirectUI
{
	ExSolidBrushD2D::ExSolidBrushD2D(EXARGB color)
	{
		throw_if_failed(
			GetRender()->m_d2d_dc->CreateSolidColorBrush(Color(color), &m_brush),
			L"纯色画刷创建失败"
		);
	}
	ExSolidBrushD2D::~ExSolidBrushD2D()
	{
	}
	HRESULT EXOBJCALL ExSolidBrushD2D::GetTransform(ExMatrix* r_tranform) const
	{
		handle_ex(E_NOTIMPL, L"不支持的操作");
	}
	HRESULT EXOBJCALL ExSolidBrushD2D::SetTransform(const ExMatrix* tranform)
	{
		handle_ex(E_NOTIMPL, L"不支持的操作");
	}
	HRESULT EXOBJCALL ExSolidBrushD2D::GetExtendMode(ExBrushExtendMode* r_mode) const
	{
		handle_ex(E_NOTIMPL, L"不支持的操作");
	}
	HRESULT EXOBJCALL ExSolidBrushD2D::SetExtendMode(ExBrushExtendMode mode)
	{
		handle_ex(E_NOTIMPL, L"不支持的操作");
	}
	EXARGB EXOBJCALL ExSolidBrushD2D::GetColor() const
	{
		return Color(m_brush->GetColor());
	}
	HRESULT EXOBJCALL ExSolidBrushD2D::SetColor(EXARGB color)
	{
		m_brush->SetColor(Color(color));
		return S_OK;
	}
}


