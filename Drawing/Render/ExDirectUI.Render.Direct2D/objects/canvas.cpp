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
			m_clip_region = NEW ExRegionD2D(clip.left, clip.top, clip.right, clip.bottom, true);

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
				ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(clip_regioin);

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
		handle_if_false(m_gdi, EE_NOREADY, L"GDI兼容对象不存在");

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
		handle_ex(E_NOTIMPL, L"尚未实现");
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
		if (font->info.IsUnderline()){
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


