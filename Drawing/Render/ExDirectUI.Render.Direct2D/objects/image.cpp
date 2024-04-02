/**
 * @file image.cpp
 * @brief 渲染器图像对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */

#include "stdafx.h"
#include "image.h"
#include "drawing/image_decoder.h"

namespace ExDirectUI
{
	ExImageD2D::ExImageD2D(uint32_t width, uint32_t height)
	{
		//创建WIC位图
		ExAutoPtr<IWICBitmap> bitmap;
		throw_if_failed(
			GetRender()->m_wic_factory->CreateBitmap(
				width, height, GUID_WICPixelFormat32bppPBGRA,
				WICBitmapCacheOnDemand, &bitmap),
			L"创建WIC位图失败"
		);

		//设置成员
		m_wic_bitmap = bitmap;
		m_width = width;
		m_height = height;
		m_cur_frame = 0;

		//刷新D2D位图
		throw_if_failed(Flush(), L"创建D2D位图失败");
	}
	ExImageD2D::ExImageD2D(LPCWSTR file)
	{
		//解码图像
		throw_if_failed(GetUtils()->DecodeImageFromFile(file, &m_decode_image), L"解码图像失败");

		//获取图像信息
		m_width = m_decode_image->GetWidth();
		m_height = m_decode_image->GetHeight();

		//创建WIC位图
		ExAutoPtr<IWICBitmap> bitmap;
		throw_if_failed(
			GetRender()->m_wic_factory->CreateBitmap(
				m_width, m_height, GUID_WICPixelFormat32bppPBGRA,
				WICBitmapCacheOnDemand, &bitmap),
			L"创建WIC位图失败"
		);

		//设置成员
		m_wic_bitmap = bitmap;
		m_cur_frame = -1;

		//切换到第一帧
		throw_if_failed(SetCurFrame(0), L"设置当前帧失败");
	}
	ExImageD2D::ExImageD2D(const byte_t* data, size_t size)
	{
		//解码图像
		throw_if_failed(GetUtils()->DecodeImageFromMemory(data, size, &m_decode_image), L"解码图像失败");

		//获取图像信息
		m_width = m_decode_image->GetWidth();
		m_height = m_decode_image->GetHeight();

		//创建WIC位图
		ExAutoPtr<IWICBitmap> bitmap;
		throw_if_failed(
			GetRender()->m_wic_factory->CreateBitmap(
				m_width, m_height, GUID_WICPixelFormat32bppPBGRA,
				WICBitmapCacheOnDemand, &bitmap),
			L"创建WIC位图失败"
		);

		//设置成员
		m_wic_bitmap = bitmap;
		m_cur_frame = -1;

		//切换到第一帧
		throw_if_failed(SetCurFrame(0), L"设置当前帧失败");
	}
	ExImageD2D::ExImageD2D(uint32_t width, uint32_t height, const EXBITSDATA bits_data, uint32_t stride)
	{
		if (stride == 0) { stride = width * sizeof(EXARGB); }

		//创建WIC位图
		ExAutoPtr<IWICBitmap> bitmap;
		throw_if_failed(
			GetRender()->m_wic_factory->CreateBitmapFromMemory(
				width, height, GUID_WICPixelFormat32bppPBGRA,
				stride, stride * height, bits_data, &bitmap),
			L"创建WIC位图失败"
		);

		//设置成员
		m_wic_bitmap = bitmap;
		m_width = width;
		m_height = height;
		m_cur_frame = 0;

		//刷新D2D位图
		throw_if_failed(Flush(), L"创建D2D位图失败");
	}
	ExImageD2D::ExImageD2D(const byte_t* image_data)
	{
		uint32_t* args = (uint32_t*)image_data;
		uint32_t width = args[0];
		uint32_t height = args[1];
		uint32_t stride = width * sizeof(EXARGB);
		EXBITSDATA bits_data = (EXBITSDATA)(args + 2);

		throw_if_false(width > 0 && height > 0, E_INVALIDARG, L"图像尺寸无效");

		//创建WIC位图
		ExAutoPtr<IWICBitmap> bitmap;
		throw_if_failed(
			GetRender()->m_wic_factory->CreateBitmapFromMemory(
				width, height, GUID_WICPixelFormat32bppPBGRA,
				stride, stride * height, bits_data, &bitmap),
			L"创建WIC位图失败"
		);

		//设置成员
		m_wic_bitmap = bitmap;
		m_width = width;
		m_height = height;
		m_cur_frame = 0;

		//刷新D2D位图
		throw_if_failed(Flush(), L"创建D2D位图失败");
	}
	ExImageD2D::ExImageD2D(HBITMAP bitmap, HPALETTE palette, bool pre_alpha)
	{
		//创建WIC位图
		ExAutoPtr<IWICBitmap> wic_bitmap;
		throw_if_failed(
			GetRender()->m_wic_factory->CreateBitmapFromHBITMAP(bitmap, palette,
				pre_alpha ? WICBitmapUsePremultipliedAlpha : WICBitmapIgnoreAlpha,
				&wic_bitmap),
			L"创建WIC位图失败"
		);

		//设置成员
		throw_if_failed(wic_bitmap->GetSize(&m_width, &m_height), L"获取图像尺寸失败");
		m_wic_bitmap = wic_bitmap;
		m_cur_frame = 0;

		//刷新D2D位图
		throw_if_failed(Flush(), L"创建D2D位图失败");
	}
	ExImageD2D::ExImageD2D(HICON icon)
	{
		auto render = GetRender();
		
		//通过HICON创建WIC位图
		ExAutoPtr<IWICBitmap> wic_bitmap;
		throw_if_failed(
			render->m_wic_factory->CreateBitmapFromHICON(icon, &wic_bitmap),
			L"创建WIC位图失败"
		);

		//创建像素格式转换器并初始化
		ExAutoPtr<IWICFormatConverter> converter;
		throw_if_failed(
			render->m_wic_factory->CreateFormatConverter(&converter),
			L"创建像素格式转换器失败"
		);
		throw_if_failed(
			converter->Initialize(wic_bitmap, GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone, nullptr, 0,
				WICBitmapPaletteTypeCustom
			), L"初始化像素格式转换器失败"
		);

		//创建符合像素格式要求的WIC位图
		throw_if_failed(
			render->m_wic_factory->CreateBitmapFromSource(converter,
				WICBitmapCacheOnDemand, &wic_bitmap),
			L"创建WIC位图对象失败"
		);

		//设置成员
		throw_if_failed(wic_bitmap->GetSize(&m_width, &m_height), L"获取图像尺寸失败");
		m_wic_bitmap = wic_bitmap;
		m_cur_frame = 0;

		//刷新D2D位图
		throw_if_failed(Flush(), L"创建D2D位图失败");
	}
	ExImageD2D::ExImageD2D(IWICBitmap* src_bitmap)
	{
		//设置成员
		throw_if_failed(src_bitmap->GetSize(&m_width, &m_height), L"获取图像尺寸失败");
		m_wic_bitmap = src_bitmap;
		m_cur_frame = 0;

		//刷新D2D位图
		throw_if_failed(Flush(), L"创建D2D位图失败");
	}
	ExImageD2D::~ExImageD2D()
	{
		//如果析构时图片锁着要先解锁
		m_bitmap_lock.Release();
	}
	uint32_t EXOBJCALL ExImageD2D::GetWidth() const
	{
		return m_width;
	}
	uint32_t EXOBJCALL ExImageD2D::GetHeight() const
	{
		return m_height;
	}
	uint32_t EXOBJCALL ExImageD2D::GetFrameCount() const
	{
		return m_decode_image ? m_decode_image->GetFrameCount() : 1;
	}
	uint32_t EXOBJCALL ExImageD2D::GetCurFrame() const
	{
		return m_cur_frame;
	}
	HRESULT EXOBJCALL ExImageD2D::SetCurFrame(uint32_t index)
	{
		//单帧图像且不是第一帧直接返回
		if (!m_decode_image && index != 0) { return E_NOTIMPL; }
		else if (!m_decode_image) { return S_FALSE; }

		//检查是否需要切换帧 (允许强制刷新)
		//if (index == m_cur_frame) { return S_FALSE; }

		//检查帧索引是否合法和图像是否被锁定
		handle_if_false(index < m_decode_image->GetFrameCount(), EE_OUTOFBOUNDS, L"帧索引越界");
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像当前被锁定");

		//锁定位图
		ExAutoPtr<IWICBitmapLock> lock;
		WICRect rc = { 0, 0, (INT)m_width, (INT)m_height };
		handle_if_failed(m_wic_bitmap->Lock(&rc, WICBitmapLockRead | WICBitmapLockWrite,
			&lock), L"图像锁定失败"
		);

		//获取帧数据
		UINT frame_size = 0;
		EXBITSDATA frame_data = nullptr;
		handle_if_failed(lock->GetDataPointer(&frame_size, &frame_data), L"获取图像锁数据失败");

		//获取行跨步
		UINT stride = 0;
		handle_if_failed(lock->GetStride(&stride), L"获取图像锁步长失败");

		//复制帧数据
		handle_if_failed(m_decode_image->CopyFrame(index, frame_data, stride), L"复制图像帧数据失败");

		//解锁位图
		lock.Release();

		//刷新D2D位图
		handle_if_failed(Flush(), L"刷新D2D位图失败");

		//设置当前帧
		m_cur_frame = index;
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageD2D::NextFrame(uint32_t* r_next_index, uint32_t* r_next_delay)
	{
		//单帧图像直接返回
		if (!m_decode_image) { return S_FALSE; }

		//获取下一帧索引
		uint32_t next_index = (m_cur_frame + 1) % m_decode_image->GetFrameCount();
		handle_if_failed(SetCurFrame(next_index), L"设置当前帧失败");
		if (r_next_index) { *r_next_index = next_index; }

		//获取下一帧延迟
		if (r_next_delay) { *r_next_delay = GetFrameDelay(); }

		return S_OK;
	}
	uint32_t EXOBJCALL ExImageD2D::GetFrameDelay() const
	{
		if (!m_decode_image) { return 0; }
		uint32_t delay = 0;
		handle_if_failed(m_decode_image->GetFrameDelay(m_cur_frame, &delay), L"获取当前帧延时失败");
		return delay;
	}
	HRESULT EXOBJCALL ExImageD2D::Lock(ExImageLock* r_lock, const ExRect* lock_rect)
	{
		CHECK_PARAM(r_lock);
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像已被锁定");

		//计算锁定区域
		WICRect rc = { 0, 0, (INT)m_width, (INT)m_height };
		if (lock_rect)
		{
			rc.X = lock_rect->GetLeft();
			rc.Y = lock_rect->GetTop();
			rc.Width = lock_rect->GetWidth();
			rc.Height = lock_rect->GetHeight();
		}

		//锁定图像
		ExAutoPtr<IWICBitmapLock> lock;
		handle_if_failed(m_wic_bitmap->Lock(&rc,
			WICBitmapLockRead | WICBitmapLockWrite,
			&lock),
			L"图像锁定失败"
		);

		//获取锁信息
		UINT width, height, stride;
		handle_if_failed(lock->GetSize(&width, &height), L"获取图像锁尺寸失败");
		handle_if_failed(lock->GetStride(&stride), L"获取图像锁步长失败");

		//获取像素数据
		UINT frame_size;
		EXBITSDATA frame_data = nullptr;
		handle_if_failed(
			lock->GetDataPointer(&frame_size, &frame_data),
			L"获取图像锁数据失败"
		);

		//写出信息
		const_cast<uint32_t&>(r_lock->width) = width;
		const_cast<uint32_t&>(r_lock->height) = height;
		const_cast<uint32_t&>(r_lock->stride) = stride;
		const_cast<EXBITSDATA&>(r_lock->data) = frame_data;
		const_cast<uintptr_t&>(r_lock->reserved) = (uintptr_t)lock.Get();

		//保存锁
		m_bitmap_lock = lock;
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageD2D::Unlock(ExImageLock* lock)
	{
		CHECK_PARAM(lock);
		handle_if_false(m_bitmap_lock != nullptr, EE_NOREADY, L"图像未被锁定");
		handle_if_false(lock->reserved == (uintptr_t)m_bitmap_lock.Get(), E_INVALIDARG, L"图像锁信息不匹配");

		//解锁图像
		m_bitmap_lock.Release();

		//清空锁信息
		ZeroMemory(lock, sizeof(ExImageLock));

		//刷新D2D位图
		handle_if_failed(Flush(), L"刷新D2D位图失败");
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageD2D::GetPixel(uint32_t x, uint32_t y, uint32_t* r_color)
	{
		CHECK_PARAM(r_color);
		CHECK_PARAM(x < m_width && y < m_height);

		//判断图像是否被锁定
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像当前被锁定");

		WICRect rc = { (INT)x, (INT)y, 1, 1 };
		EXCHANNEL* pixel = (EXCHANNEL*)r_color;
		ExAutoPtr<IWICBitmapLock> lock;

		//锁定
		handle_if_failed(
			m_wic_bitmap->Lock(&rc, WICBitmapLockRead | WICBitmapLockWrite, &lock),
			L"锁定图像失败"
		);

		//获取图像数据
		UINT frame_size = 0;
		EXBITSDATA frame_data = nullptr;
		handle_if_failed(lock->GetDataPointer(&frame_size, &frame_data), L"获取图像数据失败");

		//去除透明度预乘，设置到返回值中
		if (frame_data[3] != ALPHA_TRANSPARENT) {
			pixel[0] = (EXCHANNEL)MulDiv(frame_data[0], ALPHA_OPAQUE, frame_data[3]);
			pixel[1] = (EXCHANNEL)MulDiv(frame_data[1], ALPHA_OPAQUE, frame_data[3]);
			pixel[2] = (EXCHANNEL)MulDiv(frame_data[2], ALPHA_OPAQUE, frame_data[3]);
			pixel[3] = frame_data[3];
		}
		else { 
			pixel[0] = pixel[1] = pixel[2] = pixel[3] = 0;
		}

		//解锁
		lock.Release();
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageD2D::SetPixel(uint32_t x, uint32_t y, uint32_t color)
	{
		CHECK_PARAM(x < m_width && y < m_height);

		//判断图像是否被锁定
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像当前被锁定");

		WICRect rc = { (INT)x, (INT)y, 1, 1 };
		ExAutoPtr<IWICBitmapLock> lock;

		//锁定
		handle_if_failed(
			m_wic_bitmap->Lock(&rc, WICBitmapLockRead | WICBitmapLockWrite, &lock),
			L"锁定图像失败"
		);

		//预乘alpha
		EXCHANNEL* pixel = (EXCHANNEL*)&color;
		if (pixel[3] != ALPHA_TRANSPARENT) {
			pixel[0] = (EXCHANNEL)MulDiv(pixel[0], pixel[3], ALPHA_OPAQUE);
			pixel[1] = (EXCHANNEL)MulDiv(pixel[1], pixel[3], ALPHA_OPAQUE);
			pixel[2] = (EXCHANNEL)MulDiv(pixel[2], pixel[3], ALPHA_OPAQUE);
		}

		//设置图像数据
		UINT frame_size = 0;
		EXBITSDATA frame_data = nullptr;
		handle_if_failed(lock->GetDataPointer(&frame_size, &frame_data), L"获取图像数据失败");
		CopyMemory(frame_data, pixel, sizeof(color));

		//解锁并刷新
		lock.Release();
		handle_if_failed(Flush(), L"刷新D2D位图失败");
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageD2D::Copy(IExImage** r_dest, const ExRect* copy_rect)
	{
		CHECK_PARAM(r_dest);

		//检查图像是否被锁定
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像当前被锁定");

		//计算复制区域
		WICRect rc = { 0, 0, (INT)m_width, (INT)m_height };
		if (copy_rect)
		{
			rc.X = copy_rect->GetLeft();
			rc.Y = copy_rect->GetTop();
			rc.Width = copy_rect->GetWidth();
			rc.Height = copy_rect->GetHeight();
		}

		//创建一个新的WIC位图
		ExAutoPtr<IWICBitmap> new_bitmap;
		handle_if_failed(
			GetRender()->m_wic_factory->CreateBitmapFromSourceRect(
				m_wic_bitmap, rc.X, rc.Y, rc.Width, rc.Height, &new_bitmap
			), L"创建新WIC位图失败"
		);

		try
		{
			//用新WIC位图创建图像对象,并赋值
			ExAutoPtr<ExImageD2D> dest = new ExImageD2D(new_bitmap);
			return dest->QueryInterface(r_dest);
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExImageD2D::Scale(uint32_t new_width, uint32_t new_height, IExImage** r_dest, const ExRect* copy_rect)
	{
		CHECK_PARAM(r_dest);
		CHECK_PARAM(new_width > 0 && new_height > 0);

		//检查图像是否被锁定
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像当前被锁定");

		//计算复制区域
		WICRect rc = { 0, 0, (INT)m_width, (INT)m_height };
		if (copy_rect)
		{
			rc.X = copy_rect->GetLeft();
			rc.Y = copy_rect->GetTop();
			rc.Width = copy_rect->GetWidth();
			rc.Height = copy_rect->GetHeight();
		}

		auto* render = GetRender();
		D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);

		//创建一个WIC位图缩放器
		ExAutoPtr<IWICBitmapScaler> scaler;
		handle_if_failed(
			render->m_wic_factory->CreateBitmapScaler(&scaler),
			L"创建WIC位图缩放器失败"
		);

		//初始化WIC位图缩放器
		handle_if_failed(
			scaler->Initialize(m_wic_bitmap, new_width, new_height, WICBitmapInterpolationModeFant),
			L"初始化WIC位图缩放器失败"
		);

		//创建一个新的WIC位图
		ExAutoPtr<IWICBitmap> new_bitmap;
		handle_if_failed(
			render->m_wic_factory->CreateBitmapFromSourceRect(
				scaler, rc.X, rc.Y, rc.Width, rc.Height, &new_bitmap
			), L"创建新WIC位图失败"
		);

		try
		{
			//用新WIC位图创建图像对象,并赋值
			ExAutoPtr<ExImageD2D> dest = new ExImageD2D(new_bitmap);
			return dest->QueryInterface(r_dest);
		}
		catch_default({});


	}
	HRESULT EXOBJCALL ExImageD2D::Save(ExData* r_data)
	{
		CHECK_PARAM(r_data);

		HGLOBAL data = nullptr;
		LPVOID data_src = nullptr;

		try
		{
			auto render = GetRender();

			//创建内存块
			data = GlobalAlloc(GMEM_MOVEABLE, 0);
			throw_if_false(data, E_OUTOFMEMORY, L"分配内存失败");

			//创建流
			ExAutoPtr<IStream> stream;
			throw_if_failed(CreateStreamOnHGlobal(data, FALSE, &stream), L"创建流失败");

			//创建编码器并初始化
			ExAutoPtr<IWICBitmapEncoder> encoder;
			throw_if_failed(
				render->m_wic_factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder),
				L"创建编码器失败"
			);
			throw_if_failed(encoder->Initialize(stream, WICBitmapEncoderNoCache), L"初始化编码器失败");

			//创建编码帧对象并初始化
			ExAutoPtr<IWICBitmapFrameEncode> frame;
			throw_if_failed(encoder->CreateNewFrame(&frame, nullptr), L"创建帧对象失败");
			throw_if_failed(frame->Initialize(nullptr), L"初始化帧对象失败");

			//设置帧参数
			WICPixelFormatGUID guid_format = GUID_WICPixelFormat32bppPBGRA;
			throw_if_failed(frame->SetPixelFormat(&guid_format), L"设置帧像素格式失败");
			throw_if_failed(frame->SetSize(m_width, m_height), L"设置帧尺寸失败");

			//写出帧
			WICRect rect{ 0,0,(int)m_width,(int)m_height };
			throw_if_failed(frame->WriteSource(m_wic_bitmap, &rect), L"写出帧失败");

			//提交
			throw_if_failed(frame->Commit(), L"编码帧提交失败");
			throw_if_failed(encoder->Commit(), L"编码图像提交失败");

			//将数据转移到ExData里
			size_t size = ::GlobalSize(data);
			data_src = ::GlobalLock(data);
			throw_if_false(data_src, E_OUTOFMEMORY, L"内存锁定失败");

			//分配数据并拷贝
			ExData src_data = { (byte_t*)data_src,size };
			throw_if_failed(ExDataCopy(&src_data, r_data), L"数据复制失败");
			::GlobalUnlock(data);

			//释放内存
			stream.Release();
			::GlobalFree(data);
			return S_OK;
		}
		catch_default({
			SAFE_FREE(data_src,::GlobalUnlock);
			SAFE_FREE(data,::GlobalFree);
			});
	}

	HRESULT EXOBJCALL ExImageD2D::Flush()
	{
		ExAutoPtr<ID2D1Bitmap> bitmap;

		//通过当前WIC位图创建D2D位图
		return_if_failed(
			GetRender()->m_d2d_dc->CreateBitmapFromWicBitmap(m_wic_bitmap, &bitmap),
			L"创建位图失败"
		);

		m_d2d_bitmap = bitmap;
		return S_OK;
	}
}


