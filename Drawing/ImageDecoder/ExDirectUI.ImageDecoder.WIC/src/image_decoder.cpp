/**
 * @file image_decoder.cpp
 * @brief 图像解码器实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */

#include "stdafx.h"
#include "image_decoder.h"
#include "common/string.hpp"

namespace ExDirectUI
{
	ExImageDecoderWIC::ExImageDecoderWIC()
	{
		throw_if_failed(CoInitialize(nullptr), L"COM初始化失败");

		//创建WICImagingFactory
		throw_if_failed(
			CoCreateInstance(CLSID_WICImagingFactory, nullptr,
				CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_factory)
			), L"创建WICImagingFactory失败"
		);

		throw_if_failed(
			ExResPoolCreate(sizeof(ExDecodeImageContextWIC),
				_OnInitImage, _OnFreeImage, -m_pool
			), L"创建图像资源池失败"
		);
	}
	ExImageDecoderWIC::~ExImageDecoderWIC()
	{
	}

	HRESULT EXOBJCALL ExImageDecoderWIC::Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result)
	{
		return E_NOTIMPL;
	}

	HRESULT EXOBJCALL ExImageDecoderWIC::LoadImageFromFile(LPCWSTR file, IExDecodeImage** r_image)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_image);
		ExAssert(m_pool);

		//获取文件原子号
		EXATOM atom = ExAtomFile(file);
		if (atom == EXATOM_UNKNOWN) { return EE_IO; }

		//加载图像上下文
		ExDecodeImageContextWIC* context = nullptr;
		handle_if_notok(
			m_pool->UseOrCreateItem(atom, file, 0, 0, 0, (void**)&context),
			L"加载图像失败"
		);

		//创建解码图像对象
		ExAutoPtr<ExDecodeImageWIC> image = NEW ExDecodeImageWIC(context);
		return image->QueryInterface(r_image);
	}

	HRESULT EXOBJCALL ExImageDecoderWIC::LoadImageFromMemory(const byte_t* data, size_t size, IExDecodeImage** r_image)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(size > 0);
		CHECK_PARAM(r_image);
		ExAssert(m_pool);

		//获取文件原子号
		EXATOM atom = ExAtomData(data, size);
		if (atom == EXATOM_UNKNOWN) { return EE_IO; }

		//加载图像上下文
		ExDecodeImageContextWIC* context = nullptr;
		handle_if_notok(
			m_pool->UseOrCreateItem(atom, data, size, 0, 0, (void**)&context),
			L"加载图像失败"
		);

		//创建解码图像对象
		ExAutoPtr<ExDecodeImageWIC> image = NEW ExDecodeImageWIC(context);
		return image->QueryInterface(r_image);
	}

	HRESULT EXOBJCALL ExImageDecoderWIC::FreeImage(const ExDecodeImageContextWIC* image_context)
	{
		CHECK_PARAM(image_context);
		ExAssert(m_pool);
		return m_pool->UnUseItemByPtr((void*)image_context);
	}

	HRESULT ExImageDecoderWIC::_OnInitImage(IExResPool* pool, EXATOM key,
		const void* data, WPARAM wparam, LPARAM lparam, DWORD flags, void* r_res)
	{
		try
		{
			ExDecodeImageContextWIC* context = (ExDecodeImageContextWIC*)r_res;

			auto factory = Instance()->m_factory;
			ExAutoPtr<IWICBitmapDecoder> decoder = nullptr;
			ExAutoPtr<IWICStream> stream = nullptr;

			//根据wparam判断data是文件名还是字节流
			if (wparam == 0) {
				throw_if_failed(
					factory->CreateDecoderFromFilename((LPCWSTR)data,
						nullptr, GENERIC_READ,
						WICDecodeMetadataCacheOnLoad,
						&decoder
					), L"创建图像文件解码器失败"
				);
			}
			else {
				throw_if_failed(factory->CreateStream(&stream), L"创建流");
				throw_if_failed(stream->InitializeFromMemory((LPBYTE)data, (DWORD)wparam), L"初始化流");

				throw_if_failed(
					factory->CreateDecoderFromStream(
						stream, nullptr,
						WICDecodeMetadataCacheOnLoad,
						&decoder
					), L"创建图像流解码器失败"
				);
			}

			//获取帧数
			uint32_t frame_count = 0;
			throw_if_failed(decoder->GetFrameCount(&frame_count), L"获取图像帧数失败");
			context->frame_count = frame_count;

			//获取第一帧对象
			ExAutoPtr<IWICBitmapFrameDecode> frame;
			throw_if_failed(decoder->GetFrame(0, &frame), L"获取首帧对象失败");

			//获取图像尺寸
			throw_if_failed(frame->GetSize(&context->width, &context->height), L"获取图像尺寸失败");

			//分配点阵数据
			uint32_t stride = context->width * sizeof(EXARGB);
			uint32_t frame_size = stride * context->height;
			context->frames_data = NEW EXCHANNEL[frame_size * frame_count];

			//如果是多帧图像,还要分配帧延时信息数组
			if (frame_count > 1) {
				context->delays = NEW uint32_t[frame_count];
			}

			//遍历每一帧,获取图像数据及帧延时
			for (uint32_t i = 0; i < context->frame_count; i++) {
				//如果不是第一帧,先获取该帧对象
				if (i != 0)
				{
					throw_if_failed(
						decoder->GetFrame(i, -frame),
						ExString::format(L"获取第%u帧图像对象失败", i + 1 ).c_str()
					);
				}

				//转换图像为32bppPBGRA
				ExAutoPtr<IWICBitmapSource> converted_frame;
				throw_if_failed(
					WICConvertBitmapSource(
						GUID_WICPixelFormat32bppPBGRA,
						frame,
						-converted_frame
					), ExString::format(L"转换第%u帧图像失败", i + 1).c_str()
				);

				//获取该帧宽高
				uint32_t frame_width = 0, frame_height = 0;
				throw_if_failed(
					frame->GetSize(&frame_width, &frame_height),
					ExString::format(L"获取第%u帧尺寸失败", i + 1).c_str()
				);

				//该帧顶点偏移量(单帧用不到)
				uint32_t frame_offset_x = 0, frame_offset_y = 0;

				//多帧的话,获取一下偏移(单帧用不到)
				ExAutoPtr<IWICMetadataQueryReader> frame_info;
				PROPVARIANT var{};
				if (frame_count > 1) {
					try {
						//获取该帧的元数据对象
						throw_if_failed(
							frame->GetMetadataQueryReader(-frame_info),
							ExString::format(L"获取第%u帧元数据对象失败", i + 1).c_str()
						);

						//获取该帧偏移量
						throw_if_failed(
							frame_info->GetMetadataByName(L"/imgdesc/Left", &var),
							ExString::format(L"获取第%u帧左偏移量失败", i + 1).c_str()
						);
						throw_if_false(var.vt == VT_UI2, EE_UNMATCH, L"属性类型不匹配");
						frame_offset_x = var.uiVal;
						PropVariantClear(&var);

						throw_if_failed(
							frame_info->GetMetadataByName(L"/imgdesc/Top", &var),
							ExString::format(L"获取第%u帧顶偏移量失败", i + 1).c_str()
						);
						throw_if_false(var.vt == VT_UI2, EE_UNMATCH, L"属性类型不匹配");
						frame_offset_y = var.uiVal;
						PropVariantClear(&var);

						//获取帧延时
						throw_if_failed(
							frame_info->GetMetadataByName(L"/grctlext/Delay", &var),
							ExString::format(L"获取第%u帧延时信息失败", i + 1).c_str()
						);

						//原单位为10ms,计算后为1ms,并修复0ms的情况
						if (var.vt == VT_UI2) { context->delays[i] = var.uiVal * 10; }
						if (context->delays[i] == 0) { context->delays[i] = 10; }

						PropVariantClear(&var);
					}
					catch_ignore({ UNREFERENCED_PARAMETER(e); context->delays[i] = 10; });
				}

				//除了第一帧外,且不是完整的图像尺寸,则先拷贝前一帧数据
				EXBITSDATA frame_data_offseted = context->frames_data + frame_size * i;
				if (i != 0 && (frame_width != context->width || frame_height != context->height)) {
					CopyMemory(
						context->frames_data + i * frame_size,
						context->frames_data + (i - 1) * frame_size,
						frame_size
					);

					//计算缓冲区偏移量(如果是完整尺寸的图像,则偏移量为0)
					frame_data_offseted += frame_offset_y * stride;
					frame_data_offseted += frame_offset_x * sizeof(EXARGB);
				}

				//拷贝像素数据到缓冲区
				throw_if_failed(
					converted_frame->CopyPixels(
						nullptr, stride, frame_size,
						frame_data_offseted
					), ExString::format(L"拷贝第%u帧像素数据失败", i + 1).c_str()
				);
			}
			
			return S_OK;
		}
		//因为不一定由本解码器解码,所以如果不成功返回S_FALSE(不处理异常)
		catch_ignore({ UNREFERENCED_PARAMETER(e); return S_FALSE; });
	}

	HRESULT ExImageDecoderWIC::_OnFreeImage(IExResPool* pool, EXATOM key, DWORD flags, void* res)
	{
		ExDecodeImageContextWIC* context = (ExDecodeImageContextWIC*)res;
		SAFE_DELETE_ARR(context->delays);
		SAFE_DELETE_ARR(context->frames_data);
		return S_OK;
	}

}


