/**
 * @file factory.cpp
 * @brief 渲染器工厂实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */

#include "stdafx.h"
#include "factory.h"
#include "render_api.h"

#include "objects/image.h"
#include "objects/font.h"
#include "objects/path.h"
#include "objects/region.h"
#include "objects/pen.h"
#include "objects/solid_brush.h"
#include "objects/linear_brush.h"
#include "objects/radial_brush.h"
#include "objects/image_brush.h"
#include "objects/device.h"
#include "objects/canvas.h"

namespace ExDirectUI
{
	ExRenderD2D::ExRenderD2D(IExModuleUtils* utils) : m_utils(utils)
	{
		// 初始化COM
		throw_if_failed(CoInitialize(nullptr), L"COM初始化失败");

		//初始化渲染器API
		throw_if_failed(ExRenderAPI::OnInit(), L"渲染器API初始化失败");

		// 创建文本绘制厂类
		throw_if_failed(
			ExRenderAPI::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory), (IUnknown**)&m_dwrite_factory
			), L"创建DWrite工厂失败");

		// 创建WIC厂类
		throw_if_failed(
			CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
				IID_IWICImagingFactory, (LPVOID*)&m_wic_factory
			), L"创建WIC工厂失败");

		// 创建D3D设备
		ExAutoPtr<ID3D11Device> d3d_device;
		throw_if_failed(
			ExRenderAPI::D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
				nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
				&d3d_device, nullptr, nullptr
			), L"创建D3D设备失败");

		// 通过D3D设备拿到DXGI设备
		ExAutoPtr<IDXGIDevice> dxgi_device;
		throw_if_failed(
			d3d_device->QueryInterface(__uuidof(dxgi_device), (LPVOID*)&dxgi_device),
			L"获取DXGI设备失败"
		);

		// 通过DXGI设备创建D2D设备
		throw_if_failed(
			ExRenderAPI::D2D1CreateDevice(dxgi_device, nullptr, &m_d2d_device),
			L"创建D2D设备失败"
		);

		// 创建D2D设备上下文
		throw_if_failed(
			m_d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_d2d_dc),
			L"创建D2D设备上下文失败"
		);

		// 获取D2D工厂
		m_d2d_dc->GetFactory(&m_d2d_factory);

		//初始化配置
		m_d2d_dc->SetUnitMode(D2D1_UNIT_MODE_PIXELS);

		//创建字体池
		//因为此时构造函数还未结束，LazySingleton::Instance返回nullptr,所以需要手动传this，以下同理
		ExFontPoolD2D::GetInstance(this);

		//ExEffectD2D::RegisterEffects(this);

	}
	ExRenderD2D::~ExRenderD2D()
	{
		ExRenderAPI::OnUnInit();
		CoUninitialize();
	}
	HRESULT EXOBJCALL ExRenderD2D::Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result)
	{
		return E_NOTIMPL;
	}
	HRESULT EXOBJCALL ExRenderD2D::OnModuleUnLoad()
	{
		ExFontPoolD2D::ClearInstance(true);

		m_d2d_dc.Release();
		m_d2d_device.Release();
		m_d2d_factory.Release();
		m_wic_factory.Release();
		m_dwrite_factory.Release();

		return S_OK;
	}
	bool EXOBJCALL ExRenderD2D::IsSupportComposition() const
	{
		return ExRenderAPI::DCompositionIsSupport();
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImage(uint32_t width, uint32_t height, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(width, height);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageFromFile(LPCWSTR file, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(file);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageFromMemory(const byte_t* data, size_t size, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(data, size);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageFromBitsData(uint32_t width, uint32_t height, const EXBITSDATA bits, uint32_t stride, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(width, height, bits, stride);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageFromImageData(const byte_t* data, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(data);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageFromHBITMAP(HBITMAP bitmap, HPALETTE palette, bool pre_alpha, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(bitmap, palette, pre_alpha);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageFromHICON(HICON icon, IExImage** r_image)
	{
		try
		{
			ExAutoPtr<ExImageD2D> image = NEW ExImageD2D(icon);
			return image->QueryInterface(r_image);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::LoadFontFile(const byte_t* data, size_t size, EXATOM* r_atom)
	{
		try
		{
			auto context = ExFontPoolD2D::Instance()->LoadFontFile(data, size);
			*r_atom = context->atom;
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::UnLoadFontFile(EXATOM font_atom)
	{
		try
		{
			ExFontPoolD2D::Instance()->UnLoadFontFile(font_atom);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateFont(IExFont** r_font)
	{
		try
		{
			ExAutoPtr<ExFontD2D> font = NEW ExFontD2D();
			return font->QueryInterface(r_font);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateFontFromName(LPCWSTR name, uint32_t size, DWORD style, EXATOM file_atom, IExFont** r_font)
	{
		try
		{
			ExAutoPtr<ExFontD2D> font = NEW ExFontD2D(name, size, style, file_atom);
			return font->QueryInterface(r_font);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateFontFromInfo(const ExFontInfo* font_info, IExFont** r_font)
	{
		try
		{
			ExAutoPtr<ExFontD2D> font = NEW ExFontD2D(font_info);
			return font->QueryInterface(r_font);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePath(IExPath** r_path)
	{
		try
		{
			ExAutoPtr<ExPathD2D> path = NEW ExPathD2D();
			return path->QueryInterface(r_path);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateInfinityRegion(IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(true);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateEmptyRegion(IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(false);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRectRegion(float left, float top, float right, float bottom, IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(left, top, right, bottom,false);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRoundRectRegion(float left, float top, float right, float bottom, float radius, IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(left, top, right, bottom, radius, radius, radius, radius);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCustomRoundRectRegion(float left, float top, float right, float bottom, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom, IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(left, top, right, bottom, 
				radius_left_top, radius_right_top, radius_right_bottom, radius_left_bottom);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateEllipseRegion(float left, float top, float right, float bottom, IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(left, top, right, bottom);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePathRegion(const IExPath* path, const ExMatrixElements* tranform, IExRegion** r_region)
	{
		try
		{
			ExAutoPtr<ExRegionD2D> region = NEW ExRegionD2D(path, tranform);
			return region->QueryInterface(r_region);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePenFromColor(EXARGB color, float width, IExPen** r_pen)
	{
		try
		{
			if (color == COLOR_UNDEFINE) { color = COLOR_TRANSPARENT; }
			ExAutoPtr<ExPenD2D> pen = NEW ExPenD2D(color, width);
			return pen->QueryInterface(r_pen);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePenFromBrush(const IExBrush* brush, float width, IExPen** r_pen)
	{
		try
		{
			ExAutoPtr<ExPenD2D> pen = NEW ExPenD2D(brush, width);
			return pen->QueryInterface(r_pen);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateSolidBrush(EXARGB color, IExBrush** r_brush)
	{
		try
		{
			ExAutoPtr<ExSolidBrushD2D> brush = NEW ExSolidBrushD2D(color);
			return brush->QueryInterface(r_brush);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateLinearBrush(float begin_x, float begin_y, float end_x, float end_y, EXARGB color_begin, EXARGB color_end, IExBrush** r_brush)
	{
		try
		{
			ExAutoPtr<ExLinearBrushD2D> brush = NEW ExLinearBrushD2D(begin_x, begin_y, end_x, end_y, color_begin, color_end);
			return brush->QueryInterface(r_brush);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateLinearBrushEx(float begin_x, float begin_y, float end_x, float end_y, ExGradientPoint* gradient_points, uint32_t count, IExBrush** r_brush)
	{
		CHECK_PARAM(gradient_points && count >= 2);

		try
		{
			ExAutoPtr<ExLinearBrushD2D> brush = NEW ExLinearBrushD2D(begin_x, begin_y, end_x, end_y, gradient_points, count);
			return brush->QueryInterface(r_brush);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRadialBrush(float left, float top, float right, float bottom, EXARGB color_inside, EXARGB color_outside, IExBrush** r_brush)
	{
		try
		{
			ExAutoPtr<ExRadialBrushD2D> brush = NEW ExRadialBrushD2D(left, top, right, bottom, color_inside, color_outside);
			return brush->QueryInterface(r_brush);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRadialBrushEx(float left, float top, float right, float bottom, ExGradientPoint* gradient_points, uint32_t count, IExBrush** r_brush)
	{
		CHECK_PARAM(gradient_points && count >= 2);

		try
		{
			ExAutoPtr<ExRadialBrushD2D> brush = NEW ExRadialBrushD2D(left, top, right, bottom, gradient_points, count);
			return brush->QueryInterface(r_brush);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageBrush(const IExImage* image, const ExRectF* src,
		ExBrushExtendMode extend_mode, EXCHANNEL opacity, IExBrush** r_brush)
	{
		CHECK_PARAM(image);

		try
		{
			ExAutoPtr<ExImageBrushD2D> brush = NEW ExImageBrushD2D(image, src, extend_mode, opacity);
			return brush->QueryInterface(r_brush);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCanvasBrush(const IExCanvas* canvas, const ExRectF* dst, DWORD extend_mode, 
		EXCHANNEL alpha, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCanvasBrushFromTarget(const IExCanvasTarget* target, const ExRectF* dst, DWORD extend_mode, EXCHANNEL alpha, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::RegisterEffect(const ExEffectInfo* effect_info)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::UnRegisterEffect(EXATOM effect_id)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateEffect(EXATOM effect_id, LPARAM lparam, IExEffect** r_effect)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateWindowDevice(HWND window, ExDeviceType type, IExDevice** r_device)
	{
		CHECK_PARAM(window);

		try
		{
			*r_device = _ExDevice_CreateWindowDevice(window, type == ExDeviceType::CompositionWindow);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateBitmapDevice(uint32_t width, uint32_t height, IExDevice** r_device)
	{
		try
		{
			ExAutoPtr<ExBitmapDeviceD2D> device = NEW ExBitmapDeviceD2D(width, height);
			return device->QueryInterface(r_device);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCanvas(uint32_t width, uint32_t height, IExCanvas** r_canvas)
	{
		try
		{
			ExAutoPtr<ExCanvasD2D> canvas = NEW ExCanvasD2D(width, height);
			return canvas->QueryInterface(r_canvas);
		}
		catch_default({});
	}
}


