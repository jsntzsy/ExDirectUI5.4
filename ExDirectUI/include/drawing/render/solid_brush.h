/**
 * @file solid_brush.h
 * @brief 渲染器纯色画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once
#include "brush.h"

namespace ExDirectUI
{
	EXINTERFACE("5AA1C56B-9913-4D4E-842E-24371B4719F0") IExSolidBrush : public IExBrush
	{
		EXMETHOD EXARGB EXOBJCALL GetColor() const PURE;
		EXMETHOD HRESULT EXOBJCALL SetColor(EXARGB color) PURE;
	};

	/////////////////////

	/**
	 * @brief 纯色画刷_创建
	 * 创建纯色画刷对象
	 * @param color 画刷颜色
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExSolidBrushCreate(EXARGB color, IExSolidBrush** r_brush);

}
