/**
 * @file factory.h
 * @brief 渲染器工厂头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-18
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	EXINTERFACE("8B77AA10-57F3-4676-A8DA-549221E426C2") IExRender : public IExModule
	{
		EXMETHOD bool EXOBJCALL IsSupportComposition() const PURE;

		EXMETHOD HRESULT EXOBJCALL CreateImage(uint32_t width, uint32_t height, IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromFile(LPCWSTR file, IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromMemory(const byte_t* data, size_t size, IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromBitsData(uint32_t width, uint32_t height,
			const EXBITSDATA bits,uint32_t stride, IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromImageData(const byte_t* data, IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromHBITMAP(HBITMAP bitmap, HPALETTE palette,
			bool pre_alpha, IExImage** r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageFromHICON(HICON icon, IExImage** r_image) PURE;

		EXMETHOD HRESULT EXOBJCALL LoadFontFile(const byte_t* data, size_t size, EXATOM* r_atom) PURE;
		EXMETHOD HRESULT EXOBJCALL UnLoadFontFile(EXATOM font_atom) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateFont(IExFont** r_font) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateFontFromName(LPCWSTR name, uint32_t size, DWORD style, EXATOM file_atom, IExFont** r_font) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateFontFromInfo(const ExFontInfo* font_info, IExFont** r_font) PURE;

		EXMETHOD HRESULT EXOBJCALL CreatePath(IExPath** r_path) PURE;

		EXMETHOD HRESULT EXOBJCALL CreateInfinityRegion(IExRegion** r_region) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateEmptyRegion(IExRegion** r_region) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateRectRegion(float left, float top, float right, float bottom, IExRegion** r_region) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateRoundRectRegion(float left, float top, float right, float bottom,
			float radius, IExRegion** r_region) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateCustomRoundRectRegion(float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom,
			IExRegion** r_region) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateEllipseRegion(float left, float top, float right, float bottom, IExRegion** r_region) PURE;
		EXMETHOD HRESULT EXOBJCALL CreatePathRegion(const IExPath* path, const ExMatrixElements* tranform, IExRegion** r_region) PURE;

		EXMETHOD HRESULT EXOBJCALL CreatePen(EXARGB color, float width, IExPen** r_pen) PURE;
		EXMETHOD HRESULT EXOBJCALL CreatePenFromBrush(const IExBrush* brush, float width, IExPen** r_pen) PURE;

		EXMETHOD HRESULT EXOBJCALL CreateSolidBrush(EXARGB color, IExSolidBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateLinearBrush(float x_begin, float y_begin, float x_end, float y_end,
			EXARGB color_begin, EXARGB color_end, IExLinearBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateLinearBrushEx(float x_begin, float y_begin, float x_end, float y_end,
			ExGradientPoint* gradient_points, uint32_t count, IExLinearBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateRadialBrush(float left, float top, float right, float bottom,
			EXARGB color_inside, EXARGB color_outside, IExRadialBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateRadialBrushEx(float left, float top, float right, float bottom,
			ExGradientPoint* gradient_points, uint32_t count, IExRadialBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateImageBrush(const IExImage* image, const ExRectF* src,
			ExBrushExtendMode extend_mode, EXCHANNEL opacity, IExImageBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateCanvasBrush(const IExCanvas* canvas, const ExRectF* dst,
			DWORD extend_mode, EXCHANNEL alpha, IExCanvasBrush** r_brush) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateCanvasBrushFromTarget(const IExCanvasTarget* target, const ExRectF* dst,
			DWORD extend_mode, EXCHANNEL alpha, IExCanvasBrush** r_brush) PURE;

		EXMETHOD HRESULT EXOBJCALL RegisterEffect(const ExEffectInfo* effect_info) PURE;
		EXMETHOD HRESULT EXOBJCALL UnRegisterEffect(EXATOM effect_id) PURE;
		EXMETHOD bool EXOBJCALL IsSupportEffect(EXATOM effect_id) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetEffectInfo(EXATOM effect_id, ExEffectInfo* const r_effect_info) const PURE;
		EXMETHOD HRESULT EXOBJCALL CreateEffect(EXATOM effect_id, LPARAM lparam, IExEffect** r_effect) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateEffectByName(LPCWSTR effect_name, LPARAM lparam, IExEffect** r_effect) PURE;

		EXMETHOD HRESULT EXOBJCALL CreateWindowDevice(HWND window, ExDeviceType type, IExDevice** r_device) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateBitmapDevice(uint32_t width, uint32_t height, IExDevice** r_device) PURE;
		EXMETHOD HRESULT EXOBJCALL CreateCanvas(uint32_t width, uint32_t height, IExCanvas** r_canvas) PURE;

	};

	///////////////////
	
}
