/**
 * @file path.cpp
 * @brief 渲染器路径实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-07
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/path.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExPathCreate(IExPath** r_path)
	{
		CHECK_PARAM(r_path);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");
		
		return g_drawing_render->CreatePath(r_path);
	}

}
