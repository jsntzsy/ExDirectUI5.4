/**
 * @file .cpp
 * @brief 实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024--
 * @copyright
 */


#include "stdafx.h"
#include "drawing/render/.h"
#include "src/drawing/render/factory.h"

namespace ExDirectUI
{

	HRESULT EXAPI EXCALL XXXX( )
	{
		CHECK_PARAM(SRC);
		CHECK_PARAM(RET);
		handle_if_false(g_drawing_render, EE_NOREADY, L"渲染引擎未就绪");

		handle_if_failed(
			g_drawing_render->CreateXXX(),
			L"创建XXX对象失败"
		);
		return S_OK;
	}

}
