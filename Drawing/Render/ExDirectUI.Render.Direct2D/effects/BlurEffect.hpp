/**
 * @file BlurEffect.hpp
 * @brief 模糊效果器头文件
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
	EXENUM(ExBlurEffectParams)
	{
		BlurRadius_F = 0,
	};

	class ExBlurEffectD2D : public ExEffectD2D
	{
	public:

		ExBlurEffectD2D()
		{
			throw_if_failed(
				GetCommonDeviceContext()->CreateEffect(_CLSID_D2D1GaussianBlur, &m_effect),
				L"创建模糊效果器失败"
			);
		}

		virtual HRESULT EXOBJCALL GetParam(int index, LPVOID param) const override
		{
			switch (index)
			{
			case ExBlurEffectParams::BlurRadius_F:
				handle_if_failed(
					m_effect->GetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, (FLOAT*)param),
					L"获取模糊半径失败"
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
			case ExBlurEffectParams::BlurRadius_F: {
				FLOAT radius = *(FLOAT*)(param);
				handle_if_failed(
					m_effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, radius / 2.0F),
					L"设置模糊半径失败"
				);
				break;
			}break;
			default: handle_ex(E_NOTIMPL, L"不支持的参数索引");
			}
			return S_OK;
		}


	private:
		static HRESULT CALLBACK OnCreateEffect(EXATOM effect_id, const ExEffectInfo* info,
			LPARAM lparam, IExEffect** r_effect)
		{
			ExAutoPtr<ExBlurEffectD2D> effect = new ExBlurEffectD2D();
			return effect->QueryInterface(r_effect);
		}

		inline static const ExEffectInfo EFFECT_INFO = {
			ATOM_BLUREFFECT,
			0x0000,
			OnCreateEffect,
			NULL,
			L"ExDirectUI.BlurEffect",

			L"0:BlurRadius_F(模糊效果的模糊半径)\n"
		};

		friend class ExEffectD2D;
	};


}
