/**
 * @file brush.h
 * @brief 渲染器画刷头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 画刷扩展模式
	EXENUM(ExBrushExtendMode)
	{
		None = 0,				///< 画刷扩展模式：不扩展
		Tile = 1,				///< 画刷扩展模式：平铺
		Mirror = 2,				///< 画刷扩展模式：镜像平铺
	};

	/// 渲染渐变点信息
	struct ExGradientPoint
	{
		/// 渐变点位置 <0-1>
		float pos;
		
		/// 渐变点颜色
		EXARGB color;
	};

	/// 渲染器画刷接口
	EXINTERFACE("6AECD323-B229-4B26-A741-6E57EB82BF3C") IExBrush : public IExRenderObject
	{
		EXMETHOD HRESULT EXOBJCALL GetTransform(ExMatrixElements* r_tranform) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetTransform(const ExMatrixElements * tranform) PURE;

		EXMETHOD HRESULT EXOBJCALL GetExtendMode(ExBrushExtendMode* r_mode) const PURE;
		EXMETHOD HRESULT EXOBJCALL SetExtendMode(ExBrushExtendMode mode) PURE;
	};

}
