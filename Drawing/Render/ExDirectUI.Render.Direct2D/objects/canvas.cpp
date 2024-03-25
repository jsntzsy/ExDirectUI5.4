/**
 * @file canvas.cpp
 * @brief 渲染器画布实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-25
 * @copyright
 */

#include "stdafx.h"
#include "objects/canvas.h"
#include "objects/region.h"

namespace ExDirectUI
{
	ExCanvasD2D::ExCanvasD2D(uint32_t width, uint32_t height)
	{
		//创建dc
		throw_if_failed(
			GetRender()->m_d2d_device->CreateDeviceContext(
				D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_dc
			), L"创建设备上下文对象失败"
		);

		//设置抗锯齿模式和单位
		SetAntiAliasMode(GetUtils()->GetAntialiasMode());
		m_dc->SetUnitMode(D2D1_UNIT_MODE_PIXELS);

		//创建初始画布目标
		throw_if_failed(
			CreateTarget(width, height, false, (IExCanvasTarget**)&m_target),
			L"创建画布目标失败"
		);
	}
	ExCanvasD2D::~ExCanvasD2D()
	{
		if (m_clip_region) { ResetClip(); }
		if (m_drawing) { EndDraw(); }
		if (m_target) { delete m_target; }
	}
	HRESULT EXOBJCALL ExCanvasD2D::CreateTarget(uint32_t width, uint32_t height, bool copy, IExCanvasTarget** r_target)
	{
		CHECK_PARAM(r_target);

		//默认参数处理
		if (width == 0) { width = m_target ? m_target->width : 1; }
		if (height == 0) { height = m_target ? m_target->height : 1; }

		//创建位图
		ExAutoPtr<ID2D1Bitmap1> bitmap;
		handle_if_failed(
			m_dc->CreateBitmap(
				D2D1::SizeU(width, height), nullptr, 0,
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
					_SYS_DEFAULT_DPI, _SYS_DEFAULT_DPI, nullptr),
				&bitmap
			), L"创建渲染目标位图失败"
		);

		//如果需要复制,则从当前目标复制一份
		if (copy && m_target) {
			D2D1_RECT_U rect = D2D1::RectU(
				0, 0,
				min(width, m_target->width),
				min(height, m_target->height)
			);

			handle_if_failed(bitmap->CopyFromBitmap(nullptr, m_target->bitmap, &rect), L"复制渲染位图失败");
		}

