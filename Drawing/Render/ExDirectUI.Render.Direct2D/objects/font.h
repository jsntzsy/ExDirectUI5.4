/**
 * @file font.h
 * @brief 渲染器字体头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	struct ExFontContextD2D
	{
		EXATOM atom;
		ExFontInfo info;
		IDWriteTextFormat* font;
	};

	struct ExFontFileContextD2D
	{
		EXATOM atom;
		ExData data;
		IDWriteFontCollection* collection;
	};

	class ExFontPoolD2D : public ExLazySingleton<ExFontPoolD2D>
	{
	public:
		ExFontPoolD2D(ExRenderD2D* render);
		~ExFontPoolD2D();

		const ExFontFileContextD2D* EXOBJCALL LoadFontFile(const byte_t* data, size_t size) MAYTHROW;
		void EXOBJCALL UnLoadFontFile(EXATOM atom) MAYTHROW;

		const ExFontContextD2D* EXOBJCALL CreateFont(const ExFontInfo* info) MAYTHROW;
		void EXOBJCALL DestroyFont(EXATOM atom) MAYTHROW;

	private:
		static HRESULT CALLBACK OnInitFontFileContext(IExResPool* pool, EXATOM key,
			const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res);
		static HRESULT CALLBACK OnFreeFontFileContext(IExResPool* pool, EXATOM key,
			DWORD flags, void* res);

		static HRESULT CALLBACK OnInitFontContext(IExResPool* pool, EXATOM key,
			const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res);
		static HRESULT CALLBACK OnFreeFontContext(IExResPool* pool, EXATOM key,
			DWORD flags, void* res);

		ExAutoPtr<IExResPool> m_font_pool;
		ExAutoPtr<IExResPool> m_file_pool;
		wchar_t m_local_name[130]{};
	};

	///////////////////////

	class ExFontD2D : public ExObjectBaseImpl<IExFont>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExFont);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			const auto info = m_context->info;
			return ExString::format(L"ExFont: name: %s, size: %u, style: 0x%04X, file: 0x%08X", 
				info.name, info.size, info.style, info.file_atom
			);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_context->font;
			case 1: return (void*)&m_context->info;
			case 2: return (void*)m_context;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExFontD2D();
		ExFontD2D(const wchar_t name[LF_FACESIZE], uint32_t size, DWORD style, EXATOM file_atom);
		ExFontD2D(const ExFontInfo* info);
		virtual ~ExFontD2D();

		EXMETHOD LPCWSTR EXCALL GetName(EXATOM* r_atom = nullptr) const override;
		EXMETHOD uint32_t EXCALL GetSize() const override;
		EXMETHOD DWORD EXCALL GetStyle() const override;
		EXMETHOD HRESULT EXOBJCALL GetInfo(ExFontInfo* r_info) const override;

		EXMETHOD HRESULT EXOBJCALL SetName(const wchar_t name[LF_FACESIZE], EXATOM atom = EXATOM_UNKNOWN) override;
		EXMETHOD HRESULT EXOBJCALL SetSize(uint32_t size) override;
		EXMETHOD HRESULT EXOBJCALL SetStyle(DWORD style) override;
		EXMETHOD HRESULT EXOBJCALL SetInfo(const ExFontInfo* info) override;

	private:
		const ExFontContextD2D* m_context;
	};

}
