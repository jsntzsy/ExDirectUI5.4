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
#include "objects/pen.h"
#include "objects/path.h"
#include "objects/font.h"
#include "objects/image.h"

#include "objects/text_render.hpp"

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
		*r_target = new ExCanvasTargetD2D(this, bitmap, width, height);
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
		CHECK_PARAM(r_clip);
		if (m_clip_region == nullptr) {
			*r_clip = {};
			return S_FALSE;
		}
		return m_clip_region->GetBounds(r_clip);
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetClipRegion(IExRegion* r_clip_region) const
	{
		CHECK_PARAM(r_clip_region);
		if (m_clip_region == nullptr) { return S_FALSE; }
		return r_clip_region->CombineWithRegion(m_clip_region, ExRegionCombineMode::Copy);
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetClipRect(float left, float top, float right, float bottom)
	{
		//必须在绘制过程中设置
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中设置裁剪区域");

		try
		{
			//如果已经有裁剪区域,则先清除
			if (m_clip_region) { throw_if_failed(ResetClip(), L"重置剪辑区失败"); }

			//创建新的裁剪区域对象
			D2D1_RECT_F clip = D2D1Rect(left, top, right, bottom);
			m_clip_region = new ExRegionD2D(clip.left, clip.top, clip.right, clip.bottom, true);

			//压入裁剪区域
			m_dc->PushAxisAlignedClip(clip, m_dc->GetAntialiasMode());
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetClipRegion(IExRegion* clip_regioin)
	{
		//必须在绘制过程中设置
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中设置裁剪区域");

		try
		{
			//如果已经有裁剪区域,则先清除
			if (m_clip_region) { throw_if_failed(ResetClip(), L"重置剪辑区失败"); }

			if (clip_regioin) {

				//创建新的裁剪区域对象
				ExAutoPtr<ExRegionD2D> region = new ExRegionD2D(clip_regioin);

				//压入裁剪区域
				ExAutoPtr<ID2D1Layer> layer;
				throw_if_failed(m_dc->CreateLayer(&layer), L"创建层对象失败");

				m_dc->PushLayer(
					D2D1::LayerParameters(
						D2D1::InfiniteRect(),
						region->m_geometry,
						m_dc->GetAntialiasMode()
					), layer
				);
			}

			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExCanvasD2D::ResetClip()
	{
		//必须在绘制过程中设置
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中设置裁剪区域");

		//如果没有裁剪区域,则直接返回
		if (m_clip_region == nullptr) { return S_FALSE; }

		//否则根据裁剪区域类型进行处理
		if (m_clip_region->m_is_clip) {
			m_dc->PopAxisAlignedClip();
		}
		else {
			m_dc->PopLayer();
			m_layer.Release();
		}

		m_clip_region.Release();
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetTransform(ExMatrixElements* r_matrix) const
	{
		CHECK_PARAM(r_matrix);

		//必须在绘制过程中
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中获取");

		//获取当前变换矩阵
		m_dc->GetTransform((D2D1_MATRIX_3X2_F*)r_matrix);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::SetTransform(const ExMatrixElements* matrix)
	{
		//必须在绘制过程中
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中设置");

		//设置变换矩阵
		D2D1_MATRIX_3X2_F tranform = Matrix(matrix);
		m_dc->SetTransform(tranform);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::GetDC(HDC* r_dc)
	{
		CHECK_PARAM(r_dc);

		//必须在绘制过程中
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中获取");
		handle_if_false(m_gdi, E_NOTIMPL, L"GDI兼容对象不存在");

		handle_if_failed(m_gdi->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, r_dc), L"获取DC失败");
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::ReleaseDC()
	{
		//必须在绘制过程中
		handle_if_false(m_drawing, EE_NOREADY, L"必须在绘制过程中释放");
		handle_if_false(m_gdi, EE_NOREADY, L"GDI兼容对象不存在");

		handle_if_failed(m_gdi->ReleaseDC(nullptr), L"释放DC失败");
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::Flush()
	{
		handle_if_failed(m_dc->Flush(), L"刷新失败");
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::RenderToImage(IExImage* r_target_image)
	{
		CHECK_PARAM(r_target_image);
		ExMemDC temp_dc{};
		HDC src_dc = NULL;
		ExImageLock lock{};

		bool temp_draw = !m_drawing;
		if (temp_draw) { handle_if_failed(BeginDraw(), L"开始绘制画布失败"); }

		try
		{
			throw_if_false(m_gdi, E_NOTIMPL, L"GDI兼容对象不存在");

			//获取画布和位图大小
			uint32_t width = m_target->width;
			uint32_t height = m_target->height;
			uint32_t image_width = r_target_image->GetWidth();
			uint32_t image_height = r_target_image->GetHeight();

			//不超过图像尺寸
			if (width > image_width) { width = image_width; }
			if (height > image_height) { height = image_height; }

			//刷新当前画布
			throw_if_failed(m_dc->Flush(), L"画布刷新失败");

			//获取画布DC
			throw_if_failed(m_gdi->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &src_dc), L"获取画布DC失败");

			//创建临时DC
			throw_if_failed(ExMemDCCreate(width, height, &temp_dc), L"创建内存DC失败");

			//将画布内容复制到内存DC
			BLENDFUNCTION bf = { 0,0,ALPHA_OPAQUE,AC_SRC_OVER };
			throw_if_false(
				GdiAlphaBlend(temp_dc.dc, 0, 0, width, height,
					src_dc, 0, 0, width, height,
					bf
				), E_FAIL, L"透明混合拷贝失败"
			);

			//释放画布DC
			throw_if_failed(m_gdi->ReleaseDC(nullptr), L"释放画布DC失败");

			//锁定图像
			throw_if_failed(r_target_image->Lock(&lock), L"锁定图像失败");

			//拷贝图像数据
			throw_if_failed(
				ExImageCopyData(width, height, lock.data, temp_dc.bits,
					lock.stride, width * sizeof(EXARGB)
				), L"拷贝图像数据失败"
			);

			//解锁图像
			throw_if_failed(r_target_image->Unlock(&lock), L"解锁图像失败");

			//扫尾工作
			ExMemDCDestroy(&temp_dc);
			if (temp_draw) { throw_if_failed(EndDraw(), L"绘制过程出错"); }

			return S_OK;
		}
		catch_default({
				if (lock.data) { r_target_image->Unlock(&lock); }
				if (temp_dc.dc) { ExMemDCDestroy(&temp_dc); }
				if (src_dc) { m_gdi->ReleaseDC(nullptr); }
				if (temp_draw) { EndDraw(); }
			}
		);
	}
	HRESULT EXOBJCALL ExCanvasD2D::BeginDraw()
	{
		//检查画布是否已经在绘制
		handle_if_false(m_drawing == false, EE_NOREADY, L"画布正在绘制中");
		handle_if_false(m_target, EE_NOREADY, L"渲染目标不存在");

		//设置DC渲染目标位图
		m_dc->SetTarget(m_target->bitmap);

		//开始绘制
		m_dc->BeginDraw();

		//设置为标准变换矩阵
		m_dc->SetTransform(D2D1::Matrix3x2F::Identity());

		//获取GDI兼容对象
		m_dc->QueryInterface(&m_gdi);

		//设置开始绘制标志
		m_drawing = true;
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::EndDraw()
	{
		//判断是否在绘制中
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//清除剪辑区
		ResetClip();

		//重置变换矩阵
		m_dc->SetTransform(D2D1::Matrix3x2F::Identity());

		//结束绘制
		HRESULT hr = m_dc->EndDraw();

		//释放GDI对象
		m_gdi.Release();

		//置空渲染目标位图
		m_dc->SetTarget(nullptr);

		//标识当前未绘制状态
		m_drawing = false;

		handle_if_failed(hr, L"绘制过程出现错误");
		return S_OK;
	}
	bool EXOBJCALL ExCanvasD2D::IsDrawing() const
	{
		return m_drawing;
	}
	HRESULT EXOBJCALL ExCanvasD2D::Clear(EXARGB color)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		m_dc->Clear(Color(color));
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawLine(const IExPen* pen, float x1, float y1, float x2, float y2)
	{
		CHECK_PARAM(pen);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, x1, y1, x2, y2);

		//获取相关参数
		ExPenD2D* p = (ExPenD2D*)pen;
		ID2D1Brush* br = (ID2D1Brush*)p->m_brush->GetContext(0);

		//绘制直线
		m_dc->DrawLine(
			D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2),
			br, p->m_width, p->m_style
		);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawRect(const IExPen* pen, float left, float top,
		float right, float bottom)
	{
		CHECK_PARAM(pen);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获取相关参数
		ExPenD2D* p = (ExPenD2D*)pen;
		ID2D1Brush* br = (ID2D1Brush*)p->m_brush->GetContext(0);

		//绘制矩形
		m_dc->DrawRectangle(
			D2D1Rect(left, top, right, bottom),
			br, p->m_width, p->m_style
		);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawEllipse(const IExPen* pen, float left, float top,
		float right, float bottom)
	{
		CHECK_PARAM(pen);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获取相关参数
		ExPenD2D* p = (ExPenD2D*)pen;
		ID2D1Brush* br = (ID2D1Brush*)p->m_brush->GetContext(0);

		//绘制椭圆
		ExRectF rect = ExRectF(left, top, right, bottom).Normalize();
		m_dc->DrawEllipse(
			D2D1::Ellipse(
				D2D1::Point2F(rect.GetHorzCenter(), rect.GetVertCenter()),
				rect.Width() / 2, rect.Height() / 2
			), br, p->m_width, p->m_style
		);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawRoundRect(const IExPen* pen, float left, float top,
		float right, float bottom, float radius)
	{
		CHECK_PARAM(pen);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获取相关参数
		ExPenD2D* p = (ExPenD2D*)pen;
		ID2D1Brush* br = (ID2D1Brush*)p->m_brush->GetContext(0);

		//绘制圆角矩形
		m_dc->DrawRoundedRectangle(
			D2D1::RoundedRect(
				D2D1Rect(left, top, right, bottom), radius, radius
			), br, p->m_width, p->m_style
		);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawCustomRoundRect(const IExPen* pen, float left, float top,
		float right, float bottom, float radius_left_top, float radius_right_top,
		float radius_right_bottom, float radius_left_bottom)
	{
		CHECK_PARAM(pen);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移(生成圆角矩形时处理)
		//_offset_(false, left, top, right, bottom);

		//获取相关参数
		ExPenD2D* p = (ExPenD2D*)pen;
		ID2D1Brush* br = (ID2D1Brush*)p->m_brush->GetContext(0);

		try
		{
			//创建路径几何形
			ExAutoPtr<ID2D1PathGeometry> geometry;
			throw_if_failed(
				GetRender()->m_d2d_factory->CreatePathGeometry(&geometry),
				L"创建路径几何形失败"
			);

			//生成圆角矩形路径
			ExAutoPtr<ID2D1GeometrySink> sink;
			throw_if_failed(geometry->Open(&sink), L"开始描述路径失败");
			ExPathD2D::MakeRoundedRect(sink, left, top, right, bottom,
				radius_left_top, radius_right_top, radius_right_bottom, radius_left_bottom
			);
			throw_if_failed(sink->Close(), L"描述几何形路径错误");

			//绘制几何形
			m_dc->DrawGeometry(geometry, br, p->m_width, p->m_style);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawPath(const IExPen* pen, const IExPath* path)
	{
		CHECK_PARAM(pen);
		CHECK_PARAM(path);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//获取相关参数
		ExPenD2D* p = (ExPenD2D*)pen;
		ID2D1Brush* br = (ID2D1Brush*)p->m_brush->GetContext(0);
		ID2D1Geometry* geometry = ((ExPathD2D*)path)->m_geometry;

		//绘制几何形
		m_dc->DrawGeometry(geometry, br, p->m_width, p->m_style);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillRect(const IExBrush* brush, float left, float top,
		float right, float bottom)
	{
		CHECK_PARAM(brush);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获取相关参数
		ID2D1Brush* br = (ID2D1Brush*)brush->GetContext(0);

		//填充矩形
		m_dc->FillRectangle(D2D1Rect(left, top, right, bottom), br);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillEllipse(const IExBrush* brush, float left, float top,
		float right, float bottom)
	{
		CHECK_PARAM(brush);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获取相关参数
		ID2D1Brush* br = (ID2D1Brush*)brush->GetContext(0);

		//填充椭圆
		ExRectF rect = ExRectF(left, top, right, bottom).Normalize();
		m_dc->FillEllipse(
			D2D1::Ellipse(
				D2D1::Point2F(rect.GetHorzCenter(), rect.GetVertCenter()),
				rect.Width() / 2, rect.Height() / 2
			), br
		);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillRoundRect(const IExBrush* brush, float left, float top,
		float right, float bottom, float radius)
	{
		CHECK_PARAM(brush);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获取相关参数
		ID2D1Brush* br = (ID2D1Brush*)brush->GetContext(0);

		//填充圆角矩形
		m_dc->FillRoundedRectangle(
			D2D1::RoundedRect(
				D2D1Rect(left, top, right, bottom), radius, radius
			), br
		);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillCustomRoundRect(const IExBrush* brush, float left, float top,
		float right, float bottom, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom)
	{
		CHECK_PARAM(brush);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移(生成圆角矩形时处理)
		//_offset_(false, left, top, right, bottom);

		//获取相关参数
		ID2D1Brush* br = (ID2D1Brush*)brush->GetContext(0);

		try
		{
			//创建路径几何形
			ExAutoPtr<ID2D1PathGeometry> geometry;
			throw_if_failed(
				GetRender()->m_d2d_factory->CreatePathGeometry(&geometry),
				L"创建路径几何形失败"
			);

			//生成圆角矩形路径
			ExAutoPtr<ID2D1GeometrySink> sink;
			throw_if_failed(geometry->Open(&sink), L"开始描述路径失败");
			ExPathD2D::MakeRoundedRect(sink, left, top, right, bottom,
				radius_left_top, radius_right_top, radius_right_bottom, radius_left_bottom
			);
			throw_if_failed(sink->Close(), L"描述几何形路径错误");

			//填充几何形
			m_dc->FillGeometry(geometry, br);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillPath(const IExBrush* brush, const IExPath* path)
	{
		CHECK_PARAM(brush);
		CHECK_PARAM(path);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//获取相关参数
		ID2D1Brush* br = (ID2D1Brush*)brush->GetContext(0);
		ID2D1Geometry* geometry = ((ExPathD2D*)path)->m_geometry;

		//填充几何形
		m_dc->FillGeometry(geometry, br);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillRegion(const IExBrush* brush, const IExRegion* region)
	{
		CHECK_PARAM(brush);
		CHECK_PARAM(region);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//获取相关参数
		ID2D1Brush* br = (ID2D1Brush*)brush->GetContext(0);
		ID2D1Geometry* geometry = ((ExRegionD2D*)region)->m_geometry;

		//填充几何形
		m_dc->FillGeometry(geometry, br);
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::CalcTextSize(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float max_width, float max_height,
		DWORD effect_type, LPARAM effect_param, float* r_width, float* r_height)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//默认参数处理
		if (max_width <= 0) { max_width = D2D1::FloatMax(); }
		if (max_height <= 0) { max_height = D2D1::FloatMax(); }

		try
		{
			//生成文本布局对象
			ExAutoPtr<IDWriteTextLayout> layout = MakeTextLayout(font, text, text_length,
				ExRectF(0, 0, max_width, max_height), text_format
			);

			//获得测量结果
			DWRITE_TEXT_METRICS tm;
			throw_if_failed(layout->GetMetrics(&tm), L"获取测量结果失败");

			//返回结果
			if (r_width) { *r_width = tm.width; }
			if (r_height) { *r_height = tm.height; }
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExCanvasD2D::CalcTextWithIcon(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float left, float top, float right, float bottom,
		DWORD effect_type, LPARAM effect_param, const IExImage* icon_image, float icon_width, float icon_height,
		DWORD icon_pos, float split_size, ExRectF* r_text_rect, ExRectF* r_icon_rect, ExRectF* r_content_rect)
	{
		//获取整个范围
		ExRectF bounds_rect = ExRectF(left, top, right, bottom).Normalize();

		//获取图标真实尺寸
		uint32_t i_width = 0, i_height = 0;
		if (icon_image)
		{
			i_width = icon_image->GetWidth();
			i_height = icon_image->GetHeight();

			//如果没有边界,则按绘制边界计算
			if (icon_width <= 0) { icon_width = bounds_rect.Width(); }
			if (icon_height <= 0) { icon_height = bounds_rect.Height(); }

			//如果超界,则限定一下
			icon_width = fabs(icon_width);
			icon_height = fabs(icon_height);
			if (i_width > icon_width) { i_width = (uint32_t)icon_width; }
			if (i_height > icon_height) { i_height = (uint32_t)icon_height; }
		}
		else {
			if (icon_width < 0) { i_width = -icon_width; }
			if (icon_height < 0) { i_height = -icon_height; }
		}

		//图标是否为水平位置
		BOOL is_horz = icon_pos == ExIconPos::Left || icon_pos == ExIconPos::Right;

		//获取文本真实尺寸
		float t_width = 0, t_height = 0;
		if (font && text)
		{
			//计算得到文本尺寸
			CalcTextSize(font, text, text_length, text_format,
				bounds_rect.Width() - is_horz ? i_width + split_size : 0,
				bounds_rect.Height() - is_horz ? 0 : i_height + split_size,
				effect_type, effect_param, &t_width, &t_height);
		}

		//计算内容区域尺寸
		ExPointF content_size;
		if (is_horz) {
			content_size.x = i_width + split_size + t_width;
			content_size.y = max(i_height, t_height);
		}
		else {
			content_size.x = max(i_width, t_width);
			content_size.y = i_height + split_size + t_height;
		}

		//计算内容矩形
		ExRectF rect;

		if (text_format & ExTextFormat::Center) {
			rect.left = bounds_rect.left + (bounds_rect.Width() - content_size.x) / 2;
			rect.right = rect.left + content_size.x;
		}
		else if (text_format & ExTextFormat::Right) {
			rect.right = bounds_rect.right;
			rect.left = rect.right - content_size.x;
		}
		else {
			rect.left = bounds_rect.left;
			rect.right = rect.left + content_size.x;
		}

		if (text_format & ExTextFormat::Middle) {
			rect.top = bounds_rect.top + (bounds_rect.Height() - content_size.y) / 2;
			rect.bottom = rect.top + content_size.y;
		}
		else if (text_format & ExTextFormat::Bottom) {
			rect.bottom = bounds_rect.bottom;
			rect.top = rect.bottom - content_size.y;
		}
		else {
			rect.top = bounds_rect.top;
			rect.bottom = rect.top + content_size.y;
		}

		//计算文本区域
		if (r_text_rect) {

			ExRectF text_bounds;

			//确定文本边界矩形
			if (icon_pos == ExIconPos::Left) {
				text_bounds = ExRectF(rect.left + i_width + split_size, rect.top, rect.right, rect.bottom);
			}
			else if (icon_pos == ExIconPos::Right) {
				text_bounds = ExRectF(rect.left, rect.top, rect.right - i_width - split_size, rect.bottom);
			}
			else if (icon_pos == ExIconPos::Top) {
				text_bounds = ExRectF(rect.left, rect.top + i_height + split_size, rect.right, rect.bottom);
			}
			else if (icon_pos == ExIconPos::Bottom) {
				text_bounds = ExRectF(rect.left, rect.top, rect.right, rect.bottom - i_height - split_size);
			}
			else { text_bounds = rect; }

			//计算得到文本真实矩形

			if (text_format & ExTextFormat::Center) {
				r_text_rect->left = text_bounds.left + (text_bounds.Width() - t_width) / 2;
				r_text_rect->right = r_text_rect->left + t_width;
			}
			else if (text_format & ExTextFormat::Right) {
				r_text_rect->right = text_bounds.right;
				r_text_rect->left = r_text_rect->right - t_width;
			}
			else {
				r_text_rect->left = text_bounds.left;
				r_text_rect->right = r_text_rect->left + t_width;
			}

			if (text_format & ExTextFormat::Middle) {
				r_text_rect->top = text_bounds.top + (text_bounds.Height() - t_height) / 2;
				r_text_rect->bottom = r_text_rect->top + t_height;
			}
			else if (text_format & ExTextFormat::Bottom) {
				r_text_rect->bottom = text_bounds.bottom;
				r_text_rect->top = r_text_rect->bottom - t_height;
			}
			else {
				r_text_rect->top = text_bounds.top;
				r_text_rect->bottom = r_text_rect->top + t_height;
			}
		}

		//计算图标区域
		if (r_icon_rect) {

			ExRectF icon_bounds;

			//确定图标边界矩形
			if (icon_pos == ExIconPos::Left) {
				icon_bounds = ExRectF(rect.left, rect.top, rect.left + i_width, rect.bottom);
			}
			else if (icon_pos == ExIconPos::Right) {
				icon_bounds = ExRectF(rect.right - i_width, rect.top, rect.right, rect.bottom);
			}
			else if (icon_pos == ExIconPos::Top) {
				icon_bounds = ExRectF(rect.left, rect.top, rect.right, rect.top + i_height);
			}
			else if (icon_pos == ExIconPos::Bottom) {
				icon_bounds = ExRectF(rect.left, rect.bottom - i_height, rect.right, rect.bottom);
			}
			else { icon_bounds = rect; }

			//计算得到图标真实矩形

			if (text_format & ExTextFormat::Center) {
				r_icon_rect->left = icon_bounds.left + (icon_bounds.Width() - i_width) / 2;
				r_icon_rect->right = r_icon_rect->left + i_width;
			}
			else if (text_format & ExTextFormat::Right) {
				r_icon_rect->right = icon_bounds.right;
				r_icon_rect->left = r_icon_rect->right - i_width;
			}
			else {
				r_icon_rect->left = icon_bounds.left;
				r_icon_rect->right = r_icon_rect->left + i_width;
			}

			if (text_format & ExTextFormat::Middle) {
				r_icon_rect->top = icon_bounds.top + (icon_bounds.Height() - i_height) / 2;
				r_icon_rect->bottom = r_icon_rect->top + i_height;
			}
			else if (text_format & ExTextFormat::Bottom) {
				r_icon_rect->bottom = icon_bounds.bottom;
				r_icon_rect->top = r_icon_rect->bottom - i_height;
			}
			else {
				r_icon_rect->top = icon_bounds.top;
				r_icon_rect->bottom = r_icon_rect->top + i_height;
			}
		}

		//赋值内容区域
		if (r_content_rect) { *r_content_rect = rect; }
		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::StrokeText(const IExPen* pen, const IExFont* font,
		LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		bool clip = false;
		try
		{
			ExRectF rect = ExRectF(left, top, right, bottom).Normalize();

			ExAutoPtr<IDWriteTextLayout> layout = MakeTextLayout(
				font, text, text_length, rect, text_format
			);

			_offset_(false, rect);

			ExTextRenderD2D render(m_dc.Get(), pen, nullptr);

			clip = !query_flags(text_format, ExTextFormat::NoClip);
			if (clip) { m_dc->PushAxisAlignedClip(Rect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }

			throw_if_failed(
				layout->Draw(nullptr, &render, rect.left, rect.top),
				L"文本布局对象绘制失败"
			);

			if (clip) { m_dc->PopAxisAlignedClip(); }
			return S_OK;
		}
		catch_default({ if (clip) { m_dc->PopAxisAlignedClip(); } });
	}
	HRESULT EXOBJCALL ExCanvasD2D::FillText(const IExBrush* brush, const IExFont* font,
		LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		bool clip = false;
		try
		{
			ExRectF rect = ExRectF(left, top, right, bottom).Normalize();

			ExAutoPtr<IDWriteTextLayout> layout = MakeTextLayout(
				font, text, text_length, rect, text_format
			);

			_offset_(false, rect);

			ExTextRenderD2D render(m_dc.Get(), nullptr, brush);

			clip = !query_flags(text_format, ExTextFormat::NoClip);
			if (clip) { m_dc->PushAxisAlignedClip(Rect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }

			throw_if_failed(
				layout->Draw(nullptr, &render, rect.left, rect.top),
				L"文本布局对象绘制失败"
			);

			if (clip) { m_dc->PopAxisAlignedClip(); }
			return S_OK;
		}
		catch_default({ if (clip) { m_dc->PopAxisAlignedClip(); } });
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTextByColor(const IExFont* font, LPCWSTR text, uint32_t text_length,
		DWORD text_format, float left, float top, float right, float bottom, EXARGB text_color)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		bool clip = false;
		try
		{
			ExRectF rect = ExRectF(left, top, right, bottom).Normalize();

			//生成文本布局对象
			ExAutoPtr<IDWriteTextLayout> layout(
				MakeTextLayout(font, text, text_length, rect, text_format)
			);

			_offset_(false, rect);

			ExAutoPtr<ID2D1SolidColorBrush> brush;
			throw_if_failed(
				m_dc->CreateSolidColorBrush(Color(text_color), &brush),
				L"创建画刷失败"
			);

			clip = !query_flags(text_format, ExTextFormat::NoClip);
			if (clip) { m_dc->PushAxisAlignedClip(Rect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }

			m_dc->DrawTextLayout(
				D2D1::Point2F(rect.left, rect.top),
				layout, brush, D2D1_DRAW_TEXT_OPTIONS_CLIP
			);

			if (clip) { m_dc->PopAxisAlignedClip(); }
			return S_OK;
		}
		catch_default({ if (clip) { m_dc->PopAxisAlignedClip(); } });
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawText(const IExBrush* brush, const IExFont* font,
		LPCWSTR text, uint32_t text_length, DWORD text_format,
		float left, float top, float right, float bottom)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		bool clip = false;
		try
		{
			ExRectF rect = ExRectF(left, top, right, bottom).Normalize();

			//生成文本布局对象
			ExAutoPtr<IDWriteTextLayout> layout(
				MakeTextLayout(font, text, text_length, rect, text_format)
			);

			_offset_(false, rect);

			clip = !query_flags(text_format, ExTextFormat::NoClip);
			if (clip) { m_dc->PushAxisAlignedClip(Rect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }

			m_dc->DrawTextLayout(
				D2D1::Point2F(rect.left, rect.top),
				layout,
				(ID2D1Brush*)brush->GetContext(0),
				D2D1_DRAW_TEXT_OPTIONS_CLIP
			);

			if (clip) { m_dc->PopAxisAlignedClip(); }
			return S_OK;
		}
		catch_default({ if (clip) { m_dc->PopAxisAlignedClip(); } });
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawTextEx(const IExPen* stroke_pen, const IExBrush* fill_brush,
		const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
		float left, float top, float right, float bottom)
	{
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		bool clip = false;
		try
		{
			ExRectF rect = ExRectF(left, top, right, bottom).Normalize();

			ExAutoPtr<IDWriteTextLayout> layout = MakeTextLayout(
				font, text, text_length, rect, text_format
			);

			_offset_(false, rect);

			ExTextRenderD2D render(m_dc.Get(), stroke_pen, fill_brush);

			clip = !query_flags(text_format, ExTextFormat::NoClip);
			if (clip) { m_dc->PushAxisAlignedClip(Rect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }

			throw_if_failed(
				layout->Draw(nullptr, &render, rect.left, rect.top),
				L"文本布局对象绘制失败"
			);

			if (clip) { m_dc->PopAxisAlignedClip(); }
			return S_OK;
		}
		catch_default({ if (clip) { m_dc->PopAxisAlignedClip(); } });
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
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		bool clip = false;
		try
		{
			ExRectF rect = ExRectF(left, top, right, bottom).Normalize();

			ExAutoPtr<IDWriteTextLayout> layout = MakeTextLayout(
				font, text, text_length, rect, text_format
			);

			_offset_(false, rect);

			ExTextRenderD2D render(m_dc.Get(), stroke_pen, fill_brush);

			clip = !query_flags(text_format, ExTextFormat::NoClip);
			if (clip) { m_dc->PushAxisAlignedClip(Rect(rect), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE); }

			throw_if_failed(
				layout->Draw(nullptr, &render, rect.left, rect.top),
				L"文本布局对象绘制失败"
			);

			//绘制效果
			//这里可能可以和TextRender组合

			if (clip) { m_dc->PopAxisAlignedClip(); }
			return S_OK;
		}
		catch_default({ if (clip) { m_dc->PopAxisAlignedClip(); } });
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImage(const IExImage* image, float left, float top, EXCHANNEL alpha)
	{
		CHECK_PARAM(image);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top);

		//获得图像对象
		ID2D1Bitmap* bitmap = ((ExImageD2D*)image)->m_d2d_bitmap;

		//计算两个矩形
		D2D1_SIZE_F size = bitmap->GetSize();
		ExRectF dst_rect(left, top, size.width, size.height, true);
		ExRectF src_rect(0, 0, size.width, size.height);

		return OnDrawImage(bitmap, dst_rect, src_rect, ExImageMode::Default, alpha);
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImageRect(const IExImage* image, float left, float top,
		float right, float bottom, DWORD mode, EXCHANNEL alpha)
	{
		CHECK_PARAM(image);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获得图像对象
		ID2D1Bitmap* bitmap = ((ExImageD2D*)image)->m_d2d_bitmap;

		//计算两个矩形
		D2D1_SIZE_F size = bitmap->GetSize();
		ExRectF dst_rect = ExRectF(left, top, right, bottom).Normalize();
		ExRectF src_rect(0, 0, size.width, size.height);

		return OnDrawImage(bitmap, dst_rect, src_rect, mode, alpha);
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImagePart(const IExImage* image, float left, float top,
		float src_left, float src_top, float src_right, float src_bottom, EXCHANNEL alpha)
	{
		CHECK_PARAM(image);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top);

		//获得图像对象
		ID2D1Bitmap* bitmap = ((ExImageD2D*)image)->m_d2d_bitmap;

		//计算两个矩形
		ExRectF src_rect = ExRectF(src_left, src_top, src_right, src_bottom).Normalize();
		ExRectF dst_rect(left, top, src_rect.Width(), src_rect.Height());

		return OnDrawImage(bitmap, dst_rect, src_rect, ExImageMode::Default, alpha);
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawImagePartRect(const IExImage* image, float left, float top,
		float right, float bottom, float src_left, float src_top, float src_right, float src_bottom,
		DWORD mode, EXCHANNEL alpha)
	{
		CHECK_PARAM(image);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获得图像对象
		ID2D1Bitmap* bitmap = ((ExImageD2D*)image)->m_d2d_bitmap;

		//计算两个矩形
		ExRectF src_rect = ExRectF(src_left, src_top, src_right, src_bottom).Normalize();
		ExRectF dst_rect = ExRectF(left, top, right, bottom).Normalize();

		return OnDrawImage(bitmap, dst_rect, src_rect, mode, alpha);
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawGridsImage(const IExImage* image, float left, float top,
		float right, float bottom, ExGridsImageInfo* grids, EXCHANNEL alpha)
	{
		CHECK_PARAM(image);
		CHECK_PARAM(grids);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");

		//获得图像尺寸
		D2D1_SIZE_F size = ((ExImageD2D*)image)->m_d2d_bitmap->GetSize();

		return DrawGridsImagePart(image, left, top, right, bottom,
			0, 0, size.width, size.height,
			grids, alpha
		);
	}

	inline ExImageMode _gflag_to_imode(DWORD flags, int pos) {
		flags = (flags >> pos) & 0x0F;
		return flags == 0 ? ExImageMode::Default :
			flags & ExGridsImageMode::LeftNone ? (ExImageMode)-1 :
			flags & ExGridsImageMode::LeftTile ? ExImageMode::Tile :
			flags & ExGridsImageMode::LeftMirror ? ExImageMode::Mirror :
			ExImageMode::Default;
	}

	HRESULT EXOBJCALL ExCanvasD2D::DrawGridsImagePart(const IExImage* image, float left, float top,
		float right, float bottom, float src_left, float src_top, float src_right, float src_bottom, ExGridsImageInfo* grids, EXCHANNEL alpha)
	{
		CHECK_PARAM(image);
		CHECK_PARAM(grids);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");
		if (alpha == ALPHA_TRANSPARENT) { return S_FALSE; }

		//坐标偏移
		_offset_(false, left, top, right, bottom);

		//获得图像对象
		ID2D1Bitmap* bitmap = ((ExImageD2D*)image)->m_d2d_bitmap;

		//生成两个矩形,如果其中有的为空,则不绘制
		ExRectF dst_rect = ExRectF(left, top, right, bottom).Normalize();
		ExRectF src_rect = ExRectF(src_left, src_top, src_right, src_bottom).Normalize();
		if (dst_rect.IsEmpty()) { return S_FALSE; }
		if (src_rect.IsEmpty()) { return S_FALSE; }

		//生成两个中心块矩形[5]
		ExRectF src_rect_b5(
			src_rect.left + grids->left,
			src_rect.top + grids->top,
			src_rect.right - grids->right,
			src_rect.bottom - grids->bottom
		);
		ExRectF dst_rect_b5(
			dst_rect.left + grids->left,
			dst_rect.top + grids->top,
			dst_rect.right - grids->right,
			dst_rect.bottom - grids->bottom
		);

		//如果源中心块为空,则不绘制
		if (src_rect_b5.IsEmpty()) { return S_FALSE; }

		ExRectF src_block_rect{};
		ExRectF dst_block_rect{};
		ExImageMode mode = ExImageMode::Default;

		D2D1_INTERPOLATION_MODE interpolation_mode = D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC;

		//绘制[5]
		if ((grids->flags & ExGridsImageMode::CenterNone) == 0)
		{
			src_block_rect = src_rect_b5;
			dst_block_rect = dst_rect_b5;
			mode = _gflag_to_imode(grids->flags, 4);
			OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);
		}

		//不是垂直三宫图,绘制[4][6]
		if (!float_eq(src_rect_b5.Width(), src_rect.Width()))
		{
			//绘制[4]
			if ((grids->flags & ExGridsImageMode::LeftNone) == 0)
			{
				src_block_rect = ExRectF(src_rect.left, src_rect_b5.top, src_rect_b5.left, src_rect_b5.bottom);
				dst_block_rect = ExRectF(dst_rect.left, dst_rect_b5.top, dst_rect_b5.left, dst_rect_b5.bottom);
				mode = _gflag_to_imode(grids->flags, 0);
				OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);
			}

			//绘制[6]
			if ((grids->flags & ExGridsImageMode::RightNone) == 0)
			{
				src_block_rect = ExRectF(src_rect_b5.right, src_rect_b5.top, src_rect.right, src_rect_b5.bottom);
				dst_block_rect = ExRectF(dst_rect_b5.right, dst_rect_b5.top, dst_rect.right, dst_rect_b5.bottom);
				mode = _gflag_to_imode(grids->flags, 2);
				OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);
			}
		}

		//不是水平三宫图,绘制[2][8]
		if (!float_eq(src_rect_b5.Height(), src_rect.Height()))
		{
			//绘制[8]
			if ((grids->flags & ExGridsImageMode::TopNone) == 0)
			{
				src_block_rect = ExRectF(src_rect_b5.left, src_rect.top, src_rect_b5.right, src_rect_b5.top);
				dst_block_rect = ExRectF(dst_rect_b5.left, dst_rect.top, dst_rect_b5.right, dst_rect_b5.top);
				mode = _gflag_to_imode(grids->flags, 1);
				OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);
			}

			//绘制[2]
			if ((grids->flags & ExGridsImageMode::BottomNone) == 0)
			{
				src_block_rect = ExRectF(src_rect_b5.left, src_rect_b5.bottom, src_rect_b5.right, src_rect.bottom);
				dst_block_rect = ExRectF(dst_rect_b5.left, dst_rect_b5.bottom, dst_rect_b5.right, dst_rect.bottom);
				mode = _gflag_to_imode(grids->flags, 3);
				OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);
			}
		}

		//不是三宫图则绘制[1][3][7][9]
		if (!float_eq(src_rect_b5.Height(), src_rect.Height()) && !float_eq(src_rect_b5.Width(), src_rect.Width()))
		{
			mode = ExImageMode::Scale;

			//绘制[7]
			src_block_rect = ExRectF(src_rect.left, src_rect.top, src_rect_b5.left, src_rect_b5.top);
			dst_block_rect = ExRectF(dst_rect.left, dst_rect.top, dst_rect_b5.left, dst_rect_b5.top);
			OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);

			//绘制[9]
			src_block_rect = ExRectF(src_rect_b5.right, src_rect.top, src_rect.right, src_rect_b5.top);
			dst_block_rect = ExRectF(dst_rect_b5.right, dst_rect.top, dst_rect.right, dst_rect_b5.top);
			OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);

			//绘制[1]
			src_block_rect = ExRectF(src_rect.left, src_rect_b5.bottom, src_rect_b5.left, src_rect.bottom);
			dst_block_rect = ExRectF(dst_rect.left, dst_rect_b5.bottom, dst_rect_b5.left, dst_rect.bottom);
			OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);

			//绘制[3]
			src_block_rect = ExRectF(src_rect_b5.right, src_rect_b5.bottom, src_rect.right, src_rect.bottom);
			dst_block_rect = ExRectF(dst_rect_b5.right, dst_rect_b5.bottom, dst_rect.right, dst_rect.bottom);
			OnDrawImage(bitmap, dst_block_rect, src_block_rect, mode, alpha, interpolation_mode);
		}

		return S_OK;
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
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");
		if (!target_src) { return S_FALSE; }

		//获取源渲染目标
		ExCanvasTargetD2D* target = (ExCanvasTargetD2D*)target_src;
		handle_if_false(target != m_target, E_INVALIDARG, L"不允许绘制自身当前渲染目标");

		//偏移坐标
		_offset_(false, left, top, right, bottom);

		//生成源矩形
		D2D1_RECT_F src;
		if (src_rect) { src = Rect(src_rect->Normalize()); }
		else { src = D2D1::RectF(0, 0, target->width, target->height); }

		//绘制到dc上
		m_dc->DrawBitmap(target->bitmap, D2D1Rect(left, top, right, bottom),
			alpha / 255.0F, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src
		);

		return S_OK;
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawCanvas(IExCanvas* canvas_src, float left, float top, float right, float bottom,
		float src_left, float src_top, ExCanvasDrawMode mode, EXCHANNEL alpha)
	{
		CHECK_PARAM(canvas_src);
		handle_if_false(m_drawing, EE_NOREADY, L"画布尚未开始绘制");
		handle_if_false(canvas_src != this, E_INVALIDARG, L"不允许绘制自身");

		//获取渲染目标
		ExCanvasTargetD2D* target = ((ExCanvasD2D*)canvas_src)->m_target;
		ExAssert(target);

		bool temp_draw = false;
		try
		{
			//获取源图
			ID2D1Bitmap* src_bitmap = target->bitmap;

			//计算源、目标矩形
			D2D1_RECT_F dst = D2D1Rect(left, top, right, bottom);
			D2D1_RECT_F src = D2D1::RectF(src_left, src_top,
				src_left + fabs(dst.right - dst.left),
				src_top + fabs(dst.bottom - dst.top)
			);

			//开始绘制
			temp_draw = !canvas_src->IsDrawing();
			if (temp_draw) { throw_if_failed(canvas_src->BeginDraw(), L"开始绘制画布失败"); }

			//混合模式
			if (mode == ExCanvasDrawMode::Blend) {
				m_dc->DrawBitmap(
					src_bitmap, dst, alpha / 255.0F,
					D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src
				);
			}
			//覆盖模式
			else if (mode == ExCanvasDrawMode::Over) {

				//如果不是不透明的情况
				if (alpha != ALPHA_OPAQUE) {
					//先将目标区域清空
					m_dc->PushAxisAlignedClip(dst, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
					m_dc->Clear();
					m_dc->PopAxisAlignedClip();

					//然后按指定透明度绘制
					m_dc->DrawBitmap(
						src_bitmap, dst, alpha / 255.0F,
						D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src
					);
				}
				//否则直接拷贝位图
				else {

					//先刷新一下
					throw_if_failed(canvas_src->Flush(), L"刷新源画布失败");

					//直接拷贝
					D2D1_RECT_U src_u = D2D1::RectU(src.left, src.top, src.right, src.bottom);
					D2D1_POINT_2U dst_u = D2D1::Point2U(dst.left, dst.top);
					throw_if_failed(
						m_target->bitmap->CopyFromBitmap(&dst_u, src_bitmap, &src_u),
						L"拷贝位图失败"
					);
				}
			}
			else if (mode == ExCanvasDrawMode::OverBlend) {

				//FIXME:这里暂时使用AlphaBlend实现，后续可能使用蒙板实现？
				HDC src_dc = NULL;
				HDC dst_dc = NULL;
				bool ok = false;

				if (SUCCEEDED(canvas_src->GetDC(&src_dc))) {

					if (SUCCEEDED(GetDC(&dst_dc))) {
						BLENDFUNCTION bf = { 0,0,(EXCHANNEL)alpha, AC_SRC_OVER };

						ok = GdiAlphaBlend(
							dst_dc, dst.left, dst.top,
							dst.right - dst.left, dst.bottom - dst.top,
							src_dc, src.left, src.top,
							src.right - src.left, src.bottom - src.top,
							bf
						);

						this->ReleaseDC();
					}

					canvas_src->ReleaseDC();
				}

				throw_if_false(ok, E_FAIL, L"执行透明混合失败");
			}
			else { throw_ex(E_NOTIMPL, L"不支持的绘制模式"); }

			if (temp_draw) { throw_if_failed(canvas_src->EndDraw(), L"绘制中出现错误"); }
			return S_OK;
		}
		catch_default({
				if (temp_draw) { canvas_src->EndDraw(); }
			}
		);
	}
	HRESULT EXOBJCALL ExCanvasD2D::DrawEffect(const IExEffect* effect, float left, float top, LPARAM param)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExCanvasD2D::OnDrawImage(ID2D1Bitmap* bitmap, ExRectF& dst_rect, ExRectF& src_rect,
		DWORD mode, EXCHANNEL alpha, D2D1_INTERPOLATION_MODE interpolation)
	{
		//如果源矩形或目标矩形为空,则不绘制
		if (dst_rect.IsEmpty() || src_rect.IsEmpty() || alpha == ALPHA_TRANSPARENT) { return S_FALSE; }

		ExAutoPtr<ID2D1ImageBrush> image_brush;
		HRESULT hr = S_OK;

		//确定最终绘制的两个矩形,或者创建画刷
		switch (mode)
		{
		case ExImageMode::Scale: break;
		case ExImageMode::ScaleFill: {

			//计算缩放比
			float width_ps = dst_rect.Width() / src_rect.Width();
			float height_ps = dst_rect.Height() / src_rect.Height();

			//取大的一个
			float scale_ps = max(width_ps, height_ps);

			//求最终源宽高
			width_ps = dst_rect.Width() / scale_ps;
			height_ps = dst_rect.Height() / scale_ps;

			//求最终源图矩形
			src_rect = ExRectF(
				src_rect.left + (src_rect.Width() - width_ps) / 2,
				src_rect.top + (src_rect.Height() - height_ps) / 2,
				width_ps,
				height_ps,
				true
			);
		}break;
		case ExImageMode::ScaleCenter: {

			//计算缩放比
			float width_ps = dst_rect.Width() / src_rect.Width();
			float height_ps = dst_rect.Height() / src_rect.Height();

			//取小的一个,并约束其<=1
			float scale_ps = min(width_ps, height_ps);
			if (scale_ps > 1.0F) { scale_ps = 1.0F; }

			//求最终宽高
			width_ps = src_rect.Width() * scale_ps;
			height_ps = src_rect.Height() * scale_ps;

			//求最终目标矩形
			dst_rect = ExRectF(
				dst_rect.left + (dst_rect.Width() - width_ps) / 2,
				dst_rect.top + (dst_rect.Height() - height_ps) / 2,
				width_ps,
				height_ps,
				true
			);
		}break;
		case ExImageMode::Tile:
		case ExImageMode::Mirror:
		{
			//确定扩展模式
			D2D1_EXTEND_MODE extend_mode = (mode == ExImageMode::Tile) ? D2D1_EXTEND_MODE_WRAP : D2D1_EXTEND_MODE_MIRROR;

			//创建图像画刷
			hr = m_dc->CreateImageBrush(bitmap,
				D2D1::ImageBrushProperties(
					D2D1::RectF(src_rect.left, src_rect.top, src_rect.right, src_rect.bottom),
					extend_mode, extend_mode
				), &image_brush
			);

		}break;
		case (ExImageMode::Left | ExImageMode::Top): {
			dst_rect.right = dst_rect.left + src_rect.Width();
			dst_rect.bottom = dst_rect.top + src_rect.Height();
		}break;
		case (ExImageMode::HCenter | ExImageMode::Top): {
			float width = src_rect.Width();
			dst_rect.left += (dst_rect.Width() - width) / 2;
			dst_rect.right = dst_rect.left + width;

			dst_rect.bottom = dst_rect.top + src_rect.Height();
		}break;
		case (ExImageMode::Right | ExImageMode::Top): {
			dst_rect.left = dst_rect.right - src_rect.Width();
			dst_rect.bottom = dst_rect.top + src_rect.Height();
		}break;
		case (ExImageMode::Left | ExImageMode::VCenter): {
			dst_rect.right = dst_rect.left + src_rect.Width();

			float height = src_rect.Height();
			dst_rect.top += (dst_rect.Height() - height) / 2;
			dst_rect.bottom = dst_rect.top + height;
		}break;
		case (ExImageMode::HCenter | ExImageMode::VCenter): {
			float width = src_rect.Width();
			dst_rect.left += (dst_rect.Width() - width) / 2;
			dst_rect.right = dst_rect.left + width;

			float height = src_rect.Height();
			dst_rect.top += (dst_rect.Height() - height) / 2;
			dst_rect.bottom = dst_rect.top + height;
		}break;
		case (ExImageMode::Right | ExImageMode::VCenter): {
			dst_rect.left = dst_rect.right - src_rect.Width();

			float height = src_rect.Height();
			dst_rect.top += (dst_rect.Height() - height) / 2;
			dst_rect.bottom = dst_rect.top + height;
		}break;
		case (ExImageMode::Left | ExImageMode::Bottom): {
			dst_rect.right = dst_rect.left + src_rect.Width();
			dst_rect.top = dst_rect.bottom - src_rect.Height();
		}break;
		case (ExImageMode::HCenter | ExImageMode::Bottom): {
			float width = src_rect.Width();
			dst_rect.left += (dst_rect.Width() - width) / 2;
			dst_rect.right = dst_rect.left + width;

			dst_rect.top = dst_rect.bottom - src_rect.Height();
		}break;
		case (ExImageMode::Right | ExImageMode::Bottom): {
			dst_rect.left = dst_rect.right - src_rect.Width();
			dst_rect.top = dst_rect.bottom - src_rect.Height();
		}break;
		default: handle_ex(E_NOTIMPL, L"不支持的绘制模式");
		}

		//判断上面执行的没问题
		handle_if_failed(hr, L"绘制图像失败");

		//默认插值模式
		if (interpolation == D2D1_INTERPOLATION_MODE_FORCE_DWORD) {
			interpolation = m_interpolation_mode;
		}

		//如果有画刷则填充画刷,否则绘制图像
		if (image_brush) {
			image_brush->SetOpacity(alpha / 255.0F);
			image_brush->SetTransform(D2D1::Matrix3x2F::Translation(dst_rect.left, dst_rect.top));
			m_dc->FillRectangle(D2D1::RectF(dst_rect.left, dst_rect.top, dst_rect.right, dst_rect.bottom), image_brush);
		}
		else {
			m_dc->DrawBitmap(bitmap,
				D2D1::RectF(dst_rect.left, dst_rect.top, dst_rect.right, dst_rect.bottom),
				alpha / 255.0F, interpolation,
				D2D1::RectF(src_rect.left, src_rect.top, src_rect.right, src_rect.bottom)
			);
		}

		return S_OK;
	}
	IDWriteTextLayout* EXOBJCALL ExCanvasD2D::MakeTextLayout(const IExFont* font_, LPCWSTR text, uint32_t text_length,
		ExRectF rect, DWORD text_format)
	{
		//空串则不生成
		if (text == nullptr || text_length == 0) { return nullptr; }
		if (text_length == -1 && *text == L'\0') { return nullptr; }

		ExAutoPtr<IDWriteTextLayout> layout;
		HRESULT status = S_OK;

		//获得字体上下文
		const ExFontContextD2D* font = ((ExFontD2D*)font_)->m_context;

		//规格化矩形
		rect = rect.Normalize();

		//最终的文本对象
		std::wstring str;
		if (text_length == -1) { str = std::wstring(text); }
		else { str = std::wstring(text, text_length); }

		//对字符串预处理,记录下所有的前缀符&,并删除对应字符
		std::list<size_t> perfix_pos_list;
		if (text_format & ExTextFormat::Prefix) {

			//循环查找前缀符
			size_t pos = str.find(L'&');
			while (pos != std::wstring::npos) {

				//获取它的下一个字符
				wchar_t ch = str[pos + 1];

				//删除前缀符
				str.replace(pos, 1, 0, L'\0');

				//如果下个字符不是结束符或前缀符(&&的情况),则记录下来
				if (ch != L'\0' && ch != L'&') {
					perfix_pos_list.push_back(pos);
				}
				//否则跳过
				else { pos++; }

				//继续查找
				pos = str.find(L'&', pos);
			}
		}

		//获取最大尺寸
		float w = rect.Width(), h = rect.Height();
		if (w <= 0) { w = D2D1::FloatMax(); }
		if (h <= 0) { h = D2D1::FloatMax(); }

		//创建文本布局对象
		throw_if_failed(
			GetRender()->m_dwrite_factory->CreateTextLayout(
				str.c_str(), (uint32_t)str.size(), font->font,
				w, h, &layout
			), L"创建字体布局对象失败"
		);

		DWRITE_TEXT_RANGE range = { 0, (uint32_t)str.size() };

		//处理下换线
		if (font->info.IsUnderline()) {
			status = (layout->SetUnderline(true, range));
		}
		//如果存在前缀符,且不隐藏,则给对应字符添加下划线
		else if (perfix_pos_list.size() > 0 && (text_format & ExTextFormat::HidePrefix) == 0) {

			DWRITE_TEXT_RANGE r = { 0,1 };
			for (auto it : perfix_pos_list) {
				r.startPosition = (uint32_t)it;
				status = (layout->SetUnderline(true, r));
			}

		}

		//处理删除线
		if (font->info.IsStrikeout()) {
			status = (layout->SetStrikethrough(true, range));
		}

		//处理文本方向
		if (text_format & ExTextFormat::RtlReading) {
			status = (layout->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT));
		}

		if (text_format & ExTextFormat::Vertical) {
			status = (layout->SetFlowDirection(DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM));
		}

		//设置文本粗细
		status = layout->SetFontWeight(
			(DWRITE_FONT_WEIGHT)font->info.GetWeight(),
			range
		);

		//处理制表符
		if (text_format & ExTextFormat::TabStop) {
			//指定制表位宽度
			if (text_format & ExTextFormat::ExpandTabs)
				status = (layout->SetIncrementalTabStop((float)((text_format >> 8) & 0xFF)));
		}
		else {
			//不展开制表位
			status = (layout->SetIncrementalTabStop(0));
		}

		//是否剪辑
		if ((text_format & ExTextFormat::NoClip) == 0)
		{
			layout->SetMaxWidth(rect.Width());
			layout->SetMaxHeight(rect.Height());
		}

		//设置换行模式
		layout->SetWordWrapping(
			(text_format & ExTextFormat::SingleLine) ?
			DWRITE_WORD_WRAPPING_NO_WRAP :
			DWRITE_WORD_WRAPPING_WRAP
		);

		//设置裁剪模式
		if (text_format & (ExTextFormat::PathEllipsis | ExTextFormat::WordEllipsis | ExTextFormat::EndEllipsis)) {

			ExAutoPtr<IDWriteInlineObject> ellipsis;
			status = GetRender()->m_dwrite_factory->CreateEllipsisTrimmingSign(layout, &ellipsis);

			if (SUCCEEDED(status)) {

				DWRITE_TRIMMING trimming;

				//是否按单词裁剪
				trimming.granularity = (text_format & ExTextFormat::WordBreak) ?
					DWRITE_TRIMMING_GRANULARITY_WORD :
					DWRITE_TRIMMING_GRANULARITY_CHARACTER;

				//是否是按路径裁剪
				if (text_format & ExTextFormat::PathEllipsis) {
					trimming.delimiterCount = 2;
					trimming.delimiter = L'\\';
				}
				else {
					trimming.delimiterCount = 0;
					trimming.delimiter = 0;
				}

				//引用裁剪
				status = (layout->SetTrimming(&trimming, ellipsis));
			}
		}

		//如果不是测量模式,则设置对齐模式
		if ((text_format & DT_CALCRECT) == 0) {

			status = (layout->SetTextAlignment(
				text_format & ExTextFormat::Center ? DWRITE_TEXT_ALIGNMENT_CENTER :
				text_format & ExTextFormat::Right ? DWRITE_TEXT_ALIGNMENT_TRAILING :
				DWRITE_TEXT_ALIGNMENT_LEADING
			));

			status = (layout->SetParagraphAlignment(
				text_format & ExTextFormat::Middle ? DWRITE_PARAGRAPH_ALIGNMENT_CENTER :
				text_format & ExTextFormat::Bottom ? DWRITE_PARAGRAPH_ALIGNMENT_FAR :
				DWRITE_PARAGRAPH_ALIGNMENT_NEAR
			));
		}

		return layout.Detach();
	}
}


