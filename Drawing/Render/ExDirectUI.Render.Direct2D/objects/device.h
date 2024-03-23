﻿/**
 * @file device.h
 * @brief 渲染器设备对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-23
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExBitmapDeviceD2D : public ExObjectBaseImpl<IExDevice>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExDevice);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExBitmapDeviceD2D");
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return (void*)&m_mdc;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExBitmapDeviceD2D(uint32_t width, uint32_t height);
		virtual ~ExBitmapDeviceD2D();

		EXMETHOD ExDeviceType EXOBJCALL GetDeviceType() const override;
		EXMETHOD HANDLE EXOBJCALL GetObject() const override;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const override;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) override;

		EXMETHOD HRESULT EXOBJCALL Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc) override;

	private:
		ExMemDC m_mdc{};
	};

	/////////////////////
	
	class ExWindowDeviceD2D : public ExObjectBaseImpl<IExDevice>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExDevice);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExWindowDeviceD2D");
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return (void*)&m_window;
			case 1: return (void*)&m_ulwi;
			case 2: return (void*)&m_bf;
			case 3: return (void*)&m_src_point;
			case 4: return (void*)&m_size;
			default: return __super::GetContext(index);
			}
		}

	public:
		ExWindowDeviceD2D(HWND window);
		virtual ~ExWindowDeviceD2D();

		EXMETHOD ExDeviceType EXOBJCALL GetDeviceType() const override;
		EXMETHOD HANDLE EXOBJCALL GetObject() const override;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const override;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) override;

		EXMETHOD HRESULT EXOBJCALL Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc) override;

	private:
		HWND m_window{};
		UPDATELAYEREDWINDOWINFO m_ulwi{};
		BLENDFUNCTION m_bf{};
		ExPoint m_src_point{};
		SIZE m_size{};
	};

	//////////////////////
	
	
}