/**
 * @file region.h
 * @brief 渲染器区域头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	interface IExPath;

	/// 区域合并模式
	EXENUM(ExRegionCombineMode)
	{
		Copy = 0,				///< 区域合并模式：复制
		Union = 1,				///< 区域合并模式：合并
		Intersect = 2,			///< 区域合并模式：相交
		Exclude = 3,			///< 区域合并模式：排除
		Xor = 4,				///< 区域合并模式：异或
	};

	/// 渲染器区域接口
	EXINTERFACE("FDB71403-DFC2-4338-85D8-E0BA99294F1C") IExRegion : public IExRenderObject
	{
		EXMETHOD HRESULT EXOBJCALL CombineWithRect(float left, float top, float right, float bottom,
			ExRegionCombineMode mode, const ExMatrixElements * tranform = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL CombineWithPath(const IExPath* path, ExRegionCombineMode mode,
			const ExMatrixElements* tranform = nullptr) PURE;
		EXMETHOD HRESULT EXOBJCALL CombineWithRegion(const IExRegion* region, ExRegionCombineMode mode,
			const ExMatrixElements* tranform = nullptr) PURE;

		EXMETHOD HRESULT EXOBJCALL HitTest(float x, float y) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetBounds(ExRectF* r_bounds_rect) const PURE;
	};

	////////////////

	//HRESULT EXAPI EXCALL ExRegionCreate(IExRegion** r_region, bool infinite = false);
	//HRESULT EXAPI EXCALL ExRegionCreateFromRect(float left, float top, float right, float bottom, IExRegion** r_region);
	//HRESULT EXAPI EXCALL ExRegionCreateFromPath(const IExPath* path, const ExMatrixElements* tranform, IExRegion** r_region);
	//HRESULT EXAPI EXCALL ExRegionCopy(const IExRegion* region, const ExMatrixElements* tranform, IExRegion** r_region);

}
