/**
 * @file render_api.cpp
 * @brief 渲染引擎API对接文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */

#include "stdafx.h"
#include "render_api.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
 //#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace ExDirectUI
{
	HRESULT __stdcall ExRenderAPI::D2D1CreateDevice(IDXGIDevice* dxgiDevice, const D2D1_CREATION_PROPERTIES* creationProperties, ID2D1Device** d2dDevice)
	{
		return ::D2D1CreateDevice(dxgiDevice, creationProperties, d2dDevice);
	}
	HRESULT __stdcall ExRenderAPI::D2D1CreateFactory(D2D1_FACTORY_TYPE factoryType, REFIID riid, const D2D1_FACTORY_OPTIONS* pFactoryOptions, void** ppIFactory)
	{
		return ::D2D1CreateFactory(factoryType, riid, pFactoryOptions, ppIFactory);
	}

	void __stdcall ExRenderAPI::D2D1MakeRotateMatrix(FLOAT angle, D2D1_POINT_2F center, D2D1_MATRIX_3X2_F* matrix)
	{
		::D2D1MakeRotateMatrix(angle, center, matrix);
	}

	void __stdcall ExRenderAPI::D2D1MakeSkewMatrix(FLOAT angleX, FLOAT angleY, D2D1_POINT_2F center, D2D1_MATRIX_3X2_F* matrix)
	{
		::D2D1MakeSkewMatrix(angleX, angleY, center, matrix);
	}

	BOOL __stdcall ExRenderAPI::D2D1IsMatrixInvertible(const D2D1_MATRIX_3X2_F* matrix)
	{
		return ::D2D1IsMatrixInvertible(matrix);
	}

	BOOL __stdcall ExRenderAPI::D2D1InvertMatrix(D2D1_MATRIX_3X2_F* matrix)
	{
		return ::D2D1InvertMatrix(matrix);
	}

	HRESULT __stdcall ExRenderAPI::D3D11CreateDevice(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext)
	{
		return ::D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
	}

	HRESULT __stdcall ExRenderAPI::DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID iid, void** dcompositionDevice)
	{
		if (!_DCompositionCreateDevice2) return E_NOTIMPL;
		return _DCompositionCreateDevice2(renderingDevice, iid, dcompositionDevice);
	}

	BOOL ExRenderAPI::DCompositionIsSupport()
	{
		return _DCompositionCreateDevice2 != nullptr;
	}

	HRESULT __stdcall ExRenderAPI::DWriteCreateFactory(DWRITE_FACTORY_TYPE factoryType, REFIID iid, IUnknown** factory)
	{
		return ::DWriteCreateFactory(factoryType, iid, factory);
	}

	int __stdcall ExRenderAPI::GetUserDefaultLocaleName(LPWSTR lpLocaleName, int cchLocaleName)
	{
		return ::GetUserDefaultLocaleName(lpLocaleName, cchLocaleName);
	}

	HRESULT ExRenderAPI::OnInit()
	{
		s_module_dcomp = LoadLibraryW(L"dcomp.dll");
		if (s_module_dcomp)
		{
			_DCompositionCreateDevice2 = (PFN_DCompositionCreateDevice2)
				GetProcAddress(s_module_dcomp, "DCompositionCreateDevice2");
		}
		
		return S_OK;
	}
	void ExRenderAPI::OnUnInit()
	{
		SAFE_FREE(s_module_dcomp, FreeLibrary);
	}

	HMODULE  ExRenderAPI::s_module_dcomp;
	ExRenderAPI::PFN_DCompositionCreateDevice2  ExRenderAPI::_DCompositionCreateDevice2;
}


