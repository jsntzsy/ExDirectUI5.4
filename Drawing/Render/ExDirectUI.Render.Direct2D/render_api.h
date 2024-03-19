/**
 * @file render_api.h
 * @brief 渲染引擎API接口头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	const GUID _CLSID_D2D1GaussianBlur = { 0x1feb6d69, 0x2fe6, 0x4ac9, 0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5 };
	const GUID _CLSID_D2D1HueRotation = { 0x0f4458ec, 0x4b32, 0x491b, 0x9e, 0x85, 0xbd, 0x73, 0xf4, 0x4d, 0x3e, 0xb6 };
	const GUID _CLSID_D2D13DTransform = { 0xe8467b04, 0xec61, 0x4b8a, 0xb5, 0xde, 0xd4, 0xd7, 0x3d, 0xeb, 0xea, 0x5a };
	const GUID _CLSID_D2D13DPerspectiveTransform = { 0xC2844D0B, 0x3D86, 0x46e7, 0x85, 0xBA, 0x52, 0x6C, 0x92, 0x40, 0xF3, 0xFB };

	class ExRenderAPI
	{
	public: //D2D

		static HRESULT WINAPI
			D2D1CreateDevice(
				_In_ IDXGIDevice* dxgiDevice,
				_In_opt_ CONST D2D1_CREATION_PROPERTIES* creationProperties,
				_Outptr_ ID2D1Device** d2dDevice
			);
		static  HRESULT WINAPI
			D2D1CreateFactory(
				_In_ D2D1_FACTORY_TYPE factoryType,
				_In_ REFIID riid,
				_In_opt_ CONST D2D1_FACTORY_OPTIONS* pFactoryOptions,
				_Out_ void** ppIFactory
			);

		static void WINAPI
			D2D1MakeRotateMatrix(
				_In_ FLOAT angle,
				_In_ D2D1_POINT_2F center,
				_Out_ D2D1_MATRIX_3X2_F* matrix
			);
		static void WINAPI
			D2D1MakeSkewMatrix(
				_In_ FLOAT angleX,
				_In_ FLOAT angleY,
				_In_ D2D1_POINT_2F center,
				_Out_ D2D1_MATRIX_3X2_F* matrix
			);
		static BOOL WINAPI
			D2D1IsMatrixInvertible(
				_In_ CONST D2D1_MATRIX_3X2_F* matrix
			);
		static BOOL WINAPI
			D2D1InvertMatrix(
				_Inout_ D2D1_MATRIX_3X2_F* matrix
			);


	public: //D3D

		static HRESULT WINAPI D3D11CreateDevice(
			_In_opt_ IDXGIAdapter* pAdapter,
			D3D_DRIVER_TYPE DriverType,
			HMODULE Software,
			UINT Flags,
			_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
			UINT FeatureLevels,
			UINT SDKVersion,
			_COM_Outptr_opt_ ID3D11Device** ppDevice,
			_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
			_COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext);

	public: //DCOM

		static HRESULT __stdcall DCompositionCreateDevice2(
			_In_opt_ IUnknown* renderingDevice,
			_In_ REFIID iid,
			_Outptr_ void** dcompositionDevice
		);

		static BOOL DCompositionIsSupport();


	public: //DWRITE

		static HRESULT WINAPI DWriteCreateFactory(
			_In_ DWRITE_FACTORY_TYPE factoryType,
			_In_ REFIID iid,
			_COM_Outptr_ IUnknown** factory
		);

		static int
			WINAPI
			GetUserDefaultLocaleName(
				_Out_writes_(cchLocaleName) LPWSTR lpLocaleName,
				_In_ int cchLocaleName
			);


	private:
		static HRESULT OnInit();
		static void OnUnInit();

		static HMODULE s_module_dcomp;
		
		typedef HRESULT(WINAPI* PFN_DCompositionCreateDevice2)(IUnknown* renderingDevice, REFIID iid, void** dcompositionDevice);
		static PFN_DCompositionCreateDevice2 _DCompositionCreateDevice2;

		friend class ExRenderD2D;
	};

}
