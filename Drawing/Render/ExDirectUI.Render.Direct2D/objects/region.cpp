/**
 * @file region.cpp
 * @brief 渲染器区域实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-21
 * @copyright
 */

#include "stdafx.h"
#include "objects/region.h"
#include "objects/path.h"

namespace ExDirectUI
{
	ExRegionD2D::ExRegionD2D(bool infinity)
	{
		D2D1_RECT_F rc;
		if (infinity) { rc = D2D1::InfiniteRect(); }
		else { rc = D2D1::RectF(0, 0, 0, 0); }

		ExAutoPtr<ID2D1RectangleGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateRectangleGeometry(rc, &geometry),
			L"区域几何形创建失败"
		);

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::ExRegionD2D(float left, float top, float right, float bottom, bool is_clip)
	{
		_offset_(false, left, top, right, bottom);
		D2D1_RECT_F rc = D2D1Rect(left, top, right, bottom);

		ExAutoPtr<ID2D1RectangleGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateRectangleGeometry(rc, &geometry),
			L"区域几何形创建失败"
		);

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
		m_is_clip = is_clip;
	}
	ExRegionD2D::ExRegionD2D(float left, float top, float right, float bottom)
	{
		_offset_(false, left, top, right, bottom);
		ExRectF rc = ExRectF(left, top, right, bottom).Normalize();

		ExAutoPtr<ID2D1EllipseGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateEllipseGeometry(
				D2D1::Ellipse(
					D2D1::Point2F((rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2),
					rc.Width() / 2, rc.Height() / 2
				), &geometry
			), L"区域几何形创建失败"
		);

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::ExRegionD2D(float left, float top, float right, float bottom,
		float radius_left_top, float radius_right_top,
		float radius_right_bottom, float radius_left_bottom)
	{
		ExAutoPtr<ID2D1PathGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreatePathGeometry(&geometry),
			L"区域几何形创建失败"
		);

		ExAutoPtr<ID2D1GeometrySink> sink;
		throw_if_failed(geometry->Open(&sink), L"获取路径描述器失败");

		ExPathD2D::MakeRoundedRect(sink, left, top, right, bottom,
			radius_left_top, radius_right_top,
			radius_right_bottom, radius_left_bottom
		);

		throw_if_failed(sink->Close(), L"关闭路径描述器失败");
		sink.Release();

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::ExRegionD2D(const IExPath* path, const ExMatrixElements* tranform)
	{
		D2D1_MATRIX_3X2_F matrix = Matrix(tranform);

		ExAutoPtr<ID2D1TransformedGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateTransformedGeometry(
				((ExPathD2D*)path)->m_geometry,
				&matrix, &geometry
			), L"区域几何形创建失败"
		);

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::~ExRegionD2D()
	{
	}
	HRESULT EXOBJCALL ExRegionD2D::CombineWithRect(float left, float top, float right, float bottom,
		ExRegionCombineMode mode, const ExMatrixElements* tranform)
	{
		_offset_(left, top, right, bottom);
		D2D1_RECT_F rc = D2D1Rect(left, top, right, bottom);

		try
		{
			ExAutoPtr<ID2D1RectangleGeometry> geometry;
			throw_if_failed(
				GetRender()->m_d2d_factory->CreateRectangleGeometry(rc, &geometry),
				L"源区域几何形创建失败"
			);

			m_geometry = Combine(m_geometry, geometry, mode, tranform);
			return S_OK;
		}catch_default({});
	}
	HRESULT EXOBJCALL ExRegionD2D::CombineWithPath(const IExPath* path, ExRegionCombineMode mode, const ExMatrixElements* tranform)
	{
		CHECK_PARAM(path);

		try
		{
			m_geometry = Combine(m_geometry, ((ExPathD2D*)path)->m_geometry, mode, tranform);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRegionD2D::CombineWithRegion(const IExRegion* region, ExRegionCombineMode mode, const ExMatrixElements* tranform)
	{
		CHECK_PARAM(region);

		try
		{
			m_geometry = Combine(m_geometry, ((ExRegionD2D*)region)->m_geometry, mode, tranform);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRegionD2D::HitTest(float x, float y) const
	{
		_offset_(false, x, y);

		BOOL hit = FALSE;
		throw_if_failed(
			m_geometry->FillContainsPoint(D2D1::Point2F(x, y), nullptr, &hit),
			L"检测点是否在区域内失败"
		);

		return hit ? S_OK : S_FALSE;
	}
	HRESULT EXOBJCALL ExRegionD2D::GetBounds(ExRectF* r_bounds_rect) const
	{
		CHECK_PARAM(r_bounds_rect);

		D2D1_RECT_F rc;
		handle_if_failed(m_geometry->GetBounds(D2D1::Matrix3x2F::Identity(), &rc), L"获取边界矩形失败");
		
		*r_bounds_rect = ExRectF(rc.left, rc.top, rc.right, rc.bottom);
		return S_OK;
	}

	ID2D1Geometry* EXOBJCALL ExRegionD2D::Combine(ID2D1Geometry* geometry1, ID2D1Geometry* geometry2,
		ExRegionCombineMode mode, const ExMatrixElements* tranform_matrix)
	{
		ExAutoPtr<ID2D1PathGeometry> new_geometry;

		//获取合并模式
		D2D1_COMBINE_MODE combine_mode{};
		switch (mode)
		{
		case ExRegionCombineMode::Copy: break;
		case ExRegionCombineMode::Union: combine_mode = D2D1_COMBINE_MODE_UNION; break;
		case ExRegionCombineMode::Intersect: combine_mode = D2D1_COMBINE_MODE_INTERSECT; break;
		case ExRegionCombineMode::Exclude: combine_mode = D2D1_COMBINE_MODE_EXCLUDE; break;
		case ExRegionCombineMode::Xor: combine_mode = D2D1_COMBINE_MODE_XOR; break;
		default: throw_ex(E_NOTIMPL, L"不支持该合并模式");
		}

		//获取变换矩阵
		D2D1_MATRIX_3X2_F matrix = Matrix(tranform_matrix);

		//拷贝则直接替换
		if (mode == ExRegionCombineMode::Copy)
		{
			//则创建一个变换后的几何形
			ExAutoPtr<ID2D1TransformedGeometry> new_geometry;
			throw_if_failed(
				GetRender()->m_d2d_factory->CreateTransformedGeometry(
					geometry1, matrix, &new_geometry
				), L"创建新几何形对象失败"
			);

			return new_geometry.Detach();
		}
		else
		{
			//创建目标几何形
			throw_if_failed(
				GetRender()->m_d2d_factory->CreatePathGeometry(&new_geometry),
				L"创建新几何形对象失败"
			);

			//开始描述几何形
			ExAutoPtr<ID2D1GeometrySink> sink;
			throw_if_failed(new_geometry->Open(&sink), L"开始描述几何形失败");

			//合并
			throw_if_failed(
				geometry1->CombineWithGeometry(geometry2, combine_mode,
					matrix, sink),
				L"合并几何形失败"
			);

			//扫尾
			throw_if_failed(sink->Close(), L"描述几何形对象错误");
			sink.Release();

			return new_geometry.Detach();
		}
	}
}


