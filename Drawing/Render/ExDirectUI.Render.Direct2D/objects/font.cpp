/**
 * @file font.cpp
 * @brief 渲染器字体实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */

#include "stdafx.h"
#include "objects/font.h"
#include "common/unknown_impl.hpp"
#include <render_api.h>

namespace ExDirectUI
{
	// 字体文件流对象
	class ExFontFileStream : public ExUnknownImpl<IDWriteFontFileStream>
	{
	public:
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IDWriteFontFileStream);
		EX_DECLEAR_INTERFACE_END();

		ExFontFileStream(ExData* data) { m_data = *data; }

		virtual HRESULT __stdcall ReadFileFragment(void const** fragmentStart, UINT64 fileOffset,
			UINT64 fragmentSize, void** fragmentContext) override
		{
			*fragmentContext = nullptr;

			//在数据区内
			if (fileOffset < m_data.size && fragmentSize <= m_data.size - fileOffset)
			{
				*fragmentStart = m_data.data + fileOffset;
				return S_OK;
			}
			else
			{
				*fragmentStart = nullptr;
				return E_FAIL;
			}
		}

		virtual void __stdcall ReleaseFileFragment(void* fragmentContext) override
		{
		}
		virtual HRESULT __stdcall GetFileSize(UINT64* fileSize) override
		{
			*fileSize = m_data.size;
			return S_OK;
		}
		virtual HRESULT __stdcall GetLastWriteTime(UINT64* lastWriteTime) override
		{
			*lastWriteTime = 0;
			return E_NOTIMPL;
		}
	private:
		ExData m_data;
	};

	// 字体文件加载器对象
	class ExFontFileLoader : public ExUnknownImpl<IDWriteFontFileLoader>
	{
		friend ExLazySingleton<ExFontFileLoader>;

	public:
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IDWriteFontFileLoader);
		EX_DECLEAR_INTERFACE_END();

		virtual HRESULT __stdcall CreateStreamFromKey(void const* fontFileReferenceKey, UINT32 fontFileReferenceKeySize,
			IDWriteFontFileStream** fontFileStream) override
		{
			if (fontFileReferenceKey != nullptr && fontFileReferenceKeySize == sizeof(ExData))
			{
				ExData* data = (ExData*)fontFileReferenceKey;
				if (data->data != nullptr && data->size > 0)
				{
					*fontFileStream = new ExFontFileStream(data);
					return (*fontFileStream) ? S_OK : E_FAIL;
				}
			}
			return E_FAIL;
		}

	};

	// 字体文件枚举器对象
	class ExFontFileEnumerator : public ExUnknownImpl<IDWriteFontFileEnumerator>
	{
	public:
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IDWriteFontFileEnumerator);
		EX_DECLEAR_INTERFACE_END();


		ExFontFileEnumerator(IDWriteFactory* pFactory, ExData* aDatas, UINT cDatas)
			: m_factory(pFactory), m_data_list(aDatas), m_data_count(cDatas), m_current_file(nullptr), m_next_index(0)
		{
		}

		virtual HRESULT __stdcall MoveNext(BOOL* hasCurrentFile) override
		{
			HRESULT hr = S_OK;

			//释放当前字体文件
			SAFE_RELEASE(m_current_file);
			*hasCurrentFile = FALSE;

			//有下一个字体文件
			if (m_next_index < m_data_count)
			{
				ExData* data = m_data_list + m_next_index;
				if (data->data != nullptr && data->size > 0)
				{
					//创建字体文件流
					hr = m_factory->CreateCustomFontFileReference(data, sizeof(ExData),
						ExLazySingleton<ExFontFileLoader>::Instance(), &m_current_file);
					if (SUCCEEDED(hr))
					{
						*hasCurrentFile = TRUE;
						m_next_index++;
					}
				}
				else hr = E_FAIL;
			}

			return hr;
		}

		virtual HRESULT __stdcall GetCurrentFontFile(IDWriteFontFile** fontFile) override
		{
			*fontFile = m_current_file;
			if (m_current_file == nullptr) return E_FAIL;
			m_current_file->AddRef();
			return S_OK;
		}

	private:
		IDWriteFactory* m_factory;
		ExData* m_data_list;
		UINT m_data_count;
		IDWriteFontFile* m_current_file;
		UINT m_next_index;

	};

	// 字体集加载器对象
	class ExFontCollectionLoader : public ExUnknownImpl<IDWriteFontCollectionLoader>
	{
		friend ExLazySingleton<ExFontCollectionLoader>;

	public:
		EX_DECLEAR_INTERFACE_BEGIN();
		EX_DECLEAR_INTERFACE(IUnknown);
		EX_DECLEAR_INTERFACE(IDWriteFontCollectionLoader);
		EX_DECLEAR_INTERFACE_END();

		virtual HRESULT __stdcall CreateEnumeratorFromKey(IDWriteFactory* factory, void const* collectionKey,
			UINT32 collectionKeySize, IDWriteFontFileEnumerator** fontFileEnumerator) override
		{
			if (!collectionKey) return E_FAIL;
			if (collectionKeySize % sizeof(ExData) != 0) return E_FAIL;

			*fontFileEnumerator = new ExFontFileEnumerator(factory, (ExData*)collectionKey, collectionKeySize / sizeof(ExData));

			return (*fontFileEnumerator) ? S_OK : E_FAIL;
		}

	};

	/////////////////////////////

	ExFontPoolD2D::ExFontPoolD2D(ExRenderD2D* render)
	{
		//创建资源池
		throw_if_failed(
			ExResPoolCreate(sizeof(ExFontContextD2D),
				OnInitFontContext, OnFreeFontContext,
				&m_font_pool
			), L"创建字体池失败"
		);
		throw_if_failed(
			ExResPoolCreate(sizeof(ExFontFileContextD2D),
				OnInitFontFileContext, OnFreeFontFileContext,
				&m_file_pool
			), L"创建字体文件池失败"
		);

		//创建并注册字体文件加载器和字体集加载器
		throw_if_failed(
			render->m_dwrite_factory->RegisterFontFileLoader(
				ExLazySingleton<ExFontFileLoader>::GetInstance()
			), L"注册字体文件加载器失败"
		);

		throw_if_failed(
			render->m_dwrite_factory->RegisterFontCollectionLoader(
				ExLazySingleton<ExFontCollectionLoader>::GetInstance()
			), L"注册字体集加载器失败"
		);

		ExRenderAPI::GetUserDefaultLocaleName(m_local_name, _countof(m_local_name));
	}
	ExFontPoolD2D::~ExFontPoolD2D()
	{
		ZeroMemory(m_local_name, sizeof(m_local_name));

		//取消注册字体集加载器和字体文件加载器
		GetRender()->m_dwrite_factory->UnregisterFontCollectionLoader(
			ExLazySingleton<ExFontCollectionLoader>::Instance()
		);
		GetRender()->m_dwrite_factory->UnregisterFontFileLoader(
			ExLazySingleton<ExFontFileLoader>::Instance()
		);

		//释放字体集加载器和字体文件加载器
		ExLazySingleton<ExFontCollectionLoader>::ClearInstance(true);
		ExLazySingleton<ExFontFileLoader>::ClearInstance(true);
	}

	const ExFontFileContextD2D* EXOBJCALL ExFontPoolD2D::LoadFontFile(const byte_t* data, size_t size)
	{
		EXATOM key = ExAtomData(data, size);
		throw_if_failed(key != EXATOM_UNKNOWN, L"计算字体文件原子号失败");

		ExFontFileContextD2D* context = nullptr;
		throw_if_failed(
			m_file_pool->UseOrCreateItem(key, data, size, (LPARAM)this, 0, (void**)&context),
			L"获取字体文件上下文失败"
		);
		return context;
	}
	void EXOBJCALL ExFontPoolD2D::UnLoadFontFile(EXATOM atom)
	{
		throw_if_failed(m_file_pool->UnUseItem(atom), L"释放字体文件失败");
	}

	const ExFontContextD2D* EXOBJCALL ExFontPoolD2D::CreateFont(const ExFontInfo* info)
	{
		EXATOM key = ExAtomData(info, sizeof(ExFontInfo));
		throw_if_failed(key != EXATOM_UNKNOWN, L"计算字体原子号失败");

		ExFontContextD2D* context = nullptr;
		throw_if_failed(
			m_font_pool->UseOrCreateItem(key, info, 0, (LPARAM)this, 0, (void**)&context),
			L"获取字体上下文失败"
		);
		return context;
	}
	void EXOBJCALL ExFontPoolD2D::DestroyFont(EXATOM atom)
	{
		throw_if_failed(m_font_pool->UnUseItem(atom), L"释放字体失败");
	}

	HRESULT ExFontPoolD2D::OnInitFontFileContext(IExResPool* pool, EXATOM key, const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res)
	{
		ExFontFileContextD2D* context = (ExFontFileContextD2D*)r_res;
		context->data = {};

		try
		{
			//复制数据块
			ExData data_src = { (byte_t*)data, wparam };
			throw_if_failed(ExDataCopy(&data_src ,&context->data), L"复制字体文件数据失败");

			//创建字体集
			throw_if_failed(
				GetRender()->m_dwrite_factory->CreateCustomFontCollection(
					ExLazySingleton<ExFontCollectionLoader>::Instance(),
					&context->data, sizeof(ExData),
					&context->collection
				), L"创建字体集失败"
			);

			//返回
			context->atom = key;
			return S_OK;
		}
		catch_throw({ if (context->data.data) { ExDataFree(&context->data); } });
	}
	HRESULT ExFontPoolD2D::OnFreeFontFileContext(IExResPool* pool, EXATOM key, DWORD flags, void* res)
	{
		ExFontFileContextD2D* context = (ExFontFileContextD2D*)res;
		SAFE_RELEASE(context->collection);
		ExDataFree(&context->data);
		return S_OK;
	}
	HRESULT ExFontPoolD2D::OnInitFontContext(IExResPool* pool, EXATOM key, const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res)
	{
		ExFontContextD2D* context = (ExFontContextD2D*)r_res;
		const ExFontInfo* info = (ExFontInfo*)data;

		ExFontPoolD2D* that = (ExFontPoolD2D*)lparam;
		throw_if_false(that, E_INVALIDARG, L"字体池指针为空");
		ExFontFileContextD2D* file_context = nullptr;

		auto factory = GetRender()->m_dwrite_factory;

		try
		{
			//如果是自定义字体则引用，拿到字体文件上下文
			if (info->file_atom != EXATOM_UNKNOWN) {
				throw_if_failed(
					that->m_file_pool->UseItem(info->file_atom, (void**)&file_context),
					L"获取字体文件上下文失败"
				);
			}

			DWRITE_FONT_WEIGHT weight = (DWRITE_FONT_WEIGHT)info->GetWeight();
			DWRITE_FONT_STYLE style = info->IsItalic() ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
			DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL;

			//创建字体
			throw_if_failed(
				factory->CreateTextFormat(
					info->name,
					file_context ? file_context->collection : nullptr,
					weight, style, stretch,
					info->size,
					that->m_local_name,
					&context->font
				), L"创建字体失败"
			);
			
			context->atom = key;
			return S_OK;
		}
		catch_throw({
			if (context->font) { context->font->Release(); }
			if (file_context) { that->m_file_pool->UnUseItem(file_context->atom); }
			});
	}
	HRESULT ExFontPoolD2D::OnFreeFontContext(IExResPool* pool, EXATOM key, DWORD flags, void* res)
	{
		ExFontContextD2D* context = (ExFontContextD2D*)res;
		context->font->Release();
		if (context->info.file_atom != EXATOM_UNKNOWN) {
			ExFontPoolD2D::Instance()->m_file_pool->UnUseItem(context->info.file_atom);
		}
		return S_OK;
	}

	/////////////////////////



	ExFontD2D::ExFontD2D()
	{
		ExFontInfo info;
		throw_if_failed(GetUtils()->GetDefaultFont(&info), L"获取默认字体失败");
		m_context = ExFontPoolD2D::Instance()->CreateFont(&info);
	}

	ExFontD2D::ExFontD2D(const wchar_t name[LF_FACESIZE], uint32_t size, DWORD style, EXATOM file_atom)
	{
		ExFontInfo info;
		throw_if_failed(GetUtils()->GetDefaultFont(&info), L"获取默认字体失败");

		if (name) { info.SetName(name, file_atom); }
		if (size != -1) { info.size = size; }
		if (style != -1) { info.style = style; }

		m_context = ExFontPoolD2D::Instance()->CreateFont(&info);
	}

	ExFontD2D::ExFontD2D(const ExFontInfo* info)
	{
		m_context = ExFontPoolD2D::Instance()->CreateFont(info);
	}

	ExFontD2D::~ExFontD2D()
	{
		if (m_context) {
			ExFontPoolD2D::Instance()->DestroyFont(m_context->atom);
		}
	}

	LPCWSTR EXCALL ExFontD2D::GetName(EXATOM* r_atom) const
	{
		if (r_atom) { *r_atom = m_context->atom; }
		return m_context->info.name;
	}

	uint32_t EXCALL ExFontD2D::GetSize() const
	{
		return m_context->info.size;
	}

	DWORD EXCALL ExFontD2D::GetStyle() const
	{
		return m_context->info.style;
	}

	HRESULT EXOBJCALL ExFontD2D::GetInfo(ExFontInfo* r_info) const
	{
		CHECK_PARAM(r_info);
		*r_info = m_context->info;
		return S_OK;
	}

	HRESULT EXOBJCALL ExFontD2D::SetName(const wchar_t name[LF_FACESIZE], EXATOM atom)
	{
		CHECK_PARAM(name);
		ExFontInfo info = m_context->info;
		info.SetName(name, atom);
		return SetInfo(&info);
	}

	HRESULT EXOBJCALL ExFontD2D::SetSize(uint32_t size)
	{
		ExFontInfo info = m_context->info;
		info.size = size;
		return SetInfo(&info);
	}

	HRESULT EXOBJCALL ExFontD2D::SetStyle(DWORD style)
	{
		ExFontInfo info = m_context->info;
		info.style = style;
		return SetInfo(&info);
	}

	HRESULT EXOBJCALL ExFontD2D::SetInfo(const ExFontInfo* info)
	{
		try
		{
			auto pool = ExFontPoolD2D::Instance();
			auto old_context = m_context;
			m_context = pool->CreateFont(info);
			pool->DestroyFont(old_context->atom);
			return S_OK;
		}
		catch_default({});
	}

}


