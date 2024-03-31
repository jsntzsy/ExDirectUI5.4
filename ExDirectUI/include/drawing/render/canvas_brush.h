/**
 * @file canvas_brush.h
 * @brief 渲染器画布画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	EXINTERFACE("59383874-96F6-46DF-9C03-43A1F41EE289") IExCanvasBrush : public IExBrush
	{
	};

	///////////////////////////////////

	interface IExCanvas;
	interface IExCanvasTarget;
	
	/**
	 * @brief 画布画刷_创建
	 * 通过画布对象创建画布画刷
	 * @param canvas 源画布对象
	 * @param extend_mode 画刷扩展模式
	 * @param alpha 画刷透明度
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExCanvasBrushCreate(const IExCanvas* canvas, ExBrushExtendMode extend_mode,
		EXCHANNEL alpha, IExCanvasBrush** r_brush);
	
	/**
	 * @brief 画布画刷_创建自目标
	 * 通过画布目标创建画布画刷对象
	 * @param target 源画布目标对象
	 * @param extend_mode 画刷扩展模式
	 * @param alpha 画刷透明度
	 * @param r_brush 接收创建的画刷对象
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExCanvasBrushCreateFromTarget(const IExCanvasTarget* target,
		ExBrushExtendMode extend_mode, EXCHANNEL alpha, IExCanvasBrush** r_brush);
	
}
