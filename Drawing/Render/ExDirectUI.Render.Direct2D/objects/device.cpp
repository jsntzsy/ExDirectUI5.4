/**
 * @file device.cpp
 * @brief 渲染器设备对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-23
 * @copyright
 */

#include "stdafx.h"
#include "objects/device.h"
#include "render_api.h"

namespace ExDirectUI
{
	ExBitmapDeviceD2D::ExBitmapDeviceD2D(uint32_t width, uint32_t height)
	{
		throw_if_failed(ExMemDCCreate(width, height, &m_mdc), L"创建内存DC失败");
	}
	ExBitmapDeviceD2D::~ExBitmapDeviceD2D()
	{
		if (m_mdc.dc) { ExMemDCDestroy(&m_mdc); }
	}
	ExDeviceType EXOBJCALL ExBitmapDeviceD2D::GetDeviceType() const
	{
		return ExDeviceType::Bitmap;
	}
	HANDLE EXOBJCALL ExBitmapDeviceD2D::GetObject() const
	{
		return m_mdc.bitmap;
	}
	HRESULT EXOBJCALL ExBitmapDeviceD2D::GetSize(uint32_t* r_width, uint32_t* r_height) const
	{
		if (r_width) { *r_width = m_mdc.width; }
		if (r_height) { *r_height = m_mdc.height; }
		return S_OK;
	}
	HRESULT EXOBJCALL ExBitmapDeviceD2D::Resize(uint32_t width, uint32_t height)
	{
		return ExMemDCResize(&m_mdc, width, height);
	}
	HRESULT EXOBJCALL ExBitmapDeviceD2D::Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc)
	{
		bool temp_draw = false;
		HDC src_dc = NULL;

		try
		{
			//获取AlphaBlend参数
			if (alpha == -1) { alpha = ALPHA_OPAQUE; }
			else if (alpha < 0) { alpha = ALPHA_TRANSPARENT; }
			else if (alpha > ALPHA_OPAQUE) { alpha = ALPHA_OPAQUE; }
			BLENDFUNCTION bf = { 0,0,(BYTE)alpha,AC_SRC_OVER };

			//需要保持画布处于绘制状态
			if (!canvas_src->IsDrawing()) {
				throw_if_failed(canvas_src->BeginDraw(), L"开始绘制画布失败");
				temp_draw = true;
			}

			//获取源DC
			throw_if_failed(canvas_src->GetDC(&src_dc), L"获取画布DC失败");

			//确定最终的更新区域
			ExRect rect;
			if (update_rect) { rect = update_rect->Normalize(); }
			else { rect = ExRect(0, 0, m_mdc.width, m_mdc.height); }

			//如果未指定目标DC,则更新到本位图DC上,否则更新到目标DC上
			if (!dest_dc) { dest_dc = m_mdc.dc; }

			//更新
			throw_if_false(GdiAlphaBlend(
				dest_dc, rect.left, rect.top, rect.Width(), rect.Height(),
				src_dc, rect.left, rect.top, rect.Width(), rect.Height(),
				bf), E_FAIL, L"位图设备更新失败"
			);

			//释放
			canvas_src->ReleaseDC();
			if (temp_draw) { canvas_src->EndDraw(); }

			return S_OK;
		}
		catch_default({
			if (src_dc) { canvas_src->ReleaseDC(); }
			if (temp_draw) { canvas_src->EndDraw(); }
			});
	}

	//////////////////////////////////////////

	ExWindowDeviceD2D::ExWindowDeviceD2D(HWND window)
	{
		//获取窗口尺寸
		ExRect window_rect;
		throw_if_false(GetWindowRect(window, &window_rect), E_FAIL, L"获取窗口尺寸失败");
		m_size = { window_rect.Width(), window_rect.Height() };
		if (m_size.cx <= 0) { m_size.cx = 1; }
		if (m_size.cy <= 0) { m_size.cy = 1; }

		m_window = window;
		m_bf = { 0,0,ALPHA_OPAQUE,AC_SRC_ALPHA };

		m_ulwi.cbSize = sizeof(m_ulwi);
		m_ulwi.hdcDst = NULL;
		m_ulwi.pptDst = nullptr;
		m_ulwi.psize = &m_size;
		m_ulwi.hdcSrc = NULL;
		m_ulwi.pptSrc = &m_src_point;
		m_ulwi.prcDirty = NULL;
		m_ulwi.pblend = &m_bf;
		m_ulwi.dwFlags = ULW_ALPHA;
		m_ulwi.crKey = 0;

	}

	ExWindowDeviceD2D::~ExWindowDeviceD2D()
	{
	}

	ExDeviceType EXOBJCALL ExWindowDeviceD2D::GetDeviceType() const
	{
		return ExDeviceType::Window;
	}

	HANDLE EXOBJCALL ExWindowDeviceD2D::GetObject() const
	{
		return m_window;
	}

	HRESULT EXOBJCALL ExWindowDeviceD2D::GetSize(uint32_t* r_width, uint32_t* r_height) const
	{
		if (r_width) { *r_width = m_size.cx; }
		if (r_height) { *r_height = m_size.cy; }
		return S_OK;
	}

	HRESULT EXOBJCALL ExWindowDeviceD2D::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0) { width = 1; }
		if (height == 0) { height = 1; }
		m_size = { (LONG)width, (LONG)height };
		return S_OK;
	}

	HRESULT EXOBJCALL ExWindowDeviceD2D::Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc)
	{
		bool temp_draw = false;
		HDC src_dc = NULL;
		HDC dst_dc = dest_dc;

		try
		{
			//需要保持画布处于绘制状态
			if (!canvas_src->IsDrawing()) {
				throw_if_failed(canvas_src->BeginDraw(), L"开始绘制画布失败");
				temp_draw = true;
			}

			//alpha为-1时，使用BitBlt更新
			if (alpha == -1) {

				//如果未传入目标DC，则更新到本设备窗口上
				if (!dst_dc) { dst_dc = GetDC(m_window); }

				//确定最终的更新区域
				ExRect rect;
				if (update_rect) { rect = update_rect->Normalize(); }
				else { rect = ExRect(0, 0, m_size.cx, m_size.cy); }

				//更新到窗口上
				throw_if_false(
					BitBlt(
						dst_dc, rect.left, rect.top, rect.Width(), rect.Height(),
						src_dc, rect.left, rect.top, SRCCOPY
					), E_FAIL, L"窗口设备更新失败"
				);
			}
			else {

				//获取窗口位置
				ExRect window_rect{};
				GetWindowRect(m_window, &window_rect);

				//更新相关变量
				m_ulwi.pptDst = (POINT*)&window_rect;
				m_ulwi.hdcSrc = src_dc;
				m_ulwi.prcDirty = update_rect;

				//设置透明度
				if (alpha > ALPHA_OPAQUE) { m_bf.SourceConstantAlpha = ALPHA_OPAQUE; }
				else if (alpha < 0) { m_bf.SourceConstantAlpha = ALPHA_TRANSPARENT; }
				else { m_bf.SourceConstantAlpha = (BYTE)alpha; }

				//更新
				throw_if_false(
					ExUpdateLayeredWindow(m_window, &m_ulwi),
					E_FAIL, L"窗口设备更新失败"
				);
			}

			//释放
			if (dst_dc && dst_dc != dest_dc) { ReleaseDC(m_window, dst_dc); }
			canvas_src->ReleaseDC();
			if (temp_draw) { canvas_src->EndDraw(); }

			return S_OK;
		}
		catch_default({
				if (dst_dc && dst_dc != dest_dc) { ReleaseDC(m_window, dst_dc); }
				if (src_dc) { canvas_src->ReleaseDC(); }
				if (temp_draw) { canvas_src->EndDraw(); }
			}
		);
	}

	/////////////////////////////

	ExCompositionWindowDeviceD2D::ExCompositionWindowDeviceD2D(HWND window)
	{
		//获取窗口尺寸
		ExRect window_rect{};
		throw_if_false(GetWindowRect(window, &window_rect), E_FAIL, L"获取窗口尺寸失败");
		m_size = { window_rect.Width(), window_rect.Height() };
		if (m_size.cx <= 0) { m_size.cx = 1; }
		if (m_size.cy <= 0) { m_size.cy = 1; }

		//创建混合设备对象
		throw_if_failed(
			ExRenderAPI::DCompositionCreateDevice2(
				GetRender()->m_d2d_device,
				__uuidof(m_device),
				(void**)&m_device
			), L"创建混合设备对象失败"
		);

		//通过窗口句柄创建目标对象
		throw_if_failed(m_device->CreateTargetForHwnd(window, true, &m_target), L"创建窗口目标对象失败");

		//创建视觉对象
		throw_if_failed(m_device->CreateVisual(&m_visual), L"创建视觉对象失败");

		//创建表面对象
		throw_if_failed(m_device->CreateSurface(m_size.cx, m_size.cy,
			DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED,
			&m_surface
		), L"创建表面对象失败");

		//将表面设置到视觉对象内容里
		throw_if_failed(m_visual->SetContent(m_surface), L"设置视觉内容失败");

		//将视觉对象添加到目标对象
		throw_if_failed(m_target->SetRoot(m_visual), L"设置窗口目标根对象失败");

		m_window = window;
	}

	ExCompositionWindowDeviceD2D::~ExCompositionWindowDeviceD2D()
	{
	}

	ExDeviceType EXOBJCALL ExCompositionWindowDeviceD2D::GetDeviceType() const
	{
		return ExDeviceType::CompositionWindow;
	}

	HANDLE EXOBJCALL ExCompositionWindowDeviceD2D::GetObject() const
	{
		return m_window;
	}

	HRESULT EXOBJCALL ExCompositionWindowDeviceD2D::GetSize(uint32_t* r_width, uint32_t* r_height) const
	{
		if (r_width) { *r_width = m_size.cx; }
		if (r_height) { *r_height = m_size.cy; }
		return S_OK;
	}

	HRESULT EXOBJCALL ExCompositionWindowDeviceD2D::Resize(uint32_t width, uint32_t height)
	{
		//参数规格化
		if (width <= 0) { width = 1; }
		if (height <= 0) { height = 1; }

		//创建新表面对象
		ExAutoPtr<IDCompositionSurface> surface;
		handle_if_failed(
			m_device->CreateSurface(width, height,
				DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED,
				&surface
			), L"创建新表面对象失败"
		);

		//将表面设置到视觉对象中
		handle_if_failed(m_visual->SetContent(surface), L"设置视觉内容失败");

		//更新成员变量
		m_size = { (LONG)width, (LONG)height };
		m_surface = surface;
		return S_OK;
	}

	HRESULT EXOBJCALL ExCompositionWindowDeviceD2D::Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc)
	{
		//确定透明度
		float a;
		if (alpha == -1 || alpha > ALPHA_OPAQUE) { a = 1.0F; }
		else if (alpha < ALPHA_TRANSPARENT) { a = 0.0F; }
		else (a = ((float)alpha) / 255.0F);

		ExAutoPtr<ID2D1DeviceContext> dst_dc;
		ExPoint offset = {};

		try
		{
			////ExCanvasTargetD2D* pTarget = ((ExCanvasD2D*)pCanvasSrc)->m_pTarget;

			////FIXME:CompositionDevice要绘制得全部绘制，否则会有闪烁和撕裂，暂不清楚原因
			////计算真实剪辑更新区域
			//ExRect rcClip;
			////if (rcUpdate) rcClip = rcUpdate->Normalize();
			///*else */rcClip = ExRect(0, 0, pTarget->m_nWidth, pTarget->m_nHeight);

			////开始绘制
			//if (!_ST(m_surface->BeginDraw(&rcClip, __uuidof(dcDst), (LPVOID*)&dcDst, &ptOffset)))
			//	_ES_THROW(ES_OLE);

			////设置剪辑区
			//rcClip.Offset(ptOffset.x, ptOffset.y);
			//dcDst->PushAxisAlignedClip(
			//	D2D1::RectF(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom),
			//	D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
			//);

			////清除原由内容
			//dcDst->Clear();

			////设置变换到偏移点
			//dcDst->SetTransform(D2D1::Matrix3x2F::Translation(ptOffset.x, ptOffset.y));

			////绘制画布目标
			//dcDst->DrawBitmap(pTarget->m_pBitmap, NULL, alpha);

			////还原剪辑区
			//dcDst->PopAxisAlignedClip();

			////结束绘制
			//if (!_ST(m_surface->EndDraw())) _ES_THROW(ES_OLE);

			//SAFE_RELEASE(dcDst);

			////把变更提交
			//if (!_ST(m_device->Commit()))_ES_THROW(ES_OLE);

		}
		catch_default({

			}
		);
	}


	IExDevice* EXCALL _ExDevice_CreateWindowDevice(HWND window, bool composition)
	{
		//如果考虑使用混合模式,那么先检查系统是否支持,并且窗口是否带有WS_EX_NOREDIRECTIONBITMAP风格
		if (composition) { composition = ExRenderAPI::DCompositionIsSupport(); }
		if (composition) {
			composition = (GetWindowLong(window, GWL_EXSTYLE) & WS_EX_NOREDIRECTIONBITMAP) != 0;
		}

		if (composition) {
			//如果创建并初始化成功了,则直接使用
			try_ignore({ return NEW ExCompositionWindowDeviceD2D(window); });
		}

		//否则使用普通的窗口设备
		return NEW ExWindowDeviceD2D(window);
	}


}


