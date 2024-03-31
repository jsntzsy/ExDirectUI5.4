/**
 * @file 3DPerspectiveEffect.hpp
 * @brief 3D透视效果器头文件
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
	EXENUM(Ex3DPerspectiveEffectParams)
	{
		PerspectiveOrigin_V2 = 0,
		RotateAngle_V3 = 1,
		RotateOrigin_V3 = 2,
		Depth_F = 3,
	};

	class Ex3DPerspectiveEffectD2D : public ExEffectD2D
	{
	public:
		const static EXATOM EFFECT_ID = ATOM_3DPERSPECTIVEEFFECT;
		
		Ex3DPerspectiveEffectD2D()
		{
			throw_if_failed(
				GetCommonDeviceContext()->CreateEffect(_CLSID_D2D13DPerspectiveTransform, &m_effect),
				L"创建3D透视效果器失败"
			);
		}

		virtual HRESULT EXOBJCALL GetParam(int index, LPVOID param) const override
		{
			HRESULT hr;
			switch (index)
			{
			case Ex3DPerspectiveEffectParams::PerspectiveOrigin_V2:
				hr = m_effect->GetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_PERSPECTIVE_ORIGIN, (D2D1_VECTOR_2F*)(param));
				break;
			case Ex3DPerspectiveEffectParams::RotateAngle_V3:
				hr = m_effect->GetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_ROTATION, (D2D1_VECTOR_3F*)(param));
				break;
			case Ex3DPerspectiveEffectParams::RotateOrigin_V3:
				hr = m_effect->GetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_ROTATION_ORIGIN, (D2D1_VECTOR_3F*)(param));
				break;
			case Ex3DPerspectiveEffectParams::Depth_F:
				hr = m_effect->GetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_DEPTH, (FLOAT*)(param));
				break;
			default: handle_ex(E_NOTIMPL, L"不支持的参数索引");
			}
			handle_if_failed(hr, L"获取参数失败");
			return hr;
		}
		
		virtual HRESULT EXOBJCALL SetParam(int index, LPVOID param) override
		{
			HRESULT hr;
			switch (index)
			{
			case Ex3DPerspectiveEffectParams::PerspectiveOrigin_V2:
				hr = m_effect->SetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_PERSPECTIVE_ORIGIN, *(D2D1_VECTOR_2F*)(param));
				break;
			case Ex3DPerspectiveEffectParams::RotateAngle_V3:
				hr = m_effect->SetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_ROTATION, *(D2D1_VECTOR_3F*)(param));
				break;
			case Ex3DPerspectiveEffectParams::RotateOrigin_V3:
				hr = m_effect->SetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_ROTATION_ORIGIN, *(D2D1_VECTOR_3F*)(param));
				break;
			case Ex3DPerspectiveEffectParams::Depth_F:
				hr = m_effect->SetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_DEPTH, *(FLOAT*)(param));
				break;
			default: handle_ex(E_NOTIMPL, L"不支持的参数索引");
			}
			handle_if_failed(hr, L"设置参数失败");
			return hr;
		}

	private:
		static HRESULT CALLBACK OnCreateEffect(EXATOM effect_id, const ExEffectInfo* info,
			LPARAM lparam, IExEffect** r_effect)
		{
			ExAutoPtr<Ex3DPerspectiveEffectD2D> effect = new Ex3DPerspectiveEffectD2D();
			return effect->QueryInterface(r_effect);
		}

		inline static const ExEffectInfo EFFECT_INFO = {
			ATOM_3DPERSPECTIVEEFFECT,
			0x0000,
			OnCreateEffect,
			NULL,
			L"ExDirectUI.3DPerspectiveEffect",
			
			L"0:PerspectiveOrigin_V2(3D透视变换的基点)\n"
			L"1:RotateAngle_V3(3D透视变换的三个轴的旋转角度)\n"
			L"2:RotateOrigin_V3(3D透视变换的旋转中心点)\n"
			L"3:Depth_F(3D透视变换的透视深度)\n"
		};

		friend class ExEffectD2D;
	};




}