		//创建画布目标
		*r_target = NEW ExCanvasTargetD2D(this, bitmap, width, height);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DestroyTarget(IExCanvasTarget* target)
	{
		CHECK_PARAM(target);

		//不可以销毁当前目标
		handle_if_false(m_target != target, EE_NOREADY, L"不可以销毁当前渲染目标");

		//不可以销毁其他画布的目标
		handle_if_false(m_target->owner == this, E_INVALIDARG, L"不可以销毁其他画布的目标");

		//销毁
		delete (ExCanvasTargetD2D*)target;
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::SelectTarget(IExCanvasTarget* target, IExCanvasTarget** r_old_target)
	{
		if (r_old_target) { *r_old_target = m_target; }
		if (!target) { return S_FALSE; }

		//不可以选择其他画布的目标
		handle_if_false(((ExCanvasTargetD2D*)target)->owner == this, E_INVALIDARG, L"不可以选择其他画布的目标");

		//不可以在绘制过程中切换目标
		handle_if_false(m_drawing == false, EE_NOREADY, L"不可以在绘制过程中切换目标");

		//替换当前目标
		m_target = (ExCanvasTargetD2D*)target;
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetSize(uint32_t* r_width, uint32_t* r_height) const
	{
		if (r_width) { *r_width = m_target->width; }
		if (r_height) { *r_height = m_target->height; }
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::Resize(uint32_t width, uint32_t height)
	{
		//不可以在绘制过程中调整画布大小
		handle_if_false(m_drawing == false, EE_NOREADY, L"不可以在绘制过程中调整画布大小");

		//参数默认处理
		if (width == 0) { width = 1; }
		if (height == 0) { height = 1; }

		//没有变化直接返回
		if (m_target->width == width && m_target->height == height) { return S_FALSE; }

		//创建新的渲染目标位图
		ExAutoPtr<ID2D1Bitmap1> bitmap;
		handle_if_failed(
			m_dc->CreateBitmap(
				D2D1::SizeU(width, height), nullptr, 0,
				D2D1::BitmapProperties1(
					D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_GDI_COMPATIBLE,
					D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
					_SYS_DEFAULT_DPI, _SYS_DEFAULT_DPI, nullptr),
				&bitmap
			), L"创建渲染目标位图失败"
		);

		//更新当前画布成员
		m_target->bitmap = bitmap;
		m_target->width = width;
		m_target->height = height;
		return S_OK;
	}
	DWORD EXOBJCALL ExCanvasD2D::GetAntiAliasMode() const
	{
		return m_antialias_mode;
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetAntiAliasMode(DWORD mode)
	{
		//没有变化直接返回
		if (m_antialias_mode == mode) { return S_FALSE; }

		//设置成员变量
		m_antialias_mode = mode;

		//设置图像插值模式
		m_interpolation_mode =
			query_flags(mode, ExAntiAliasMode::ImageHighQuality) ? D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC :
			query_flags(mode, ExAntiAliasMode::Image) ? D2D1_INTERPOLATION_MODE_LINEAR :
			D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR;

		//设置图形抗锯齿模式
		m_dc->SetAntialiasMode(
			query_flags(mode, ExAntiAliasMode::Figure) ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE :
			D2D1_ANTIALIAS_MODE_ALIASED
		);

		//设置文本抗锯齿模式
		m_dc->SetTextAntialiasMode(
			query_flags(mode, ExAntiAliasMode::TextHighQuality) ? D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE : // D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE(有问题)
			query_flags(mode, ExAntiAliasMode::Text) ? D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE :
			D2D1_TEXT_ANTIALIAS_MODE_ALIASED
		);

		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetClipRect(ExRectF* r_clip) const
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetClipRegion(IExRegion* r_clip_region) const
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetClipRect(float left, float top, float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetClipRegion(IExRegion* clip_regioin)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::ResetClip()
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetTransform(ExMatrixElements* r_matrix) const
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetTransform(const ExMatrixElements* matrix)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetDC(HDC* r_dc)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::ReleaseDC()
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::Flush()
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::RenderToImage(IExImage* r_target_image)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::BeginDraw()
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::EndDraw()
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	bool EXOBJCALL ExCanvasD2D::IsDrawing() const
	{
		return m_drawing;
	}
	HRESULT EXOBJCALL ExCanvasD2D::Clear(EXARGB color)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawLine(const IExPen* pen, float x1, float y1, float x2, float y2)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawRect(const IExPen* pen, float left, float top,
		float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawEllipse(const IExPen* pen, float left, float top,
		float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawRoundRect(const IExPen* pen, float left, float top,
		float right, float bottom, float radius_x, float radius_y)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawCustomRoundRect(const IExPen* pen, float left, float top,
		float right, float bottom, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawPath(const IExPen* pen, const IExPath* path)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillRect(const IExBrush* brush, float left, float top,
		float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillEllipse(const IExBrush* brush, float left, float top,
		float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillRoundRect(const IExBrush* brush, float left, float top,
		float right, float bottom, float radius_x, float radius_y)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillCustomRoundRect(const IExBrush* brush, float left, float top,
		float right, float bottom, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillPath(const IExBrush* brush, const IExPath* path)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillRegion(const IExBrush* brush, const IExRegion* region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::CalcTextSize(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float max_width, float max_height,
		DWORD effect_type, LPARAM effect_param, float* r_width, float* r_height)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::CalcTextWithIcon(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float left, float top, float right, float bottom,
		DWORD effect_type, LPARAM effect_param, const IExImage* icon_image, float icon_width, float icon_height,
		DWORD icon_pos, float split_size, ExRectF* r_text_rect, ExRectF* r_icon_rect, ExRectF* r_content_rect)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::StrokeText(const IExPen* pen, const IExFont* font,
		LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillText(const IExBrush* brush, const IExFont* font,
		LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTextByColor(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float left, float top, float right, float bottom, EXARGB text_color)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawText(const IExBrush* brush, const IExFont* font,
		LPCWSTR text, uint32_t text_length, DWORD text_format,
		float left, float top, float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTextEx(const IExPen* stroke_pen, const IExBrush* fill_brush,
		const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
		float left, float top, float right, float bottom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTextEffect(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float left, float top, float right, float bottom,
		DWORD effect_type, EXARGB effect_color, LPARAM effect_param)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTextAndEffect(const IExPen* stroke_pen, const IExBrush* fill_brush,
		const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
		float left, float top, float right, float bottom,
		DWORD effect_type, EXARGB effect_color, LPARAM effect_param)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImage(const IExImage* image, float left, float top, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImageRect(const IExImage* image, float left, float top,
		float right, float bottom, ExImageMode mode, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImagePart(const IExImage* image, float left, float top,
		float src_left, float src_top, float src_right, float src_bottom, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImagePartRect(const IExImage* image, float left, float top,
		float right, float bottom, float src_left, float src_top, float src_right, float src_bottom, ExImageMode mode, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawGridsImage(const IExImage* image, float left, float top,
		float right, float bottom, ExGridsImageInfo* grids, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawGridsImagePart(const IExImage* image, float left, float top,
		float right, float bottom, float src_left, float src_top, float src_right, float src_bottom, ExGridsImageInfo* grids, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawShadow(const IExBrush* bkg_brush, float left, float top,
		float right, float bottom, float size, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom, float offset_x, float offset_y)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawShadowFromRegion(const IExBrush* bkg_brush, const IExRegion* region,
		float size, float offset_x, float offset_y)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTarget(const IExCanvasTarget* target_src, float left, float top,
		float right, float bottom, ExRectF* src_rect, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawCanvas(IExCanvas* canvas_src, float left, float top, float right, float bottom,
		float src_left, float src_top, ExCanvasDrawMode mode, EXCHANNEL alpha)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawEffect(const IExEffect* effect, float left, float top, LPARAM param)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::OnDrawImage(ID2D1Bitmap* bitmap, ExRectF& dst_rect, ExRectF& src_rect,
		ExImageMode mode, EXCHANNEL alpha, D2D1_INTERPOLATION_MODE interpolation)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	IDWriteTextLayout* EXOBJCALL ExCanvasD2D::MakeTextLayout(const IExFont* font, LPCWSTR text, uint32_t text_length,
		ExRectF rect, DWORD text_format)
	{
		return nullptr;
	}
}


