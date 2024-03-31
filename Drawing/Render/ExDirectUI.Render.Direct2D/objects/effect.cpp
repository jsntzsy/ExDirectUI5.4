/**
 * @file effect.cpp
 * @brief 渲染器效果实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */

#include "stdafx.h"
#include "objects/effect.h"
#include "objects/image.h"
#include "objects/canvas.h"

namespace ExDirectUI
{
	ExEffectD2D::ExEffectD2D()
	{
	}
	ExEffectD2D::~ExEffectD2D()
	{
	}
	bool EXOBJCALL ExEffectD2D::HaveSource()
	{
		return m_source;
	}
	bool EXOBJCALL ExEffectD2D::CanBeSource()
	{
		return true;
	}
	HRESULT EXOBJCALL ExEffectD2D::SetSource(IExRenderObject* source, const ExRectF* src_rect)
	{
		CHECK_PARAM(source);
		handle_if_false(m_effect, EE_NOREADY, L"效果器对象未创建成功");

		//尝试转为effect
		ExAutoPtr<ExEffectD2D> effect;
		if (SUCCEEDED(source->QueryInterface(__uuidof(IExEffect), (void**)&effect))) {
			handle_if_false(effect->CanBeSource(), E_NOTIMPL, L"该效果器对象不支持作为源");
			m_effect->SetInputEffect(0, effect->m_effect);
			m_source = effect->m_effect;
		}
		else {
			ExAutoPtr<ExImageD2D> image;
			ExAutoPtr<ExCanvasD2D> canvas;
			ID2D1Bitmap* source_bitmap = nullptr;

			// 尝试转为image或canvas
			if (SUCCEEDED(source->QueryInterface(__uuidof(IExImage), (void**)&image))) {
				source_bitmap = image->m_d2d_bitmap;
			}
			else if (SUCCEEDED(source->QueryInterface(__uuidof(IExCanvas), (void**)&canvas))) {
				ExAssert(canvas->m_target);
				source_bitmap = canvas->m_target->bitmap;
			}
			else { handle_ex(E_NOTIMPL, L"不支持的渲染对象源目标"); }
			handle_if_false(source_bitmap, EE_NOREADY, L"渲染对象源位图不存在");

			//获取真实源矩形
			D2D1_RECT_F rect;
			handle_if_failed(GetSourceRect(source, src_rect, rect), L"获取源矩形失败");

			//创建源位图
			ExAutoPtr<ID2D1Bitmap> bitmap;
			handle_if_failed(
				GetRender()->m_d2d_dc->CreateBitmap(
					D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top),
					D2D1::BitmapProperties(D2D1::PixelFormat(
						DXGI_FORMAT_B8G8R8A8_UNORM,
						D2D1_ALPHA_MODE_PREMULTIPLIED
					)), &bitmap
				), L"创建源位图失败"
			);

			//拷贝源图
			D2D1_RECT_U copy_rect = D2D1::RectU(rect.left, rect.top, rect.right, rect.bottom);
			handle_if_failed(
				bitmap->CopyFromBitmap(nullptr, source_bitmap, &copy_rect),
				L"拷贝源位图失败"
			);

			//设置到效果源
			m_effect->SetInput(0, bitmap);
			m_source = bitmap;
		}

