/**
 * @file region.h
 * @brief 渲染器区域头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */
#pragma once

namespace ExDirectUI
{

	class ExRegionD2D : public ExObjectBaseImpl<IExRegion>
	{
	public:
		EX_BEGIN_INTERFACE_MAP();
		EX_INTERFACE_ENTRY(IUnknown);
		EX_INTERFACE_ENTRY(IExObject);
		EX_INTERFACE_ENTRY(IExRenderObject);
		EX_INTERFACE_ENTRY(IExRegion);
		EX_END_INTERFACE_MAP();

	public:
		EXMETHOD std::wstring EXOBJCALL ToString() const override
		{
			return ExString::format(L"ExRegion: 0x%p", this);
		}
		EXMETHOD void* EXOBJCALL GetContext(int index) const override
		{
			switch (index)
			{
			case 0: return m_geometry;
			default: return __super::GetContext(index);
			}
		}

	public:

		ExRegionD2D(bool infinity);
		ExRegionD2D(float left, float top, float right, float bottom, bool is_clip);
		ExRegionD2D(float left, float top, float right, float bottom);
		ExRegionD2D(float left, float top, float right, float bottom, 
			float radius_left_top,  float radius_right_top, 
			float radius_right_bottom, float radius_left_bottom);
		ExRegionD2D(const IExPath* path, const ExMatrixElements* tranform);

		virtual ~ExRegionD2D();


		EXMETHOD HRESULT EXOBJCALL CombineWithRect(float left, float top, float right, float bottom,
			ExRegionCombineMode mode, const ExMatrixElements* tranform = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL CombineWithPath(const IExPath* path, ExRegionCombineMode mode,
			const ExMatrixElements* tranform = nullptr) override;
		EXMETHOD HRESULT EXOBJCALL CombineWithRegion(const IExRegion* region, ExRegionCombineMode mode,
			const ExMatrixElements* tranform = nullptr) override;

		EXMETHOD HRESULT EXOBJCALL HitTest(float x, float y) const override;
		EXMETHOD HRESULT EXOBJCALL GetBounds(ExRectF* r_bounds_rect) const override;

	private:
		static ID2D1Geometry* EXOBJCALL Combine(ID2D1Geometry* geometry1, ID2D1Geometry* geometry2,
			ExRegionCombineMode mode, const ExMatrixElements* tranform_matrix) MAYTHROW;

		ExAutoPtr<ID2D1Geometry> m_geometry{};
		bool m_is_clip{ false };
	};

}
