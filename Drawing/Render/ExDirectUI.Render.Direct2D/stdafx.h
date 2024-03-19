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
	inline D2D1_MATRIX_3X2_F Matrix(const ExMatrixElements3x2& matrix)
	{
		return D2D1::Matrix3x2F(
			matrix._11, matrix._12,
			matrix._21, matrix._22,
			matrix._31, matrix._32
		);
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

}


