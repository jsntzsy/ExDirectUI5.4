/**
 * @file render.h
 * @brief 渲染器头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once


namespace ExDirectUI
{
	/// 渲染器对象接口
	EXINTERFACE("8B95839B-26A1-4D09-AB2B-3127CEB4266C") IExRenderObject : public IExObject
	{
	};
}

#include "render/matrix.h"
#include "render/image.h"
#include "render/font.h"
#include "render/path.h"
#include "render/region.h"
#include "render/brush.h"
#include "render/pen.h"
#include "render/solid_brush.h"
#include "render/linear_brush.h"
#include "render/radial_brush.h"
#include "render/image_brush.h"
#include "render/canvas_brush.h"
#include "render/effect.h"
#include "render/mem_dc.h"
#include "render/device.h"
#include "render/canvas.h"
#include "render/factory.h"

