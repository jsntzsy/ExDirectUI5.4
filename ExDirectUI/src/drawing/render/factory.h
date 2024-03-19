/**
 * @file factory.h
 * @brief 渲染器工厂头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	extern IExRender* g_drawing_render;

	HRESULT EXCALL _ExRender_Group(IExRender* render);
	HRESULT EXCALL _ExRender_UnGroup(IExRender* render);
}

