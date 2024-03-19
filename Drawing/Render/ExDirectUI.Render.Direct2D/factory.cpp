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

		return S_OK;
	}
}


