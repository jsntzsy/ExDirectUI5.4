/**
 * @file canvas_brush.cpp
 * @brief 渲染器画布画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */

#include "stdafx.h"
#include "objects/canvas_brush.h"
#include "objects/canvas.h"

namespace ExDirectUI
{
	ExCanvasBrushD2D::ExCanvasBrushD2D(const IExCanvas* canvas_src, ExBrushExtendMode extend_mode, EXCHANNEL alpha)
		:ExCanvasBrushD2D(((ExCanvasD2D*)canvas_src)->m_target, extend_mode, alpha)
	{
	}
	ExCanvasBrushD2D::ExCanvasBrushD2D(const IExCanvasTarget* target_src, ExBrushExtendMode extend_mode, EXCHANNEL alpha)
	{
		throw_if_false(target_src, EE_NOREADY, L"渲染目标为空");
		ExCanvasTargetD2D* target = (ExCanvasTargetD2D*)target_src;

		//获取扩展模式
		m_extend_mode = extend_mode;
		D2D1_EXTEND_MODE mode = GetD2DExtendMode(extend_mode);

		//生成属性
		D2D1_BRUSH_PROPERTIES bp = D2D1::BrushProperties(alpha / 255.0F);
		D2D1_BITMAP_BRUSH_PROPERTIES1 bbp = D2D1::BitmapBrushProperties1(
			mode, mode, target->owner->m_interpolation_mode
		);

		//创建画刷
		throw_if_failed(
			target->owner->m_dc->CreateBitmapBrush(
				target->bitmap, bbp, bp, &m_brush
			), L"创建位图画刷失败"
		);
	}
	ExCanvasBrushD2D::~ExCanvasBrushD2D()
	{
	}
	HRESULT EXOBJCALL ExCanvasBrushD2D::GetTransform(ExMatrix* r_tranform) const
	{
		CHECK_PARAM(r_tranform);

		m_brush->GetTransform((D2D1_MATRIX_3X2_F*)r_tranform);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasBrushD2D::SetTransform(const ExMatrix* tranform)
	{
		D2D1_MATRIX_3X2_F mx = Matrix(tranform);
		m_brush->SetTransform(mx);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasBrushD2D::GetExtendMode(ExBrushExtendMode* r_mode) const
	{
		CHECK_PARAM(r_mode);
		*r_mode = m_extend_mode;
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasBrushD2D::SetExtendMode(ExBrushExtendMode mode)
	{
		if (m_extend_mode == mode) { return S_FALSE; }

		D2D1_EXTEND_MODE extend = GetD2DExtendMode(mode);
		m_brush->SetExtendModeX(extend);
		m_brush->SetExtendModeY(extend);
		return S_OK;
	}
}


