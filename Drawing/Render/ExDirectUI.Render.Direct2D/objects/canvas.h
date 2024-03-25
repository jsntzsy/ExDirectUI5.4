/**
 * @file canvas.h
 * @brief 渲染器画布头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-25
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	struct ExCanvasTargetD2D : public IExCanvasTarget
	{
		ExCanvasD2D* owner;
		ExAutoPtr<ID2D1Bitmap1> bitmap;
		uint32_t width;
		uint32_t height;

		ExCanvasTargetD2D(ExCanvasD2D* owner, ID2D1Bitmap1* bitmap, uint32_t width, uint32_t height)
		{
			this->owner = owner;
			this->bitmap = bitmap;
			this->width = width;
			this->height = height;
		}

		virtual ~ExCanvasTargetD2D()
		{
			if (bitmap) { bitmap->Release(); }
		}
	};

	class ExCanvasD2D : public ExObjectImpl<IExCanvas>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExCanvas);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExCanvasD2D()");
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			default: return __super::GetContext(index);
			}
		}

	public:
		ExCanvasD2D(uint32_t width, uint32_t height);
		virtual ~ExCanvasD2D();

		EXMETHOD HRESULT EXOBJCALL CreateTarget(uint32_t width, uint32_t height, bool copy, IExCanvasTarget** r_target) override;
		EXMETHOD HRESULT EXOBJCALL DestroyTarget(IExCanvasTarget* target) override;
		EXMETHOD HRESULT EXOBJCALL SelectTarget(IExCanvasTarget* target, IExCanvasTarget** r_old_target = nullptr) override;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const override;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) override;

		EXMETHOD DWORD EXOBJCALL GetAntiAliasMode() const override;
		EXMETHOD HRESULT EXOBJCALL SetAntiAliasMode(DWORD mode) override;

		EXMETHOD HRESULT EXOBJCALL GetClipRect(ExRectF* r_clip) const override;
		EXMETHOD HRESULT EXOBJCALL GetClipRegion(IExRegion* r_clip_region) const override;
		EXMETHOD HRESULT EXOBJCALL SetClipRect(float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL SetClipRegion(IExRegion* clip_regioin) override;
		EXMETHOD HRESULT EXOBJCALL ResetClip() override;

		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrixElements* r_matrix) const override;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrixElements* matrix) override;

		EXMETHOD HRESULT EXOBJCALL GetDC(HDC* r_dc) override;
		EXMETHOD HRESULT EXOBJCALL ReleaseDC() override;
		EXMETHOD HRESULT EXOBJCALL Flush() override;
		EXMETHOD HRESULT EXOBJCALL RenderToImage(IExImage* r_target_image) override;

		EXMETHOD HRESULT EXOBJCALL BeginDraw() override;
		EXMETHOD HRESULT EXOBJCALL EndDraw() override;
		EXMETHOD bool EXOBJCALL IsDrawing() const override;

		EXMETHOD HRESULT EXOBJCALL Clear(EXARGB color = COLOR_TRANSPARENT) override;

		EXMETHOD HRESULT EXOBJCALL DrawLine(const IExPen* pen, float x1, float y1, float x2, float y2) override;
		EXMETHOD HRESULT EXOBJCALL DrawRect(const IExPen* pen, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL DrawEllipse(const IExPen* pen, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL DrawRoundRect(const IExPen* pen, float left, float top, float right, float bottom,
			float radius_x, float radius_y) override;
		EXMETHOD HRESULT EXOBJCALL DrawCustomRoundRect(const IExPen* pen, float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom) override;
		EXMETHOD HRESULT EXOBJCALL DrawPath(const IExPen* pen, const IExPath* path) override;

		EXMETHOD HRESULT EXOBJCALL FillRect(const IExBrush* brush, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL FillEllipse(const IExBrush* brush, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL FillRoundRect(const IExBrush* brush, float left, float top, float right, float bottom,
			float radius_x, float radius_y) override;
		EXMETHOD HRESULT EXOBJCALL FillCustomRoundRect(const IExBrush* brush, float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom) override;
		EXMETHOD HRESULT EXOBJCALL FillPath(const IExBrush* brush, const IExPath* path) override;
		EXMETHOD HRESULT EXOBJCALL FillRegion(const IExBrush* brush, const IExRegion* region) override;

		EXMETHOD HRESULT EXOBJCALL CalcTextSize(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float max_width, float max_height, DWORD effect_type, LPARAM effect_param, float* r_width, float* r_height) override;
		EXMETHOD HRESULT EXOBJCALL CalcTextWithIcon(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom, DWORD effect_type, LPARAM effect_param,
			const IExImage* icon_image, float icon_width, float icon_height, DWORD icon_pos, float split_size,
			ExRectF* r_text_rect, ExRectF* r_icon_rect, ExRectF* r_content_rect) override;

		//TODO:文本绘制函数可能需要调整，以支持ClearType等效果
		EXMETHOD HRESULT EXOBJCALL StrokeText(const IExPen* pen, const IExFont* font, LPCWSTR text, uint32_t text_length,
			DWORD text_format, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL FillText(const IExBrush* brush, const IExFont* font, LPCWSTR text, uint32_t text_length,
			DWORD text_format, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL DrawTextByColor(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom, EXARGB text_color) override;
		EXMETHOD HRESULT EXOBJCALL DrawText(const IExBrush* brush, const IExFont* font, LPCWSTR text, uint32_t text_length,
			DWORD text_format, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL DrawTextEx(const IExPen* stroke_pen, const IExBrush* fill_brush, const  IExFont* font,
			LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom) override;
		EXMETHOD HRESULT EXOBJCALL DrawTextEffect(const IExFont* font, LPCWSTR text, uint32_t text_length, DWORD text_format,
			float left, float top, float right, float bottom, DWORD effect_type, EXARGB effect_color, LPARAM effect_param) override;
		EXMETHOD HRESULT EXOBJCALL DrawTextAndEffect(const IExPen* stroke_pen, const IExBrush* fill_brush, const IExFont* font,
			LPCWSTR text, uint32_t text_length, DWORD text_format, float left, float top, float right, float bottom,
			DWORD effect_type = 0, EXARGB effect_color = COLOR_TRANSPARENT, LPARAM effect_param = 0) override;

		EXMETHOD HRESULT EXOBJCALL DrawImage(const IExImage* image, float left, float top, EXCHANNEL alpha = ALPHA_OPAQUE) override;
		EXMETHOD HRESULT EXOBJCALL DrawImageRect(const IExImage* image, float left, float top, float right, float bottom,
			ExImageMode mode = ExImageMode::Default, EXCHANNEL alpha = ALPHA_OPAQUE) override;
		EXMETHOD HRESULT EXOBJCALL DrawImagePart(const IExImage* image, float left, float top,
			float src_left, float src_top, float src_right, float src_bottom, EXCHANNEL alpha = ALPHA_OPAQUE) override;
		EXMETHOD HRESULT EXOBJCALL DrawImagePartRect(const IExImage* image, float left, float top, float right, float bottom,
			float src_left, float src_top, float src_right, float src_bottom,
			ExImageMode mode = ExImageMode::Default, EXCHANNEL alpha = ALPHA_OPAQUE) override;

		EXMETHOD HRESULT EXOBJCALL DrawGridsImage(const IExImage* image, float left, float top, float right, float bottom,
			ExGridsImageInfo* grids, EXCHANNEL alpha = ALPHA_OPAQUE) override;
		EXMETHOD HRESULT EXOBJCALL DrawGridsImagePart(const IExImage* image, float left, float top, float right, float bottom,
			float src_left, float src_top, float src_right, float src_bottom,
			ExGridsImageInfo* grids, EXCHANNEL alpha = ALPHA_OPAQUE) override;

		EXMETHOD HRESULT EXOBJCALL DrawShadow(const IExBrush* bkg_brush, float left, float top, float right, float bottom,
			float size, float radius_left_top = 0.0F, float radius_right_top = 0.0F,
			float radius_right_bottom = 0.0F, float radius_left_bottom = 0.0F,
			float offset_x = 0.0F, float offset_y = 0.0F) override;
		EXMETHOD HRESULT EXOBJCALL DrawShadowFromRegion(const IExBrush* bkg_brush, const IExRegion* region, float size,
			float offset_x = 0.0F, float offset_y = 0.0F) override;

		EXMETHOD HRESULT EXOBJCALL DrawTarget(const IExCanvasTarget* target_src, float left, float top,
			float right, float bottom, ExRectF* src_rect = nullptr, EXCHANNEL alpha = ALPHA_OPAQUE) override;
		EXMETHOD HRESULT EXOBJCALL DrawCanvas(IExCanvas* canvas_src, float left, float top, float right, float bottom,
			float src_left, float src_top, ExCanvasDrawMode mode, EXCHANNEL alpha = ALPHA_OPAQUE) override;

		EXMETHOD HRESULT EXOBJCALL DrawEffect(const IExEffect* effect, float left, float top, LPARAM param = NULL) override;

	private:
		HRESULT EXOBJCALL OnDrawImage(ID2D1Bitmap* bitmap, ExRectF& dst_rect, ExRectF& src_rect, ExImageMode mode,
			EXCHANNEL alpha, D2D1_INTERPOLATION_MODE interpolation = D2D1_INTERPOLATION_MODE_FORCE_DWORD);
		static IDWriteTextLayout* EXOBJCALL MakeTextLayout(const IExFont* font, LPCWSTR text, uint32_t text_length,
			ExRectF rect, DWORD text_format) MAYTHROW;

	private:
		ExCanvasTargetD2D* m_target;
		ExAutoPtr<ID2D1DeviceContext> m_dc;
		ExAutoPtr<ID2D1GdiInteropRenderTarget> m_gdi;
		ExAutoPtr<ID2D1Layer> m_layer;
		ExAutoPtr<ExRegionD2D> m_clip_region;

		bool m_drawing = false;
		DWORD m_antialias_mode = ExAntiAliasMode::Default;
		D2D1_INTERPOLATION_MODE m_interpolation_mode = D2D1_INTERPOLATION_MODE_LINEAR;

		friend class ExCanvasBrushD2D;
		friend class ExTextRenderD2D;
		friend class ExCompositionWindowDeviceD2D;
		friend class ExEffectD2D;
	};

}
