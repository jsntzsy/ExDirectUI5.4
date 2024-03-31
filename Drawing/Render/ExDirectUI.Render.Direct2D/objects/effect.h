/**
 * @file effect.h
 * @brief 渲染器效果头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	class ExCanvasD2D;
	class ExEffectD2D : public ExObjectImpl<IExEffect>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExEffect);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExEffect()");
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_effect.Get();
			case 1: return m_source.Get();
				
			default: return __super::GetContext(index);
			}
		}

	public:
		ExEffectD2D();
		virtual EXOBJCALL ~ExEffectD2D();

		EXMETHOD bool EXOBJCALL HaveSource() override;
		EXMETHOD bool EXOBJCALL CanBeSource() override;
		EXMETHOD HRESULT EXOBJCALL SetSource(IExRenderObject* source, const ExRectF* src_rect = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL SetSourceFromTarget(IExCanvasTarget* target, const ExRectF* src_rect = nullptr) override;
		
		EXMETHOD HRESULT EXOBJCALL OnDraw(IExCanvas* canvas, float left, float top, LPARAM lparam) override;

	protected:
		HRESULT EXOBJCALL OnDrawBitmap(ExCanvasD2D* canvas, ID2D1DeviceContext* dc,
			float left, float top, LPARAM lparam);

	protected:
		ExAutoPtr<ID2D1Effect> m_effect;
		ExAutoPtr<IUnknown> m_source;

	protected:
		static HRESULT GetSourceRect(IExRenderObject* src, const ExRectF* src_rect, D2D1_RECT_F& real_rect);
		static HRESULT GetSourceRect(IExCanvasTarget* src, const ExRectF* src_rect, D2D1_RECT_F& real_rect);
		static ID2D1DeviceContext* GetCommonDeviceContext();

	private:
		static HRESULT RegisterEffects(ExRenderD2D* render);
		static std::unordered_map<EXATOM, ExEffectCreateProc> s_effects;

		friend class ExCanvasD2D;
		friend class ExRenderD2D;

	};

}
