/**
 * @file image_brush.cpp
 * @brief 渲染器纯色画刷实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-22
 * @copyright
 */

#include "stdafx.h"
#include "objects/image_brush.h"
#include "objects/image.h"

namespace ExDirectUI
{
	ExImageBrushD2D::ExImageBrushD2D(const IExImage* image, const ExRectF* src, ExBrushExtendMode extend_mode, EXCHANNEL opacity)
	{
		m_image = (ExImageD2D*)image;
		m_opacity = opacity;
		m_extend_mode = extend_mode;

		if (src) { m_src_rect = Rect(*src); }
		else {
			m_src_rect.left = m_src_rect.top = 0;
			m_src_rect.right = (float)m_image->GetWidth();
			m_src_rect.bottom = (float)m_image->GetHeight();
		}

		throw_if_failed(Flush(), L"创建图像画刷失败");
	}

	ExImageBrushD2D::~ExImageBrushD2D()
	{
	}

	HRESULT EXOBJCALL ExImageBrushD2D::GetTransform(ExMatrix* r_tranform) const
	{
		CHECK_PARAM(r_tranform);
		*r_tranform = Matrix(m_transform_user);
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageBrushD2D::SetTransform(const ExMatrix* tranform)
	{
		m_transform_user = Matrix(tranform);
		m_brush->SetTransform(m_transform_src * m_transform_user);
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageBrushD2D::GetExtendMode(ExBrushExtendMode* r_mode) const
	{
		CHECK_PARAM(r_mode);
		*r_mode = m_extend_mode;
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageBrushD2D::SetExtendMode(ExBrushExtendMode mode)
	{
		if (m_extend_mode != mode) {
			m_extend_mode = mode;
			auto extend = GetD2DExtendMode(mode);
			m_brush->SetExtendModeX(extend);
			m_brush->SetExtendModeY(extend);
		}
		return S_OK;
	}

	HRESULT EXOBJCALL ExImageBrushD2D::GetImage(IExImage** r_image)
	{
		CHECK_PARAM(r_image);
		*r_image = m_image;
		m_image->AddRef();
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageBrushD2D::GetSourceRect(ExRectF* r_src_rect) const
	{
		CHECK_PARAM(r_src_rect);
		*r_src_rect = Rect(m_src_rect);
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageBrushD2D::SetImage(IExImage* image, ExRectF* src_rect)
	{
		//如果修改了图像
		if (image != nullptr && image != m_image) {
			m_image = (ExImageD2D*)image;
			m_brush->SetImage(m_image->m_d2d_bitmap);
		}

		//设置源矩形
		if (src_rect) { m_src_rect = Rect(*src_rect); }
		else {
			m_src_rect.left = m_src_rect.top = 0;
			m_src_rect.right = (float)m_image->GetWidth();
			m_src_rect.bottom = (float)m_image->GetHeight();
		}
		m_brush->SetSourceRectangle(&m_src_rect);

		//获取原始变换
		m_brush->GetTransform(&m_transform_src);

		return S_OK;
	}

	HRESULT EXOBJCALL ExImageBrushD2D::TransformToRect(float left, float top, float right, float bottom)
	{
		//获取最终目标矩形
		ExRectF dst_rect = ExRectF(left, top, right, bottom).Normalize();

		//计算缩放比例
		float scale_x = dst_rect.Width() / m_image->GetWidth();
		float scale_y = dst_rect.Height() / m_image->GetHeight();

		//生成变换矩阵
		ExMatrix3x2 transform;
		transform.Scale(scale_x, scale_y).Translate(left, top);

		//设置变换
		m_transform_user = Matrix(ExMatrix3x2::MakeScale(scale_x, scale_y).Translate(left, top));
		m_brush->SetTransform(m_transform_src * m_transform_user);
		return S_OK;
	}

	EXCHANNEL EXOBJCALL ExImageBrushD2D::GetOpacity() const
	{
		return m_opacity;
	}
	HRESULT EXOBJCALL ExImageBrushD2D::SetOpacity(EXCHANNEL alpha)
	{
		if (alpha != m_opacity) {
			m_opacity = alpha;
			m_brush->SetOpacity(alpha / 255.0F);
		}
		return S_OK;
	}

	HRESULT EXOBJCALL ExImageBrushD2D::Flush()
	{
		auto render = GetRender();

		//获取扩展模式
		auto extend_mode = GetD2DExtendMode(m_extend_mode);

		//获取插值模式
		DWORD antialias_mode = GetUtils()->GetAntialiasMode();
		auto interpolation_mode =
			query_flags(antialias_mode, ExAntiAliasMode::ImageHighQuality) ? D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC :
			query_flags(antialias_mode, ExAntiAliasMode::Image) ? D2D1_INTERPOLATION_MODE_LINEAR :
			D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

		//创建图像画刷
		ExAutoPtr<ID2D1ImageBrush> brush;
		handle_if_failed(
			render->m_d2d_dc->CreateImageBrush(
				m_image->m_d2d_bitmap, D2D1::ImageBrushProperties(
					m_src_rect,
					extend_mode, extend_mode,
					interpolation_mode
				), D2D1::BrushProperties(m_opacity / 255.0F), &brush
			), L"创建图像画刷失败"
		);

		//应用矩阵变换
		if (!IsIdentityMatrix(m_transform_user)) {
			brush->SetTransform(m_transform_src * m_transform_user);
		}

		//替换成员变量
		m_brush = brush;
		return S_OK;
	}

}


