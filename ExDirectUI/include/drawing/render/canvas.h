/**
 * @file canvas.h
 * @brief 渲染器画布头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	enum ExImageMode
	{



		EX_IMAGE_MODE_DEFAULT,
	};

	struct ExGridsImageInfo
	{

	};

	enum ExCanvasDrawMode
	{

	};

	/////////////////

	interface IExCanvasTarget
	{
	};

	EXINTERFACE("7BBF8CF9-3257-4A3A-8409-26A9EC418221") IExCanvas : public IExRenderObject
	{
		EXMETHOD HRESULT EXOBJCALL CreateTarget(IExCanvasTarget * *r_target) PURE;
		EXMETHOD HRESULT EXOBJCALL DestroyTarget(IExCanvasTarget* target) PURE;
		EXMETHOD HRESULT EXOBJCALL SelectTarget(IExCanvasTarget* target, IExCanvasTarget** r_old_target = nullptr) PURE;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const PURE;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) PURE;

		EXMETHOD DWORD EXOBJCALL GetAntiAliasMode() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetAntiAliasMode(DWORD mode) PURE;

		EXMETHOD HRESULT EXOBJCALL GetClipRect(ExRectF* r_clip) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetClipRegion(IExRegion* r_clip_region) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetClipRect(float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL SetClipRegion(IExRegion* clip_regioin) PURE;
		EXMETHOD HRESULT EXOBJCALL ResetClip() PURE;

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrixElements* r_matrix) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrixElements* matrix) PURE;

		EXMETHOD HRESULT EXOBJCALL GetDC(HDC* r_dc) PURE;
		EXMETHOD HRESULT EXOBJCALL ReleaseDC() PURE;
		EXMETHOD HRESULT EXOBJCALL Flush() PURE;
		EXMETHOD HRESULT EXOBJCALL RenderToImage(IExImage* r_target_image) PURE;

		EXMETHOD HRESULT EXOBJCALL BeginDraw() PURE;
		EXMETHOD HRESULT EXOBJCALL EndDraw() PURE;
		EXMETHOD bool EXOBJCALL IsDrawing() const PURE;

		EXMETHOD HRESULT EXOBJCALL Clear(EXARGB color = COLOR_TRANSPARENT) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawLine(const IExPen* pen, float x1, float y1, float x2, float y2) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawRect(const IExPen* pen, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawEllipse(const IExPen* pen, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawRoundRect(const IExPen* pen, float left, float top, float right, float bottom,
			float radius_x, float radius_y) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawCustomRoundRect(const IExPen* pen, float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawPath(const IExPen* pen, const IExPath* path) PURE;

		EXMETHOD HRESULT EXOBJCALL FillRect(const IExBrush* brush, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL FillEllipse(const IExBrush* brush, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL FillRoundRect(const IExBrush* brush, float left, float top, float right, float bottom,
			float radius_x, float radius_y) PURE;
		EXMETHOD HRESULT EXOBJCALL FillCustomRoundRect(const IExBrush* brush, float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL FillPath(const IExBrush* brush, const IExPath* path) PURE;
		EXMETHOD HRESULT EXOBJCALL FillRegion(const IExBrush* brush, const IExRegion* region) PURE;

		EXMETHOD HRESULT EXOBJCALL CalcTextSize(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float max_width, float max_height, DWORD effect_type, LPARAM effect_param, float* r_width, float* r_height) PURE;
		EXMETHOD HRESULT EXOBJCALL CalcTextWithIcon(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom, DWORD effect_type, LPARAM effect_param,
			const IExImage* icon_image, float icon_width, float icon_height, DWORD icon_pos, float split_size,
			ExRectF* r_text_rect, ExRectF* r_icon_rect, ExRectF* r_content_rect) PURE;

		EXMETHOD HRESULT EXOBJCALL StrokeText(const IExPen* pen, const IExFont* font, LPCWSTR text, uint32_t text_length,
			DWORD text_format, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL FillText(const IExBrush* brush, const IExFont* font, LPCWSTR text, uint32_t text_length,
			DWORD text_format, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawTextByColor(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom, EXARGB text_color) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawText(const IExBrush* brush, const IExFont* font, LPCWSTR text, uint32_t text_length,
			DWORD text_format, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawTextEx(const IExPen* stroke_pen, const IExBrush* fill_brush, const  IExFont* font,
			LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawTextEffect(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom, DWORD effect_type, EXARGB effect_color, LPARAM effect_param) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawTextAndEffect(const IExPen* stroke_pen, const IExBrush* fill_brush, const IExFont* font,
			LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom,
			DWORD effect_type = 0, EXARGB effect_color = COLOR_TRANSPARENT, LPARAM effect_param = 0) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawImage(const IExImage* image, float left, float top, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawImageRect(const IExImage* image, float left, float top, float right, float bottom,
			ExImageMode mode = EX_IMAGE_MODE_DEFAULT, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawImagePart(const IExImage* image, float left, float top,
			float src_left, float src_top, float src_right, float src_bottom, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawImagePartRect(const IExImage* image, float left, float top, float right, float bottom,
			float src_left, float src_top, float src_right, float src_bottom,
			ExImageMode mode = EX_IMAGE_MODE_DEFAULT, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawGridsImage(const IExImage* image, float left, float top, float right, float bottom,
			ExGridsImageInfo* grids, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawGridsImagePart(const IExImage* image, float left, float top, float right, float bottom,
			float src_left, float src_top, float src_right, float src_bottom,
			ExGridsImageInfo* grids, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawShadow(const IExBrush* bkg_brush,float left, float top, float right, float bottom,
			 float size, float radius_left_top = 0.0F, float radius_right_top = 0.0F,
			float radius_right_bottom = 0.0F, float radius_left_bottom = 0.0F,
			float offset_x = 0.0F, float offset_y = 0.0F) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawShadowFromRegion(const IExBrush* bkg_brush, const IExRegion* region, float size,
			float offset_x = 0.0F, float offset_y = 0.0F) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawTarget(const IExCanvasTarget* target_src, float left, float top,
			float right, float bottom, ExRectF* src_rect = nullptr, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawCanvas(IExCanvas* canvas_src, float left, float top, float right, float bottom,
			float src_left, float src_top, ExCanvasDrawMode mode, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawEffect(const IExEffect* effect, float left, float top, LPARAM param = NULL) PURE;


	}

}
