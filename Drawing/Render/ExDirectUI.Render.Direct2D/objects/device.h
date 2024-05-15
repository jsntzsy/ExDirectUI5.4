/**
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
	class ExBitmapDeviceD2D : public ExObjectImpl<IExDevice>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExDevice);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD BSTR EXOBJCALL ToString() const override
		{
			return ExSysAllocStringFormat(L"ExBitmapDeviceD2D(size: %ux%u)", m_mdc.width, m_mdc.height);
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

	class ExWindowDeviceD2D : public ExObjectImpl<IExDevice>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExDevice);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD BSTR EXOBJCALL ToString() const override
		{
			return ExSysAllocStringFormat(
				L"ExWindowDeviceD2D(window: 0x%p, size: %ux%u)",
					m_window, m_size.cx, m_size.cy
			);
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

	class ExCompositionWindowDeviceD2D : public ExObjectImpl<IExDevice>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExDevice);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD BSTR EXOBJCALL ToString() const override
		{
			return ExSysAllocStringFormat(
				L"ExCompositionWindowDeviceD2D(window: 0x%p, size: %ux%u)",
				m_window, m_size.cx, m_size.cy
			);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_device; 
			case 1: return m_target; 
			case 2: return m_visual; 
			case 3: return m_surface; 
			default: return __super::GetContext(index);
			}
		}

	public:
		ExCompositionWindowDeviceD2D(HWND window);
		virtual ~ExCompositionWindowDeviceD2D();

		EXMETHOD ExDeviceType EXOBJCALL GetDeviceType() const override;
		EXMETHOD HANDLE EXOBJCALL GetObject() const override;

		EXMETHOD HRESULT EXOBJCALL GetSize(uint32_t* r_width, uint32_t* r_height) const override;
		EXMETHOD HRESULT EXOBJCALL Resize(uint32_t width, uint32_t height) override;

		EXMETHOD HRESULT EXOBJCALL Update(IExCanvas* canvas_src, ExRect* update_rect, int alpha, HDC dest_dc) override;

	private:
		HWND m_window{};
		SIZE m_size{};
		ExAutoPtr<IDCompositionDesktopDevice> m_device;
		ExAutoPtr<IDCompositionTarget> m_target;
		ExAutoPtr<IDCompositionVisual2> m_visual;
		ExAutoPtr<IDCompositionSurface> m_surface;
	};

	/////////////////
	
	IExDevice* EXCALL _ExDevice_CreateWindowDevice(HWND window, bool composition) MAYTHROW;
	
}
