/**
 * @file matrix_test.cpp
 * @brief 渲染器矩阵测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */

#include "pch.h"

#include "common/utils/auto_releaser.hpp"
#include <d2d1.h>
#include <gdiplus.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "gdiplus.lib")

#define TEST_MODULE_NAME    Ex_Render_Matrix_Test

class TEST_MODULE_NAME : public ::testing::Test
{
protected:

	void SetUp() override
	{
		Gdiplus::GdiplusStartupInput si;
		Gdiplus::GdiplusStartup(&token, &si, NULL);
	}

	void TearDown() override
	{
		Gdiplus::GdiplusShutdown(token);
	}

	//如果相同返回-1,否则返回不相同的元素索引
	inline int MatrixEquals(const float* m1, const float* m2)
	{
		for (int i = 0; i < 6; i++)
		{
			if (!float_eq(m1[i], m2[i])) { return i; }
		}
		return -1;
	}

	inline int MatrixEquals(const ExMatrixElements3x2& m1, const D2D1::Matrix3x2F& m2)
	{
		float* m1_ = (float*)&m1;
		float* m2_ = (float*)&m2;
		return MatrixEquals(m1_, m2_);
	}

	inline int MatrixEquals(const ExMatrix3x2& m1, const Gdiplus::Matrix& m2)
	{
		float* m1_ = (float*)&m1;
		float m2_[6];
		m2.GetElements(m2_);
		return MatrixEquals(m1_, m2_);
	}


	ULONG_PTR token = 0;
};

TEST_F(TEST_MODULE_NAME, Test)
{
	ExMatrix3x2 _mx{ 11,22,33,44,55,66 };
	Gdiplus::Matrix _mx_gp{ 11,22,33,44,55,66 };
	D2D1::Matrix3x2F _mx_d2d{ 11,22,33,44,55,66 };


	ExMatrix3x2 mx{};
	Gdiplus::Matrix mx_gp{};
	D2D1::Matrix3x2F mx_d2d = D2D1::Matrix3x2F::Identity();

	// 测试初始化
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试直接设置元素
	mx.SetElements(1, 2, 3, 4, 5, 6);
	mx_gp.SetElements(1, 2, 3, 4, 5, 6);
	mx_d2d = D2D1::Matrix3x2F(1, 2, 3, 4, 5, 6);
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵相乘
	mx.Multiply(_mx);
	mx_gp.Multiply(&_mx_gp, Gdiplus::MatrixOrderAppend);
	mx_d2d = mx_d2d * _mx_d2d;
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵重置
	mx.Reset();
	mx_gp.Reset();
	mx_d2d = D2D1::Matrix3x2F::Identity();
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵平移
	mx.Translate(1, 2);
	mx_gp.Translate(1, 2, Gdiplus::MatrixOrderAppend);
	mx_d2d = mx_d2d * D2D1::Matrix3x2F::Translation(1, 2);
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵缩放
	mx.Scale(1, 2);
	mx_gp.Scale(1, 2, Gdiplus::MatrixOrderAppend);
	mx_d2d = mx_d2d * D2D1::Matrix3x2F::Scale(1, 2);
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵旋转
	mx.Rotate(30);
	mx_gp.Rotate(30, Gdiplus::MatrixOrderAppend);
	mx_d2d = mx_d2d * D2D1::Matrix3x2F::Rotation(30);
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵斜切
	//测试和GDI+的不太一样,和D2D是一致的
	//mx.Skew(30, 45, 0, 0, true);
	//mx_gp.Shear(30, 45, Gdiplus::MatrixOrderPrepend);	
	//mx_d2d = D2D1::Matrix3x2F::Skew(30, 45) * mx_d2d;
	//ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	//ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试矩阵反转
	mx.Invert();
	mx_gp.Invert();
	mx_d2d.Invert();
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);

	//测试坐标变换
	ExPointF pt{ 1,2 };
	Gdiplus::PointF pt_gp{ 1,2 };
	D2D1_POINT_2F pt_d2d{ 1,2 };
	mx.Transform(&pt);
	mx_gp.TransformPoints(&pt_gp, 1);
	pt_d2d = mx_d2d.TransformPoint(pt_d2d);
	ASSERT_TRUE(float_eq(pt.x, pt_gp.X));
	ASSERT_TRUE(float_eq(pt.y, pt_gp.Y));
	ASSERT_TRUE(float_eq(pt.x, pt_d2d.x));
	ASSERT_TRUE(float_eq(pt.y, pt_d2d.y));

	//测试向量变换
	pt = { 3,4 };
	pt_gp = { 3,4 };
	//pt_d2d = { 3,4 };
	mx.Transform(&pt, true);
	mx_gp.TransformVectors(&pt_gp, 1);
	//pt_d2d = mx_d2d.TransformVector(pt_d2d);
	ASSERT_TRUE(float_eq(pt.x, pt_gp.X));
	ASSERT_TRUE(float_eq(pt.y, pt_gp.Y));

	//测试右乘
	mx = _mx * mx;
	mx_gp.Multiply(&_mx_gp, Gdiplus::MatrixOrderPrepend);
	mx_d2d = _mx_d2d * mx_d2d;
	ASSERT_EQ(MatrixEquals(mx, mx_d2d), -1);
	ASSERT_EQ(MatrixEquals(mx, mx_gp), -1);



}