		return S_OK;
	}
	HRESULT EXOBJCALL ExEffectD2D::SetSourceFromTarget(IExCanvasTarget* target, const ExRectF* src_rect)
	{
		CHECK_PARAM(target);
		ExCanvasTargetD2D* t = (ExCanvasTargetD2D*)target;
		CHECK_PARAM(t->bitmap);

		//获取真实源矩形
		D2D1_RECT_F rect;
		handle_if_failed(GetSourceRect(target, src_rect, rect), L"获取源矩形失败");

		//创建源位图
		ExAutoPtr<ID2D1Bitmap> bitmap;
		handle_if_failed(
			GetRender()->m_d2d_dc->CreateBitmap(
				D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top),
				D2D1::BitmapProperties(D2D1::PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					D2D1_ALPHA_MODE_PREMULTIPLIED
				)), &bitmap
			), L"创建源位图失败"
		);

		//拷贝源图
		D2D1_RECT_U copy_rect = D2D1::RectU(rect.left, rect.top, rect.right, rect.bottom);
		handle_if_failed(
			bitmap->CopyFromBitmap(nullptr, t->bitmap, &copy_rect),
			L"拷贝源位图失败"
		);

		//设置到效果源
		m_effect->SetInput(0, bitmap);
		m_source = bitmap;

		return S_OK;
	}
	HRESULT EXOBJCALL ExEffectD2D::OnDraw(IExCanvas* canvas, float left, float top, LPARAM lparam)
	{
		CHECK_PARAM(canvas);
		handle_if_false(m_effect, EE_NOREADY, L"效果器对象不存在");
		handle_if_false(m_source, EE_NOREADY, L"渲染器源对象不存在");
		ExCanvasD2D* cvs = (ExCanvasD2D*)canvas;
		return OnDrawBitmap(cvs, cvs->m_dc, left, top, lparam);
	}
	HRESULT EXOBJCALL ExEffectD2D::OnDrawBitmap(ExCanvasD2D* canvas, ID2D1DeviceContext* dc,
		float left, float top, LPARAM lparam)
	{
		D2D1_POINT_2F pt = D2D1::Point2F(left, top);
		dc->DrawImage(m_effect, &pt);
		return S_OK;
	}
	HRESULT ExEffectD2D::GetSourceRect(IExRenderObject* src, const ExRectF* src_rect, D2D1_RECT_F& real_rect)
	{
		ExAutoPtr<ExImageD2D> image;
		ExAutoPtr<ExCanvasD2D> canvas;

		// 尝试转为image或canvas
		if (SUCCEEDED(src->QueryInterface(__uuidof(IExImage), (void**)&image))) {
			if (src_rect) {
				real_rect = D2D1Rect(
					src_rect->left, src_rect->top,
					src_rect->right, src_rect->bottom
				);
			}
			else {
				real_rect = D2D1::RectF(
					0, 0,
					image->m_width,
					image->m_height
				);
			}
		}
		else if (SUCCEEDED(src->QueryInterface(__uuidof(IExCanvas), (void**)&canvas))) {
			ExCanvasTargetD2D* target = canvas->m_target;
			ExAssert(target);

			if (src_rect) {
				real_rect = D2D1Rect(
					src_rect->left, src_rect->top,
					src_rect->right, src_rect->bottom
				);
			}
			else {
				real_rect = D2D1::RectF(
					0, 0,
					target->width,
					target->height
				);
			}
		}
		else { handle_ex(E_NOTIMPL, L"不支持的渲染对象源目标"); }
		return S_OK;
	}
	HRESULT ExEffectD2D::GetSourceRect(IExCanvasTarget* src, const ExRectF* src_rect, D2D1_RECT_F& real_rect)
	{
		if (src_rect) {
			real_rect = D2D1Rect(
				src_rect->left, src_rect->top,
				src_rect->right, src_rect->bottom
			);
		}
		else {
			ExCanvasTargetD2D* target = (ExCanvasTargetD2D*)src;
			real_rect = D2D1::RectF(
				0, 0,
				target->width,
				target->height
			);
		}
		return S_OK;
	}
	ID2D1DeviceContext* ExEffectD2D::GetCommonDeviceContext()
	{
		return GetRender()->m_d2d_dc;
	}

}

///////////////////////////

#include "effects/BlurEffect.hpp"
#include "effects/HueRotateEffect.hpp"
#include "effects/3DPerspectiveEffect.hpp"
#include "effects/3DTransformEffect.hpp"


namespace ExDirectUI
{
	HRESULT ExEffectD2D::RegisterEffects(ExRenderD2D* render)
	{
		render->RegisterEffect(&ExBlurEffectD2D::EFFECT_INFO);
		render->RegisterEffect(&ExHueRotateEffectD2D::EFFECT_INFO);
		render->RegisterEffect(&Ex3DPerspectiveEffectD2D::EFFECT_INFO);
		render->RegisterEffect(&Ex3DTransformEffectD2D::EFFECT_INFO);

		return S_OK;
	}
}


