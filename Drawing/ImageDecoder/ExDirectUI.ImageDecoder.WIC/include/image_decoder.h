/**
 * @file image_decoder.h
 * @brief 图像解码器模块头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */
#pragma once
#include "common/interfaces/object_impl.hpp"
#include "common/utils/singleton.hpp"

namespace ExDirectUI
{
	class ExImageDecoderWIC
		: public ExObjectBaseImpl<IExImageDecoder>,
		public ExLazySingleton<ExImageDecoderWIC>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IExImageDecoder);
		EX_INTERFACE_ENTRY(IExModule);
		EX_INTERFACE_ENTRY(IUnknown);
		EX_END_INTERFACE_MAP();

	public:
		ExImageDecoderWIC();
		virtual ~ExImageDecoderWIC();

		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return L"ExImageDecoderWIC: core: WIC, support_format: PNG/JPG/GIF/BMP";
		}
		
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_pool;
			case 1: return m_factory;
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
		EXMETHOD HRESULT EXOBJCALL OnModuleUnLoad() override 
		{ 
			ExImageDecoderWIC::ClearInstance(false); 
			return S_OK;
		}
		
	public:

		EXSTDMETHOD LoadImageFromFile(LPCWSTR file, IExDecodeImage** r_image) override;
		EXSTDMETHOD LoadImageFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) override;

		HRESULT EXOBJCALL FreeImage(const ExDecodeImageContextWIC* image_context);

	private:
		
		static HRESULT CALLBACK _OnInitImage(IExResPool* pool, EXATOM key,
			const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res);
		
		static HRESULT CALLBACK _OnFreeImage(IExResPool* pool, EXATOM key,
			DWORD flags, void* res);

	private:
		ExAutoPtr<IExResPool> m_pool;
		ExAutoPtr<IWICImagingFactory> m_factory;
		
		inline static const ExModuleInfo MODULE_INFO = 
		{
			ATOM_EXDIRECTUI_IMAGEDECODER_WIC,
			EX_MODULE_IMAGE_DECODER,
			EX_MODULE_FLAG_NONE,
			L"ExDirectUI.ImageDecoder.WIC",
			L"1.0.0.0",
			L"EternalZSY@ExDirectUI",
			L"ExDirectUI图像解码器(内核：WIC，支持格式：PNG/JPG/GIF/BMP)"
		};
		
	};
	
}
