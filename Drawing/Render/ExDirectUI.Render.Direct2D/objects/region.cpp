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
		//确定几何形边界矩形
		D2D1_RECT_F rc;
		if (infinity) { rc = D2D1::InfiniteRect(); }
		else { rc = D2D1::RectF(0, 0, 0, 0); }

		//创建几何形
		ExAutoPtr<ID2D1RectangleGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateRectangleGeometry(rc, &geometry),
			L"区域几何形创建失败"
		);

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::ExRegionD2D(float left, float top, float right, float bottom, bool is_clip)
	{
		//确定矩形边界
		_offset_(false, left, top, right, bottom);
		D2D1_RECT_F rc = D2D1Rect(left, top, right, bottom);

		//创建矩形几何形
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
		//确定椭圆边界
		_offset_(false, left, top, right, bottom);
		ExRectF rc = ExRectF(left, top, right, bottom).Normalize();

		//创建椭圆几何形
		ExAutoPtr<ID2D1EllipseGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateEllipseGeometry(
				D2D1::Ellipse(
					D2D1::Point2F(rc.GetHorzCenter(), rc.GetVertCenter()),
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
		//创建路径几何形
		ExAutoPtr<ID2D1PathGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreatePathGeometry(&geometry),
			L"区域几何形创建失败"
		);

		//开始描述
		ExAutoPtr<ID2D1GeometrySink> sink;
		throw_if_failed(geometry->Open(&sink), L"获取路径描述器失败");

		//生成圆角矩形
		ExPathD2D::MakeRoundRectFigure(sink, left, top, right, bottom,
			radius_left_top, radius_right_top,
			radius_right_bottom, radius_left_bottom
		);

		//结束描述
		throw_if_failed(sink->Close(), L"关闭路径描述器失败");
		sink.Release();
		
		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::ExRegionD2D(const IExPath* path, const ExMatrixElements* tranform)
	{
		D2D1_MATRIX_3X2_F matrix = Matrix(tranform);

		//生成变换几何形
		ExAutoPtr<ID2D1TransformedGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateTransformedGeometry(
				((ExPathD2D*)path)->m_geometry,
				&matrix, &geometry
			), L"区域几何形创建失败"
		);

		throw_if_failed(geometry->QueryInterface(&m_geometry), L"查询接口失败");
	}
	ExRegionD2D::ExRegionD2D(const IExRegion* region, const ExMatrixElements* tranform)
	{
		D2D1_MATRIX_3X2_F matrix = Matrix(tranform);

		ExAutoPtr<ID2D1TransformedGeometry> geometry;
		throw_if_failed(
			GetRender()->m_d2d_factory->CreateTransformedGeometry(
				((ExRegionD2D*)region)->m_geometry,
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
		//确定矩形边界
		_offset_(left, top, right, bottom);
		D2D1_RECT_F rc = D2D1Rect(left, top, right, bottom);

		try
		{
			//创建矩形几何形
			ExAutoPtr<ID2D1RectangleGeometry> geometry;
			throw_if_failed(
				GetRender()->m_d2d_factory->CreateRectangleGeometry(rc, &geometry),
				L"源区域几何形创建失败"
			);

			//合并并覆盖当前几何形
			m_geometry = Combine(m_geometry, geometry, mode, tranform);
			return S_OK;
		}catch_default({});
	}
	HRESULT EXOBJCALL ExRegionD2D::CombineWithPath(const IExPath* path, ExRegionCombineMode mode, const ExMatrixElements* tranform)
	{
		CHECK_PARAM(path);

		try
		{
			//合并并覆盖当前几何形
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
			//合并并覆盖当前几何形
			m_geometry = Combine(m_geometry, ((ExRegionD2D*)region)->m_geometry, mode, tranform);
			return S_OK;
		}
		catch_default({});
	}
	HRESULT EXOBJCALL ExRegionD2D::HitTest(float x, float y) const
	{
		//偏移坐标
		_offset_(false, x, y);

		//命中测试
		BOOL hit = FALSE;
		handle_if_failed(
			m_geometry->FillContainsPoint(D2D1::Point2F(x, y), nullptr, &hit),
			L"检测点是否在区域内失败"
		);

		return hit ? S_OK : S_FALSE;
	}
	HRESULT EXOBJCALL ExRegionD2D::GetBounds(ExRectF* r_bounds_rect) const
	{
		CHECK_PARAM(r_bounds_rect);

		//获取边界
		D2D1_RECT_F rc;
		handle_if_failed(m_geometry->GetBounds(D2D1::Matrix3x2F::Identity(), &rc), L"获取边界矩形失败");
		
		//偏移回去并返回
		_offset_(true, rc);
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

			//将新几何形直接返回
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

			//将新几何形直接返回
			return new_geometry.Detach();
		}
	}
}


