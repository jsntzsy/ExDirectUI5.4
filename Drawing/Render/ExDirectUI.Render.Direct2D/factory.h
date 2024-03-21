/**
 * @file factory.h
 * @brief 渲染器工厂头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

#define ATOM_EXDIRECTUI_RENDER_DIRECT2D				0x7DE8373E		// 原子号: exdirectui.render.direct2d

namespace ExDirectUI
{
	class ExRenderD2D : public ExObjectBaseImpl<IExRender>,
		public ExLazySingleton<ExRenderD2D>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExModule);
		EX_INTERFACE_ENTRY(IExRender);
		EX_END_INTERFACE_MAP();

	public:

		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return L"ExRenderD2D: core: Direct2D/DWrite/ImageCodec";
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_d2d_factory.Get();
			case 1: return m_dwrite_factory.Get();
			case 2: return m_wic_factory.Get();
			case 3: return m_d2d_device.Get();
			case 4: return m_d2d_dc.Get();

			default: return __super::GetContext(index);
			}
		}

	public:
		ExRenderD2D(IExModuleUtils* utils);
		virtual ~ExRenderD2D();

		EXMETHOD EXATOM EXOBJCALL GetId() const override { return MODULE_INFO.id; }
		EXMETHOD uint16_t EXOBJCALL GetType() const override { return MODULE_INFO.type; }
		EXMETHOD HRESULT EXOBJCALL GetInfo(ExModuleInfo* r_info) const override
		{
			CHECK_PARAM(r_info);
			*r_info = MODULE_INFO;
			return S_OK;
		}
		EXMETHOD HRESULT EXOBJCALL Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result) override;
		EXMETHOD HRESULT EXOBJCALL OnModuleUnLoad() override;
	public:

		EXMETHOD bool EXOBJCALL IsSupportComposition() const override;

		EXMETHOD HRESULT EXOBJCALL CreateImage(uint32_t width, uint32_t height, IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromFile(LPCWSTR file, IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromMemory(const byte_t* data, size_t size, IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromBitsData(uint32_t width, uint32_t height,
			const EXBITSDATA bits, uint32_t stride, IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromImageData(const byte_t* data, IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromHBITMAP(HBITMAP bitmap, HPALETTE palette,
			bool pre_alpha, IExImage** r_image) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromHICON(HICON icon, IExImage** r_image) override;

		EXMETHOD HRESULT EXOBJCALL LoadFontFile(const byte_t* data, size_t size, EXATOM* r_atom) override;
		EXMETHOD HRESULT EXOBJCALL UnLoadFontFile(EXATOM font_atom) override;
		EXMETHOD HRESULT EXOBJCALL CreateFont(IExFont** r_font) override;
		EXMETHOD HRESULT EXOBJCALL CreateFontFromName(LPCWSTR name, uint32_t size, DWORD style, EXATOM file_atom, IExFont** r_font) override;
		EXMETHOD HRESULT EXOBJCALL CreateFontFromInfo(const ExFontInfo* font_info, IExFont** r_font) override;

		EXMETHOD HRESULT EXOBJCALL CreatePath(IExPath** r_path) override;

		EXMETHOD HRESULT EXOBJCALL CreateInfinityRegion(IExRegion** r_region) override;
		EXMETHOD HRESULT EXOBJCALL CreateEmptyRegion(IExRegion** r_region) override;
		EXMETHOD HRESULT EXOBJCALL CreateRectRegion(float left, float top, float right, float bottom, IExRegion** r_region) override;
		EXMETHOD HRESULT EXOBJCALL CreateRoundRectRegion(float left, float top, float right, float bottom,
			float radius, IExRegion** r_region) override;
		EXMETHOD HRESULT EXOBJCALL CreateCustomRoundRectRegion(float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom,
			IExRegion** r_region) override;
		EXMETHOD HRESULT EXOBJCALL CreateEllipseRegion(float left, float top, float right, float bottom, IExRegion** r_region) override;
		EXMETHOD HRESULT EXOBJCALL CreatePathRegion(const IExPath* path, const ExMatrixElements* tranform, IExRegion** r_region) override;

		EXMETHOD HRESULT EXOBJCALL CreatePenFromColor(EXARGB color, float width, IExPen** r_pen) override;
		EXMETHOD HRESULT EXOBJCALL CreatePenFromBrush(const IExBrush* brush, float width, IExPen** r_pen) override;

		EXMETHOD HRESULT EXOBJCALL CreateSolidBrush(EXARGB color, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateLinearBrush(float x_begin, float y_begin, float x_end, float y_end,
			EXARGB color_begin, EXARGB color_end, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateLinearBrushEx(float x_begin, float y_begin, float x_end, float y_end,
			ExGradientPoint* gradient_points, uint32_t count, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateRadialBrush(float left, float top, float right, float bottom,
			EXARGB color_inside, EXARGB color_outside, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateRadialBrushEx(float left, float top, float right, float bottom,
			ExGradientPoint* gradient_points, uint32_t count, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateImageBrush(const IExImage* image, float left, float top,
			float right, float bottom, const ExRectF* src, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateCanvasBrush(const IExCanvas* canvas, const ExRectF* dst,
			DWORD extend_mode, EXCHANNEL alpha, IExBrush** r_brush) override;
		EXMETHOD HRESULT EXOBJCALL CreateCanvasBrushFromTarget(const IExCanvasTarget* target, const ExRectF* dst,
			DWORD extend_mode, EXCHANNEL alpha, IExBrush** r_brush) override;

		EXMETHOD HRESULT EXOBJCALL RegisterEffect(const ExEffectInfo* effect_info) override;
		EXMETHOD HRESULT EXOBJCALL UnRegisterEffect(EXATOM effect_id) override;
		EXMETHOD HRESULT EXOBJCALL CreateEffect(EXATOM effect_id, LPARAM lparam, IExEffect** r_effect) override;

		EXMETHOD HRESULT EXOBJCALL CreateWindowDevice(HWND window, ExDeviceType type, IExDevice** r_device) override;
		EXMETHOD HRESULT EXOBJCALL CreateBitmapDevice(uint32_t width, uint32_t height, IExDevice** r_device) override;
		EXMETHOD HRESULT EXOBJCALL CreateCanvas(uint32_t width, uint32_t height, IExCanvas** r_canvas) override;

	private:
		IExModuleUtils* m_utils;

		ExAutoPtr<ID2D1Factory> m_d2d_factory;
		ExAutoPtr<IDWriteFactory> m_dwrite_factory;
		ExAutoPtr<IWICImagingFactory> m_wic_factory;
		ExAutoPtr<ID2D1Device> m_d2d_device;
		ExAutoPtr<ID2D1DeviceContext> m_d2d_dc;

		friend inline IExModuleUtils* GetUtils();
		friend class ExImageD2D;
		friend class ExFontPoolD2D;
		friend class ExSolidBrushD2D;
		friend class ExLinearBrushD2D;
		friend class ExRadialBrushD2D;
		friend class ExImageBrushD2D;
		friend class ExCanvasBrushD2D;
		friend class ExPathD2D;
		friend class ExRegionD2D;
		friend class ExCompositionWindowDeviceD2D;
		friend class ExCanvasD2D;
		friend class ExEffectD2D;

		inline static const ExModuleInfo MODULE_INFO =
		{
			ATOM_EXDIRECTUI_RENDER_DIRECT2D,
			ExModuleType::Render,
			ExModuleFlags::None,
			0,
			L"ExDirectUI.Render.Direct2D",
			L"1.0.0.0",
			L"EternalZSY@ExDirectUI",
			L"ExDirectUI渲染器(内核：Direct2D/DWrite/ImageCodec，效率高且支持混合窗口，需要Win7SP1以上系统)"
		};
	};

	inline IExModuleUtils* GetUtils() { return ExRenderD2D::Instance()->m_utils; }
	inline ExRenderD2D* GetRender() { return ExRenderD2D::Instance(); }

}
