/**
 * @file image_decoder.h
 * @brief 图像解码器模块头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once
#include "common/interfaces/object_impl.hpp"
#include "common/utils/singleton.hpp"

namespace ExDirectUI
{
	class ExImageDecoderLibPng 
		: public ExObjectBaseImpl<IExImageDecoder>,
		public ExLazySingleton<ExImageDecoderLibPng>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IExImageDecoder);
		EX_INTERFACE_ENTRY(IExModule);
		EX_INTERFACE_ENTRY(IUnknown);
		EX_END_INTERFACE_MAP();

	public:
		ExImageDecoderLibPng();
		virtual ~ExImageDecoderLibPng();

		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			//std::wstringstream ss;
			//ss << L"ExImageDecoderLibPng";
			//return ss.str();
			
			return L"ExImageDecoderLibPng";
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_pool;
			default: return __super::GetContext(index);
			}
		}

	public:
		
		EXMETHOD EXATOM EXOBJCALL GetId() const override { return MODULE_INFO.id;}
		EXMETHOD uint16_t EXOBJCALL GetType() const override { return MODULE_INFO.type; }
		EXMETHOD HRESULT EXOBJCALL GetInfo(ExModuleInfo* r_info) const override 
		{
			CHECK_PARAM(r_info);
			*r_info = MODULE_INFO;
			return S_OK;
		}
		EXMETHOD HRESULT EXOBJCALL Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result) override;
		
	public:

		EXSTDMETHOD LoadImageFromFile(LPCWSTR file, IExDecodeImage** r_image) override;
		EXSTDMETHOD LoadImageFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) override;

		HRESULT EXOBJCALL FreeImage(const ExDecodeImageContextLibPng* image_context);

	private:
		
		static HRESULT CALLBACK _OnInitImage(IExResPool* pool, EXATOM key,
			const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res);
		
		static HRESULT CALLBACK _OnFreeImage(IExResPool* pool, EXATOM key,
			DWORD flags, void* res);

	private:
		ExAutoPtr<IExResPool> m_pool;
		inline static const ExModuleInfo MODULE_INFO = {};
		
	};

}
