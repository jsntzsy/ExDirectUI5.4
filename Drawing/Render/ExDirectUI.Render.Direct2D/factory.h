/**
 * @file factory.h
 * @brief 渲染器工厂头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

#define ATOM_EXDIRECTUI_RENDER_DIRECT2D				0x7DE8373E		// 原子号: exdirectui.render.direct2d

namespace ExDirectUI
{
	class ExRenderD2D : public ExObjectBaseImpl<IExRender>,
		public ExLazySingleton<ExRenderD2D>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IExRender);
		EX_INTERFACE_ENTRY(IExModule);
		EX_INTERFACE_ENTRY(IUnknown);
		EX_END_INTERFACE_MAP();

	public:
		ExRenderD2D(IExModuleUtils* utils);
		virtual ~ExRenderD2D();

		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return L"ExRenderD2D: core: Direct2D/DirectWrite/ImageCodec";
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
				/*case 0: return m_pool;*/
			default: return __super::GetContext(index);
			}
		}

	public:
		EXMETHOD EXATOM EXOBJCALL GetId() const override { return MODULE_INFO.id; }
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
			return S_OK;
		}

	public:




	private:
		IExModuleUtils* m_utils;

		ExAutoPtr<ID2D1Factory> m_d2d_factory;
		ExAutoPtr<IDWriteFactory> m_dwrite_factory;
		ExAutoPtr<IWICImagingFactory> m_wic_factory;
		ExAutoPtr<ID2D1Device> m_d2d_device;
		ExAutoPtr<ID2D1DeviceContext> m_d2d_dc;

		friend inline IExModuleUtils* GetUtils();
		friend class ExImageD2D;
		friend class ExFontPoolD2D;
		friend class ExSolidBrushD2D;
		friend class ExLinearBrushD2D;
		friend class ExRadialBrushD2D;
		friend class ExImageBrushD2D;
		friend class ExCanvasBrushD2D;
		friend class ExPathD2D;
		friend class ExRegionD2D;
		friend class ExCompositionWindowDeviceD2D;
		friend class ExCanvasD2D;
		friend class ExEffectD2D;

		inline static const ExModuleInfo MODULE_INFO =
		{
			ATOM_EXDIRECTUI_RENDER_DIRECT2D,
			ExModuleType::Render,
			ExModuleFlags::None,
			0,
			L"ExDirectUI.Render.Direct2D",
			L"1.0.0.0",
			L"EternalZSY@ExDirectUI",
			L"ExDirectUI渲染器(内核：Direct2D，效率高且支持混合窗口，需要Win7SP1以上系统)"
		};
	};

	inline IExModuleUtils* GetUtils() { return ExRenderD2D::Instance()->m_utils; }
	inline ExRenderD2D* GetRender() { return ExRenderD2D::Instance(); }

}
