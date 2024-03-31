/**
 * @file 3DTransformEffect.hpp
 * @brief 3D变换效果器头文件
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
	EXENUM(Ex3DTransformEffectParams)
	{
		Matrix_M44 = 0,
	};

	class Ex3DTransformEffectD2D : public ExEffectD2D
	{
	public:
		const static EXATOM EFFECT_ID = ATOM_3DTRANSFORMEFFECT;

		Ex3DTransformEffectD2D()
		{
			throw_if_failed(
				GetCommonDeviceContext()->CreateEffect(_CLSID_D2D13DTransform, &m_effect),
				L"创建3D变换效果器失败"
			);
		}
		
		virtual HRESULT EXOBJCALL GetParam(int index, LPVOID param) const override
		{
			switch (index)
			{
			case Ex3DTransformEffectParams::Matrix_M44:
				handle_if_failed(
					m_effect->GetValue(D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX, (D2D1_MATRIX_4X4_F*)(param)),
					L"获取3D变换矩阵参数失败"
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
			case Ex3DTransformEffectParams::Matrix_M44:
				handle_if_failed(
					m_effect->SetValue(D2D1_3DTRANSFORM_PROP_TRANSFORM_MATRIX, *(D2D1_MATRIX_4X4_F*)(param)),
					L"设置3D变换矩阵参数失败"
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
			ExAutoPtr<Ex3DTransformEffectD2D> effect = new Ex3DTransformEffectD2D();
			return effect->QueryInterface(r_effect);
		}

		inline static const ExEffectInfo EFFECT_INFO = {
			ATOM_3DTRANSFORMEFFECT,
			0x0000,
			OnCreateEffect,
			NULL,
			L"ExDirectUI.3DTransformEffect",

			L"0:Matrix_M44(3D变换的4x4变换矩阵)\n"
		};

		friend class ExEffectD2D;
	};




}
