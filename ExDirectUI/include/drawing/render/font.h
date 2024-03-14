/**
 * @file font.h
 * @brief 渲染器字体头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-30-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 字体风格
	enum ExFontStyle
	{
		EX_FONT_STYLE_NORMAL = 0x0000,				///< 字体风格：正常
		EX_FONT_STYLE_BOLD = 0x0001,				///< 字体风格：粗体
		EX_FONT_STYLE_ITALIC = 0x0002,				///< 字体风格：斜体
		EX_FONT_STYLE_UNDERLINE = 0x0004,			///< 字体风格：下划线
		EX_FONT_STYLE_STRIKEOUT = 0x0008,			///< 字体风格：删除线

		EX_FONT_STYLE_CUSTOM_WEIGHT = 0x0100,		///< 字体风格：自定义粗细 <此时style高16位表示粗细,取值一般在0-1000,可参考FW_开头的常量>
	};

	/// 字体信息
	struct ExFontInfo
	{
		/// 字体尺寸
		uint32_t size;

		/// 字体风格 
		DWORD style;

		/// 自定义字体文件原子号
		EXATOM file_atom;

		/// 字体名称
		wchar_t name[LF_FACESIZE];

		///////////////

		ExFontInfo() :size(), style(), file_atom(EXATOM_UNKNOWN), name({}) {}
		ExFontInfo(const wchar_t name[LF_FACESIZE], uint32_t size, DWORD style,
			EXATOM file_atom = EXATOM_UNKNOWN) :size(size), style(style)
		{
			SetName(name, file_atom);
		}
		ExFontInfo(const LOGFONTW* log_font, EXATOM file_atom = EXATOM_UNKNOWN)
		{
			size = abs(log_font->lfHeight);

			style = (log_font->lfItalic ? EX_FONT_STYLE_ITALIC : 0) |
				(log_font->lfUnderline ? EX_FONT_STYLE_UNDERLINE : 0) |
				(log_font->lfStrikeOut ? EX_FONT_STYLE_STRIKEOUT : 0);
			
			if (log_font->lfWeight == FW_BOLD) { style |= EX_FONT_STYLE_BOLD; }
			else if (log_font->lfWeight != FW_NORMAL) { 
				style |= EX_FONT_STYLE_CUSTOM_WEIGHT; 
				style = MAKEDWORD(style, log_font->lfWeight);
			}

			SetName(log_font->lfFaceName, file_atom);
		}

		inline bool IsBold()
		{
			if (style & EX_FONT_STYLE_CUSTOM_WEIGHT) { return HIWORD(style) >= FW_BOLD; }
			else { return !!(style & EX_FONT_STYLE_BOLD); };
		}
		inline bool IsItalic() { return !!(style & EX_FONT_STYLE_ITALIC); }
		inline bool IsUnderline() { return !!(style & EX_FONT_STYLE_UNDERLINE); }
		inline bool IsStrikeout() { return !!(style & EX_FONT_STYLE_STRIKEOUT); }
		inline uint16_t GetWeight()
		{
			if (style & EX_FONT_STYLE_CUSTOM_WEIGHT) { return HIWORD(style); }
			else { return (style & EX_FONT_STYLE_BOLD) ? FW_BOLD : FW_NORMAL; }
		}
		inline void SetWeight(int16_t weight)
		{
			if (weight < 0) {
				style = LOWORD(style) & ~EX_FONT_STYLE_CUSTOM_WEIGHT;
			}
			else {
				style = MAKEDWORD(
					(style | EX_FONT_STYLE_CUSTOM_WEIGHT),
					weight
				);
			}
		}
		inline void SetName(const wchar_t name[LF_FACESIZE], EXATOM atom = EXATOM_UNKNOWN)
		{
			wcsncpy(this->name, name, LF_FACESIZE - 1);
			if (atom != EXATOM_UNKNOWN) { this->atom = atom; }
		}

	};

	EXINTERFACE("879F7C79-F491-480D-AA92-EDD0F5E32F5C") IExFont : public IExRenderObject
	{
		EXMETHOD LPCWSTR EXCALL GetName(EXATOM * r_atom = nullptr) PURE;
		EXMETHOD uint32_t EXCALL GetSize() PURE;
		EXMETHOD DWORD EXCALL GetStyle() PURE;
		EXSTDMETHOD GetInfo(ExFontInfo* r_info) PURE;

		EXSTDMETHOD SetName(const wchar_t name[LF_FACESIZE], EXATOM atom = EXATOM_UNKNOWN) PURE;
		EXSTDMETHOD SetSize(uint32_t size) PURE;
		EXSTDMETHOD SetStyle(DWORD style) PURE;
		EXSTDMETHOD SetInfo(const ExFontInfo* info) PURE;
	};

	/////////////////////////////////

	//HRESULT EXAPI EXCALL ExFontLoadFile(const byte_t* data, size_t size, EXATOM* r_atom);
	//HRESULT EXAPI EXCALL ExFontUnLoadFile(EXATOM atom);
	//HRESULT EXAPI EXCALL ExFontCreate(IExFont** r_font);
	//HRESULT EXAPI EXCALL ExFontCreateFromName(const wchar_t name[LF_FACESIZE], uint32_t size,
	//	DWORD style, EXATOM file_atom, IExFont** r_font);
	//HRESULT EXAPI EXCALL ExFontCreateFromInfo(const ExFontInfo* info, IExFont** r_font);

}
