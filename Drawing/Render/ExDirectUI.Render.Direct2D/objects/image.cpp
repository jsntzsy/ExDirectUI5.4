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

namespace ExDirectUI
{
	ExImageD2D::ExImageD2D(uint32_t width, uint32_t height)
	{
	}
	ExImageD2D::ExImageD2D(LPCWSTR file)
	{
	}
	ExImageD2D::ExImageD2D(const byte_t* data, size_t size)
	{
	}
	ExImageD2D::ExImageD2D(uint32_t width, uint32_t height, const EXBITSDATA bits_data, uint32_t stride)
	{
	}
	ExImageD2D::ExImageD2D(const byte_t* image_data)
	{
	}
	ExImageD2D::ExImageD2D(HBITMAP bitmap, HPALETTE palette, bool pre_alpha)
	{
	}
	ExImageD2D::ExImageD2D(HICON icon)
	{
	}
	ExImageD2D::ExImageD2D(IWICBitmap* src_bitmap)
	{
	}
	ExImageD2D::~ExImageD2D()
	{
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
		//单帧图像直接返回
		if (!m_decode_image) { return E_NOTIMPL; }

		//检查帧索引
		handle_if_false(index < m_decode_image->GetFrameCount(), EE_OUTOFBOUNDS, L"帧索引越界");

		//判断是否锁着
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像已被锁定");
		
		//锁定当前帧


	}
	HRESULT EXOBJCALL ExImageD2D::NextFrame(uint32_t* r_next_index, uint32_t* r_next_delay)
	{
		//单帧图像直接返回
		if(!m_decode_image) { return S_FALSE; }

		//获取下一帧索引
		uint32_t next_index = (m_cur_frame + 1) % m_decode_image->GetFrameCount();
		handle_if_failed(SetCurFrame(next_index), L"设置当前帧失败");
		if(r_next_index){ *r_next_index = next_index; }

		//获取下一帧延迟
		if(r_next_delay){ *r_next_delay = GetFrameDelay(); }

		return S_OK;
	}
	uint32_t EXOBJCALL ExImageD2D::GetFrameDelay() const
	{
		if (!m_decode_image) { return 0; }
		uint32_t delay = 0;
		m_decode_image->GetFrameDelay(m_cur_frame, &delay);
		return delay;
	}
	HRESULT EXOBJCALL ExImageD2D::Lock(ExImageLock* r_lock)
	{
		CHECK_PARAM(r_lock);
		handle_if_false(m_bitmap_lock == nullptr, EE_NOREADY, L"图像已被锁定");

		//锁定图像
		WICRect rc = { 0, 0, m_width, m_height };
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
	HRESULT EXOBJCALL ExImageD2D::Unlock(const ExImageLock* lock)
	{
		CHECK_PARAM(lock);
		handle_if_false(m_bitmap_lock != nullptr, EE_NOREADY, L"图像未被锁定");
		handle_if_false(lock->reserved == (uintptr_t)m_bitmap_lock.Get(), E_INVALIDARG, L"图像锁信息不匹配");

		//解锁图像
		m_bitmap_lock.Release();
		return S_OK;
	}
	HRESULT EXOBJCALL ExImageD2D::GetPixel(uint32_t x, uint32_t y, uint32_t* r_color)
	{
		return  HRESULT EXOBJCALL();
	}
	HRESULT EXOBJCALL ExImageD2D::SetPixel(uint32_t x, uint32_t y, uint32_t color)
	{
		return  HRESULT EXOBJCALL();
	}
	HRESULT EXOBJCALL ExImageD2D::Copy(IExImage* r_dest, const ExRect* copy_rect)
	{
		return  HRESULT EXOBJCALL();
	}
	HRESULT EXOBJCALL ExImageD2D::Scale(uint32_t new_width, uint32_t new_height, IExImage* r_dest, const ExRect* copy_rect)
	{
		return  HRESULT EXOBJCALL();
	}
	HRESULT EXOBJCALL ExImageD2D::Save(ExData* r_data)
	{
		return  HRESULT EXOBJCALL();
	}
	HRESULT EXOBJCALL ExImageD2D::Flush()
	{
		return HRESULT EXOBJCALL();
	}
}


