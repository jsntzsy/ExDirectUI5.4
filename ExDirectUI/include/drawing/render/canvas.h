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
	/// 抗锯齿模式
	EXENUM(ExAntiAliasMode)
	{
		None = 0x00000000,																				///< 抗锯齿模式：无
		Figure = 0x00000001,																			///< 抗锯齿模式：图形
		FigureHighQuality = 0x00000003,																	///< 抗锯齿模式：图形高质量
		Image = 0x00000010,																				///< 抗锯齿模式：图像
		ImageHighQuality = 0x00000030,																	///< 抗锯齿模式：图像高质量
		Text = 0x00000100,																				///< 抗锯齿模式：文本
		TextHighQuality = 0x00000300,																	///< 抗锯齿模式：文本高质量
		All = ExAntiAliasMode::Figure | ExAntiAliasMode::Image | ExAntiAliasMode::Text,					///< 抗锯齿模式：全部
		AllHighQuality = ExAntiAliasMode::FigureHighQuality | ExAntiAliasMode::ImageHighQuality |		///< 抗锯齿模式：全部高质量
			ExAntiAliasMode::TextHighQuality,

		Default = ExAntiAliasMode::All,																	///< 抗锯齿模式：默认
	};

	/// 文本格式
	EXENUM(ExTextFormat)
	{
		Left = 0x00000000,							///< 文本格式：左对齐
		Top = 0x00000000,							///< 文本格式：顶对齐
		Center = 0x00000001,						///< 文本格式：水平居中
		Right = 0x00000002,							///< 文本格式：右对齐
		Middle = 0x00000004,						///< 文本格式：垂直居中
		Bottom = 0x00000008,						///< 文本格式：底对齐
		WordBreak = 0x00000010,						///< 文本格式：按字断开
		SingleLine = 0x00000020,					///< 文本格式：单行
		ExpandTabs = 0x00000040,					///< 文本格式：展开制表符 
		TabStop = 0x00000080,						///< 文本格式：自定制表符宽度 (高字节表示制表符宽度)
		NoClip = 0x00000100,						///< 文本格式：忽略裁剪
		ExternalLeading = 0x00000200,				///< 文本格式：
		//占用一个 0x00000400 的位
		Prefix = 0x00000800,						///< 文本格式：转义前缀符 (&转义为下划线)
		PathEllipsis = 0x00004000,					///< 文本格式：路径省略 (路径中间省略)
		EndEllipsis = 0x00008000,					///< 文本格式：末尾省略 (末尾变为省略号)
		RtlReading = 0x00020000,					///< 文本格式：从右往左
		WordEllipsis = 0x00040000,					///< 文本格式：单词忽略 (按单词省略)
		NoFullWidthCharBreak = 0x00080000,			///< 文本格式：
		HidePrefix = 0x00100000,					///< 文本格式：隐藏前缀符 (被转义的&不显示下划线)

		Vertical = 0x00200000						///< 文本格式：垂直显示
	};

	
	EXENUM(ExImageMode)
	{
		Scale = 0,
		ScaleFill = 1,
		ScaleCenter = 2,
		Tile = 3,
		Mirror = 4,

		LeftTop = 11,
		CenterTop = 12,
		RightTop = 13,
		LeftMiddle = 14,
		CenterMiddle = 15,
		RightMiddle = 16,
		LeftBottom = 17,
		CenterBottom = 18,
		RightBottom = 19,
			
		Default = ExImageMode::Scale,
		Center = ExImageMode::CenterMiddle,

		Grids = 0x01000000,
	};

	EXENUM(ExGridsImageMode)
	{
		Scale = 0x00000000,

		LeftNone = 0x00000001,
		LeftTile = 0x00000002,
		LeftMirror = 0x00000004,

		TopNone = 0x00000010,
		TopTile = 0x00000020,
		TopMirror = 0x00000040,

		RightNone = 0x00000100,
		RightTile = 0x00000200,
		RightMirror = 0x00000400,

		BottomNone = 0x00001000,
		BottomTile = 0x00002000,
		BottomMirror = 0x00004000,

		CenterNone = 0x00010000,
		CenterTile = 0x00020000,
		CenterMirror = 0x00040000,

		Default = ExGridsImageMode::Scale,
	};

	struct ExGridsImageInfo
	{
		float left;
		float top;
		float right;
		float bottom;
		uint32_t flags;

		ExGridsImageInfo()
		{
			left = top = right = bottom = 0.0F;
			flags = ExGridsImageMode::Default;
		}

		ExGridsImageInfo(float left, float top, float right, float bottom, uint32_t flags = ExGridsImageMode::Default)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
			this->flags = flags;
		}
		ExGridsImageInfo(float size, uint32_t flags = ExGridsImageMode::Default) 
			: ExGridsImageInfo(size, size, size, size, flags)
		{ }
		ExGridsImageInfo(float horz,float vert, uint32_t flags = ExGridsImageMode::Default)
			: ExGridsImageInfo(horz, vert, horz, vert, flags)
		{ }

		inline bool IsEmpty() const { return left == 0.0F && top == 0.0F && right == 0.0F && bottom == 0.0F; }
	};

	EXENUM(ExIconPos)
	{
		Left,
		Top,
		Right,
		Bottom,
	};

	EXENUM(ExCanvasDrawMode)
	{
		Blend = 0x00000000,
		Over = 0x00000001,
		OverBlend = 0x00000002,
	};

	/////////////////

	interface IExCanvasTarget
	{
	};

	EXINTERFACE("7BBF8CF9-3257-4A3A-8409-26A9EC418221") IExCanvas : public IExRenderObject
	{
		EXMETHOD HRESULT EXOBJCALL CreateTarget(uint32_t width, uint32_t height, bool copy, IExCanvasTarget * *r_target) PURE;
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
			float radius) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawCustomRoundRect(const IExPen* pen, float left, float top, float right, float bottom,
			float radius_left_top, float radius_right_top, float radius_right_bottom, float radius_left_bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawPath(const IExPen* pen, const IExPath* path) PURE;

		EXMETHOD HRESULT EXOBJCALL FillRect(const IExBrush* brush, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL FillEllipse(const IExBrush* brush, float left, float top, float right, float bottom) PURE;
		EXMETHOD HRESULT EXOBJCALL FillRoundRect(const IExBrush* brush, float left, float top, float right, float bottom,
			float radius) PURE;
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
			ExImageMode mode = ExImageMode::Default, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawImagePart(const IExImage* image, float left, float top,
			float src_left, float src_top, float src_right, float src_bottom, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawImagePartRect(const IExImage* image, float left, float top, float right, float bottom,
			float src_left, float src_top, float src_right, float src_bottom,
			ExImageMode mode = ExImageMode::Default, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawGridsImage(const IExImage* image, float left, float top, float right, float bottom,
			ExGridsImageInfo* grids, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;
		EXMETHOD HRESULT EXOBJCALL DrawGridsImagePart(const IExImage* image, float left, float top, float right, float bottom,
			float src_left, float src_top, float src_right, float src_bottom,
			ExGridsImageInfo* grids, EXCHANNEL alpha = ALPHA_OPAQUE) PURE;

		EXMETHOD HRESULT EXOBJCALL DrawShadow(const IExBrush* bkg_brush, float left, float top, float right, float bottom,
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


	};

}
