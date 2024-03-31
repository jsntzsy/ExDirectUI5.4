/**
 * @file HueRotateEffect.hpp
 * @brief 色调旋转效果器头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-31
 * @copyright
 */
#pragma once
#include "objects/effect.h"
#include "objects/canvas.h"
#include "render_api.h"
#include "atom.h"

namespace ExDirectUI
{
	EXENUM(ExHueRotateEffectParams)
	{
		Angle_F = 0,
	};

	class ExHueRotateEffectD2D : public ExEffectD2D
	{
	public:
		const static EXATOM EFFECT_ID = ATOM_HUEROTATEEFFECT;

		ExHueRotateEffectD2D()
		{
			throw_if_failed(
				GetCommonDeviceContext()->CreateEffect(_CLSID_D2D1HueRotation, &m_effect),
				L"创建色调旋转效果器失败"
			);
		}
		
		virtual HRESULT EXOBJCALL GetParam(int index, LPVOID param) const override
		{
			switch (index)
			{
			case ExHueRotateEffectParams::Angle_F:
				handle_if_failed(
					m_effect->GetValue(D2D1_HUEROTATION_PROP_ANGLE, (FLOAT*)(param)),
					L"获取色调旋转角度失败"
				);
				break;
			default: handle_ex(E_NOTIMPL, L"不支持的参数索引");
			}
			return S_OK;
		}
		
		virtual HRESULT EXOBJCALL SetParam(int index, LPVOID param) override
		{
			switch (index)
			{
			case ExHueRotateEffectParams::Angle_F:
				handle_if_failed(
					m_effect->SetValue(D2D1_HUEROTATION_PROP_ANGLE, *(FLOAT*)(param)),
					L"设置色调旋转角度失败"
				);
				break;
			default: handle_ex(E_NOTIMPL, L"不支持的参数索引");
			}
			return S_OK;
		}

	private:
		static HRESULT CALLBACK OnCreateEffect(EXATOM effect_id, const ExEffectInfo* info,
			LPARAM lparam, IExEffect** r_effect)
		{
			ExAutoPtr<ExHueRotateEffectD2D> effect = new ExHueRotateEffectD2D();			
			return effect->QueryInterface(r_effect);
		}

		inline static const ExEffectInfo EFFECT_INFO = {
			ATOM_HUEROTATEEFFECT,
			0x0000,
			OnCreateEffect,
			NULL,
			L"ExDirectUI.HueRotateEffect",
			
			L"0:Angle_F(色调旋转效果的旋转角度)\n"
		};
		
		friend class ExEffectD2D;
	};
	
}
