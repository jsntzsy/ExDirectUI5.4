/**
 * @file stdafx.h
 * @brief 预编译头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-19
 * @copyright
 */
#pragma once

#include <d2d1_1.h>
#include <dwrite.h>
#include <d3d11.h>
#include <wincodec.h>
#include <dcomp.h>

#include "ExDirectUI.h"
#pragma comment(lib,"ExDirectUI.lib")
using namespace ExDirectUI;

#include "common/object_impl.hpp"
#include "common/singleton.hpp"
#include "common/string.hpp"

#include "factory.h"



namespace ExDirectUI
{
	inline bool IsIdentityMatrix(const D2D1_MATRIX_3X2_F& matrix)
	{
		return matrix._11 == 1.0F && matrix._12 == 0.0F &&
			matrix._21 == 0.0F && matrix._22 == 1.0F &&
			matrix._31 == 0.0F && matrix._32 == 0.0F;
	}

	inline D2D1_MATRIX_3X2_F Matrix(const ExMatrixElements3x2& matrix)
	{
		return D2D1::Matrix3x2F(
			matrix._11, matrix._12,
			matrix._21, matrix._22,
			matrix._31, matrix._32
		);
	}
	inline D2D1_MATRIX_3X2_F Matrix(const ExMatrixElements3x2* matrix)
	{
		if (matrix == nullptr) { return D2D1::Matrix3x2F::Identity(); }
		return D2D1::Matrix3x2F{
			matrix->_11, matrix->_12,
			matrix->_21, matrix->_22,
			matrix->_31, matrix->_32
		};
	}
	inline ExMatrixElements3x2 Matrix(const D2D1_MATRIX_3X2_F& matrix)
	{
		return ExMatrixElements3x2{
			matrix._11, matrix._12,
			matrix._21, matrix._22,
			matrix._31, matrix._32
		};
	}

	inline D2D1_RECT_F Rect(const ExRectF& rect)
	{
		return D2D1::RectF(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	}
	inline ExRectF Rect(const D2D1_RECT_F& rect)
	{
		return ExRectF(rect.left, rect.top, rect.right, rect.bottom);
	}
	inline D2D1_RECT_F D2D1Rect(float left, float top, float right, float bottom)
	{
		return D2D1::RectF(
			min(left, right), min(top, bottom),
			max(left, right), max(top, bottom)
		);
	}

	inline D2D1_COLOR_F Color(EXARGB argb)
	{
		EXCHANNEL* ch = (EXCHANNEL*)&argb;
		return D2D1::ColorF(ch[2] / 255.0F, ch[1] / 255.0F, ch[0] / 255.0F, ch[3] / 255.0F);
	}
	inline EXARGB Color(const D2D1_COLOR_F& color)
	{
		return MAKEARGB(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
	}

	inline void _offset_(bool reverse, float& last)
	{
#if FALSE
		if (reverse) { last -= 0.5F; }
		else { last += 0.5F; }
#endif // false

	}
	inline void _offset_(bool reverse, ExPointF& last)
	{
		_offset_(reverse, last.x);
		_offset_(reverse, last.y);
	}
	inline void _offset_(bool reverse, ExRectF& last)
	{
		_offset_(reverse, last.left);
		_offset_(reverse, last.top);
		_offset_(reverse, last.right);
		_offset_(reverse, last.bottom);
	}
	inline void _offset_(bool reverse, D2D1_POINT_2F& last)
	{
		_offset_(reverse, last.x);
		_offset_(reverse, last.y);
	}
	inline void _offset_(bool reverse, D2D1_RECT_F& last)
	{
		_offset_(reverse, last.left);
		_offset_(reverse, last.top);
		_offset_(reverse, last.right);
		_offset_(reverse, last.bottom);
	}

	template<typename T, class ...Args>
	inline void _offset_(bool reverse, T& first, Args&... args)
	{
		_offset_(reverse, first);
		_offset_(reverse, args...);
	}

}


