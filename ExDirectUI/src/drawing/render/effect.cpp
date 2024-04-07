/**
 * @file effect.cpp
 * @brief 渲染器效果对象实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-07
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/effect.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExEffectRegister(const ExEffectInfo* effect_info)
	{
		CHECK_PARAM(effect_info);
		CHECK_PARAM(effect_info->id != EXATOM_UNKNOWN);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->RegisterEffect(effect_info);
	}

	HRESULT EXAPI EXCALL ExEffectUnRegister(EXATOM effect_id)
	{
		CHECK_PARAM(effect_id != EXATOM_UNKNOWN);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->UnRegisterEffect(effect_id);
	}

	HRESULT EXAPI EXCALL ExEffectIsSupport(EXATOM effect_id)
	{
		CHECK_PARAM(effect_id != EXATOM_UNKNOWN);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->IsSupportEffect(effect_id) ? S_OK : S_FALSE;
	}

	HRESULT EXAPI EXCALL ExEffectGetInfo(EXATOM effect_id, ExEffectInfo* const r_effect_info)
	{
		CHECK_PARAM(effect_id != EXATOM_UNKNOWN);
		CHECK_PARAM(r_effect_info);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->GetEffectInfo(effect_id, r_effect_info);
	}

	HRESULT EXAPI EXCALL ExEffectCreate(EXATOM effect_id, LPARAM lparam, IExEffect** r_effect)
	{
		CHECK_PARAM(effect_id != EXATOM_UNKNOWN);
		CHECK_PARAM(r_effect);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateEffect(effect_id, lparam, r_effect);
	}

	HRESULT EXAPI EXCALL ExEffectCreateByName(LPCWSTR effect_name, LPARAM lparam, IExEffect** r_effect)
	{
		CHECK_PARAM(effect_name);
		CHECK_PARAM(r_effect);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		return g_drawing_render->CreateEffectByName(effect_name, lparam, r_effect);
	}

}
