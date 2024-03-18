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
	/// 文本格式
	enum ExTextFormat
	{
		EX_TEXT_FORMAT_LEFT = 0x00000000,							///< 文本格式：左对齐
		EX_TEXT_FORMAT_TOP = 0x00000000,							///< 文本格式：顶对齐
		EX_TEXT_FORMAT_CENTER = 0x00000001,							///< 文本格式：水平居中
		EX_TEXT_FORMAT_RIGHT = 0x00000002,							///< 文本格式：右对齐
		EX_TEXT_FORMAT_MIDDLE = 0x00000004,							///< 文本格式：垂直居中
		EX_TEXT_FORMAT_BOTTOM = 0x00000008,							///< 文本格式：底对齐
		EX_TEXT_FORMAT_WORDBREAK = 0x00000010,						///< 文本格式：按字断开
		EX_TEXT_FORMAT_SINGLELINE = 0x00000020,						///< 文本格式：单行
		EX_TEXT_FORMAT_EXPANDTABS = 0x00000040,						///< 文本格式：展开制表符 
		EX_TEXT_FORMAT_TABSTOP = 0x00000080,						///< 文本格式：自定制表符宽度 (高字节表示制表符宽度)
		EX_TEXT_FORMAT_NOCLIP = 0x00000100,							///< 文本格式：忽略裁剪
		//EX_TEXT_FORMAT_EXTERNALLEADING = 0x00000200,				///< 文本格式：

		EX_TEXT_FORMAT_PREFIX = 0x00000800,							///< 文本格式：转义前缀符 (&转义为下划线)
		EX_TEXT_FORMAT_PATH_ELLIPSIS = 0x00004000,					///< 文本格式：路径省略 (路径中间省略)
		EX_TEXT_FORMAT_END_ELLIPSIS = 0x00008000,					///< 文本格式：末尾省略 (末尾变为省略号)
		EX_TEXT_FORMAT_RTLREADING = 0x00020000,						///< 文本格式：从右往左
		EX_TEXT_FORMAT_WORD_ELLIPSIS = 0x00040000,					///< 文本格式：单词忽略 (按单词省略)
		//EX_TEXT_FORMAT_NOFULLWIDTHCHARBREAK = 0x00080000,			///< 文本格式：
		EX_TEXT_FORMAT_HIDEPREFIX = 0x00100000,						///< 文本格式：隐藏前缀符 (被转义的&不显示下划线)

		EX_TEXT_FORMAT_VERTICAL = 0x00200000						///< 文本格式：垂直显示
	};

	enum ExImageMode
	{
		EX_IMAGE_MODE_LEFT = 0x00000001,
		EX_IMAGE_MODE_CENTER = 0x00000002,
		EX_IMAGE_MODE_RIGHT = 0x00000004,
		EX_IMAGE_MODE_TOP = 0x00000010,
		EX_IMAGE_MODE_MIDDLE = 0x00000020,
		EX_IMAGE_MODE_BOTTOM = 0x00000040,

		EX_IMAGE_MODE_SCALE = 0x00000000,
		EX_IMAGE_MODE_SCALE_FILL = 0x00000100,
		EX_IMAGE_MODE_SCALE_CENTER = 0x00000200,
		EX_IMAGE_MODE_TILE = 0x00000400,
		EX_IMAGE_MODE_MIRROR = 0x00000800,


		EX_IMAGE_MODE_DEFAULT = EX_IMAGE_MODE_SCALE,
		EX_IMAGE_MODE_CENTER = EX_IMAGE_MODE_CENTER | EX_IMAGE_MODE_MIDDLE,
		EX_IMAGE_MODE_LEFT_TOP = EX_IMAGE_MODE_LEFT | EX_IMAGE_MODE_TOP,

		EX_IMAGE_MODE_GRIDS = 0x01000000,
	};

	enum ExGridsImageMode
	{
		EX_GRIDS_IMAGE_SCALE = 0x00000000,

		EX_GRIDS_IMAGE_LEFT_NONE = 0x00000001,
		EX_GRIDS_IMAGE_LEFT_TILE = 0x00000002,
		EX_GRIDS_IMAGE_LEFT_MIRROR = 0x00000004,

		EX_GRIDS_IMAGE_TOP_NONE = 0x00000100,
		EX_GRIDS_IMAGE_TOP_TILE = 0x00000200,
		EX_GRIDS_IMAGE_TOP_MIRROR = 0x00000400,

		EX_GRIDS_IMAGE_RIGHT_NONE = 0x00010000,
		EX_GRIDS_IMAGE_RIGHT_TILE = 0x00020000,
		EX_GRIDS_IMAGE_RIGHT_MIRROR = 0x00040000,

		EX_GRIDS_IMAGE_BOTTOM_NONE = 0x01000000,
		EX_GRIDS_IMAGE_BOTTOM_TILE = 0x02000000,
		EX_GRIDS_IMAGE_BOTTOM_MIRROR = 0x04000000,

		EX_GRIDS_IMAGE_DEFAULT = EX_GRIDS_IMAGE_SCALE,
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
			flags = EX_GRIDS_IMAGE_DEFAULT;
		}

		ExGridsImageInfo(float left, float top, float right, float bottom, uint32_t flags = EX_GRIDS_IMAGE_DEFAULT)
		{
			this->left = left;
			this->top = top;
			this->right = right;
			this->bottom = bottom;
			this->flags = flags;
		}
		ExGridsImageInfo(float size, uint32_t flags = EX_GRIDS_IMAGE_DEFAULT) 
			: ExGridsImageInfo(size, size, size, size, flags)
		{ }
		ExGridsImageInfo(float horz,float vert, uint32_t flags = EX_GRIDS_IMAGE_DEFAULT)
			: ExGridsImageInfo(horz, vert, horz, vert, flags)
		{ }

		inline bool IsEmpty() const { return left == 0.0F && top == 0.0F && right == 0.0F && bottom == 0.0F; }
	};

	enum ExIconPos
	{
		EX_ICON_POS_LEFT,
		EX_ICON_POS_TOP,
		EX_ICON_POS_RIGHT,
		EX_ICON_POS_BOTTOM,
	};

	enum ExCanvasDrawMode
	{
		EX_CANVAS_DRAW_MODE_BLEND = 0x00000000,
		EX_CANVAS_DRAW_MODE_OVER = 0x00000001,
		EX_CANVAS_DRAW_MODE_OVER_BLEND = 0x00000002,
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


	}

}
