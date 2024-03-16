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
	
	//HRESULT EXAPI EXCALL ExSolidBrushCreate(EXARGB color, IExSolidBrush** r_brush);
	
}
