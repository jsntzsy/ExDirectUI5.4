/**
 * @file matrix.h
 * @brief 渲染器矩阵头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-14
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 矩阵元素3x2
	struct ExMatrixElements3x2
	{
		float _11, _12;
		float _21, _22;
		float _31, _32;
	};

	/// 矩阵对象3x2
	class ExMatrix3x2 : public ExMatrixElements3x2
	{
	public:
		ExMatrix3x2()
		{
			_11 = 1.0f; _12 = 0.0f;
			_21 = 0.0f; _22 = 1.0f;
			_31 = 0.0f; _32 = 0.0f;
		}
		ExMatrix3x2(float m11, float m12, float m21, float m22, float m31, float m32)
		{
			_11 = m11; _12 = m12;
			_21 = m21; _22 = m22;
			_31 = m31; _32 = m32;
		}
		ExMatrix3x2(const ExMatrixElements3x2& elements)
		{
			_11 = elements._11; _12 = elements._12;
			_21 = elements._21; _22 = elements._22;
			_31 = elements._31; _32 = elements._32;
		}

	public:
		inline ExMatrix3x2& Reset()
		{
			_11 = 1.0f; _12 = 0.0f;
			_21 = 0.0f; _22 = 1.0f;
			_31 = 0.0f; _32 = 0.0f;
			return *this;
		}
		inline ExMatrix3x2& SetElements(float m11, float m12, float m21, float m22, float m31, float m32)
		{
			_11 = m11; _12 = m12;
			_21 = m21; _22 = m22;
			_31 = m31; _32 = m32;
			return *this;
		}
		inline ExMatrix3x2& SetElements(const ExMatrixElements3x2& elements)
		{
			_11 = elements._11; _12 = elements._12;
			_21 = elements._21; _22 = elements._22;
			_31 = elements._31; _32 = elements._32;
			return *this;
		}

		inline bool IsIdentity() const
		{
			return _11 == 1.0f && _12 == 0.0f &&
				_21 == 0.0f && _22 == 1.0f &&
				_31 == 0.0f && _32 == 0.0f;
		}
		inline float Determinant() const { return (_11 * _22) - (_12 * _21); }
		inline bool IsInvertible() const { return Determinant() != 0.0f; }
		inline bool Invert()
		{
			float det = Determinant();
			if (det == 0.0f) { return false; }

			float inv_det = 1.0f / det;

			ExMatrix3x2 m = *this;
			_11 = m._22 * inv_det;
			_12 = -m._12 * inv_det;
			_21 = -m._21 * inv_det;
			_22 = m._11 * inv_det;
			_31 = (m._21 * m._32 - m._22 * m._31) * inv_det;
			_32 = (m._12 * m._31 - m._11 * m._32) * inv_det;

			return true;
		}

		inline ExMatrix3x2& Multiply(const ExMatrixElements3x2& src, bool prepend = false)
		{
			ExMatrix3x2 dst = *this;
			if (prepend)
			{
				_11 = src._11 * dst._11 + src._12 * dst._21;
				_12 = src._11 * dst._12 + src._12 * dst._22;
				_21 = src._21 * dst._11 + src._22 * dst._21;
				_22 = src._21 * dst._12 + src._22 * dst._22;
				_31 = src._31 * dst._11 + src._32 * dst._21 + dst._31;
				_32 = src._31 * dst._12 + src._32 * dst._22 + dst._32;
			}
			else
			{
				_11 = dst._11 * src._11 + dst._12 * src._21;
				_12 = dst._11 * src._12 + dst._12 * src._22;
				_21 = dst._21 * src._11 + dst._22 * src._21;
				_22 = dst._21 * src._12 + dst._22 * src._22;
				_31 = dst._31 * src._11 + dst._32 * src._21 + src._31;
				_32 = dst._31 * src._12 + dst._32 * src._22 + src._32;
			}
			return *this;
		}
		inline ExMatrix3x2& Translate(float x, float y, bool prepend = false)
		{
			return Multiply(MakeTranslation(x, y), prepend);
		}
		inline ExMatrix3x2& Scale(float x, float y, float org_x = 0.0f, float org_y = 0.0f, bool prepend = false)
		{
			return Multiply(MakeScale(x, y, org_x, org_y), prepend);
		}
		inline ExMatrix3x2& Rotate(float angle, float org_x = 0.0f, float org_y = 0.0f, bool prepend = false)
		{
			return Multiply(MakeRotation(angle, org_x, org_y), prepend);
		}
		inline ExMatrix3x2& Skew(float angle_x, float angle_y, float org_x = 0.0f, float org_y = 0.0f, bool prepend = false)
		{
			return Multiply(MakeSkew(angle_x, angle_y, org_x, org_y), prepend);
		}

		inline ExMatrix3x2& Transform(ExPointF* point, bool is_vector = false) const
		{
			return Transform(point, 1, is_vector);
		}
		inline ExMatrix3x2& Transform(ExRectF* rect) const
		{
			return Transform((ExPointF*)rect, 2, false);
		}
		inline ExMatrix3x2& Transform(ExPointF* points, uint32_t count, bool is_vector = false) const
		{
			for (uint32_t i = 0; i < count; i++) {
				ExPointF pt = points[i];
				points[i].x = pt.x * _11 + pt.y * _21;
				points[i].y = pt.x * _12 + pt.y * _22;

				if (!is_vector) {
					points[i].x += _31;
					points[i].y += _32;
				}
			}
			return const_cast<ExMatrix3x2&>(*this);
		}

		inline operator bool() const { return !IsIdentity(); }
		inline ExMatrix3x2& operator* (const ExMatrixElements3x2& src) { return Multiply(src); }
		inline ExMatrix3x2& operator>>(ExPointF& point) const { return Transform(&point); }
		inline ExMatrix3x2& operator>>(ExRectF& rect) const { return Transform(&rect); }
		inline operator float* () { return *this; }
		inline operator ExMatrixElements3x2& () { return *this; }
		inline float operator[] (int index) const MAYTHROW
		{
			ExAssert(index >= 0 && index < 6);
			return ((float*)this)[index];
		}

	public:
		inline static ExMatrix3x2 MakeIdentity() { return ExMatrix3x2(); }
		inline static ExMatrix3x2 MakeTranslation(float x, float y)
		{
			return ExMatrix3x2(
				1.0f, 0.0f,
				0.0f, 1.0f,
				x, y
			);
		}
		inline static ExMatrix3x2 MakeScale(float x, float y, float org_x = 0.0f, float org_y = 0.0f)
		{
			return ExMatrix3x2(
				x, 0.0f,
				0.0f, y,
				org_x - x * org_x,
				org_y - y * org_y
			);
		}
		inline static ExMatrix3x2 MakeRotation(float angle, float org_x = 0.0f, float org_y = 0.0f)
		{
			float arc_angle = angle * PI / 180;
			float sin_a = sinf(arc_angle);
			float cos_a = cosf(arc_angle);

			return ExMatrix3x2(
				cos_a, sin_a,
				-sin_a, cos_a,
				org_x - cos_a * org_x + sin_a * org_y,
				org_y - cos_a * org_y - sin_a * org_x
			);
		}
		inline static ExMatrix3x2 MakeSkew(float angle_x, float angle_y, float org_x = 0.0f, float org_y = 0.0f)
		{
			float arc_angle_x = angle_x * PI / 180;
			float arc_angle_y = angle_y * PI / 180;
			float tan_x = tanf(arc_angle_x);
			float tan_y = tanf(arc_angle_y);

			return ExMatrix3x2(
				1.0f, tan_y,
				tan_x, 1.0f,
				-org_x * tan_x,
				-org_y * tan_y
			);
		}
		inline static ExMatrix3x2 MakeMultiply(const ExMatrixElements3x2& m1, const ExMatrixElements3x2& m2)
		{
			ExMatrix3x2 mx;
			mx._11 = m1._11 * m2._11 + m1._12 * m2._21;
			mx._12 = m1._11 * m2._12 + m1._12 * m2._22;
			mx._21 = m1._21 * m2._11 + m1._22 * m2._21;
			mx._22 = m1._21 * m2._12 + m1._22 * m2._22;
			mx._31 = m1._31 * m2._11 + m1._32 * m2._21 + m2._31;
			mx._32 = m1._31 * m2._12 + m1._32 * m2._22 + m2._32;
			return mx;
		}
	};

	typedef ExMatrixElements3x2 ExMatrixElements;
	typedef ExMatrix3x2 ExMatrix;

}
