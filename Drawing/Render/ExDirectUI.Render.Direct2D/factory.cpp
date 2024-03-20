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

		//ExFontPoolD2D::Init(this);

		//ExEffectD2D::RegisterEffects(this);

	}
	ExRenderD2D::~ExRenderD2D()
	{
	}
	HRESULT EXOBJCALL ExRenderD2D::Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result)
	{
		return E_NOTIMPL;
	}
	HRESULT EXOBJCALL ExRenderD2D::OnModuleUnLoad()
	{

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
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::UnLoadFontFile(EXATOM font_atom)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateFont(IExFont** r_font)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateFontFromName(LPCWSTR name, uint32_t size, DWORD style, EXATOM file_atom, IExFont** r_font)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateFontFromInfo(const ExFontInfo* font_info, IExFont** r_font)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePath(IExPath** r_path)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateInfinityRegion(IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateEmptyRegion(IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRectRegion(float left, float top, float right, float bottom, IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRoundRectRegion(float left, float top, float right, float bottom, float radius_horz, float radius_vert, IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCustomRoundRectRegion(float left, float top, float right, float bottom, float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom, IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateEllipseRegion(float left, float top, float right, float bottom, IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePathRegion(const IExPath* path, const ExMatrixElements* tranform, IExRegion** r_region)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePenFromColor(EXARGB color, float width, IExPen** r_pen)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreatePenFromBrush(const IExBrush* brush, float width, IExPen** r_pen)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateSolidBrush(EXARGB color, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateLinearBrush(float x_begin, float y_begin, float x_end, float y_end, EXARGB color_begin, EXARGB color_end, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateLinearBrushEx(float x_begin, float y_begin, float x_end, float y_end, ExGradientPoint* gradient_points, uint32_t count, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRadialBrush(float left, float top, float right, float bottom, EXARGB color_inside, EXARGB color_outside, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateRadialBrushEx(float left, float top, float right, float bottom, ExGradientPoint* gradient_points, uint32_t count, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateImageBrush(const IExImage* image, float left, float top, float right, float bottom, const ExRectF* src, IExBrush** r_brush)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCanvasBrush(const IExCanvas* canvas, const ExRectF* dst, DWORD extend_mode, EXCHANNEL alpha, IExBrush** r_brush)
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
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateBitmapDevice(uint32_t width, uint32_t height, IExDevice** r_device)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
	HRESULT EXOBJCALL ExRenderD2D::CreateCanvas(uint32_t width, uint32_t height, IExCanvas** r_canvas)
	{
		handle_ex(E_NOTIMPL, L"尚未实现");
	}
}


