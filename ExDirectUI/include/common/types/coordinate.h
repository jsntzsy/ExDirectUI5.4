/**
 * @file coordinate.h
 * @brief 坐标相关类型定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	struct ExPoint;
	struct ExPointF;

	/// 整数坐标
	struct ExPoint : public POINT
	{
		ExPoint() { x = y = 0; }
		ExPoint(long X, long Y) { x = X; y = Y; }
		ExPoint(POINT* pt) { x = pt->x; y = pt->y; }
		ExPoint(POINT& pt) { x = pt.x; y = pt.y; }
		ExPoint(SIZE* sz) { x = sz->cx; y = sz->cy; }
		ExPoint(SIZE& sz) { x = sz.cx; y = sz.cy; }
		ExPoint(LPARAM lParam) { x = GET_X_LPARAM(lParam); y = GET_Y_LPARAM(lParam); }

		inline void Offset(long x, long y) { this->x += x; this->y += y; }

		inline bool operator==(ExPoint& pt) const
		{
			return x == pt.x && y == pt.y;
		}

		ExPointF ToPointF() const;
	};

	/// 小数坐标
	struct ExPointF
	{
		float x;
		float y;

		ExPointF() { x = y = 0; }
		ExPointF(float X, float Y) { x = X; y = Y; }
		ExPointF(POINT* pt) { x = pt->x; y = pt->y; }
		ExPointF(POINT& pt) { x = pt.x; y = pt.y; }

		inline void Offset(float x, float y) { this->x += x; this->y += y; }

		inline bool operator==(ExPointF& pt) const
		{
			return float_eq(x, pt.x) && float_eq(y, pt.y);
		}


		ExPoint ToPoint() const;
	};

	inline ExPointF ExPoint::ToPointF() const { return ExPointF(x, y); }
	inline ExPoint ExPointF::ToPoint() const { return ExPoint(roundf(x), roundf(y)); }

	/////////////////////////////////

	struct ExRect;
	struct ExRectF;

	/// 整数矩形
	struct ExRect : public RECT
	{
		ExRect() { left = top = right = bottom = 0; }
		ExRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
		ExRect(long l, long t, long w, long h, bool unuse) { left = l; top = t; right = l + w; bottom = t + h; }
		ExRect(RECT* rc) { left = rc->left; top = rc->top; right = rc->right; bottom = rc->bottom; }
		ExRect(RECT& rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }

		inline long Width()  const { return right - left; }
		inline long Height() const { return bottom - top; }

		inline long GetLeft() const { return min(left, right); }
		inline long GetTop()  const { return min(top, bottom); }
		inline long GetRight() const { return max(left, right); }
		inline long GetBottom() const { return max(top, bottom); }
		inline long GetWidth() const { return abs(right - left); }
		inline long GetHeight() const { return abs(bottom - top); }

		inline void Offset(long x, long y) { left += x; top += y; right += x; bottom += y; }
		inline bool IsEmpty() const { return ::IsRectEmpty(this); }
		inline bool PtInRect(long x, long y) const { return ::PtInRect(this, POINT{ x,y }); }

		inline ExRect Normalize() const
		{
			ExRect rc;
			rc.left = min(left, right);
			rc.top = min(top, bottom);
			rc.right = max(left, right);
			rc.bottom = max(top, bottom);
			return rc;
		}

		inline bool operator==(ExRect& rc) const
		{
			return left == rc.left && top == rc.top && right == rc.right && bottom == rc.bottom;
		}

		ExRectF ToRectF() const;
	};

	/// 小数矩形
	struct ExRectF
	{
		float left;
		float top;
		float right;
		float bottom;

		ExRectF() { left = top = right = bottom = 0; }
		ExRectF(float l, float t, float r, float b) { left = l; top = t; right = r; bottom = b; }
		ExRectF(float l, float t, float w, float h, bool unuse) { left = l; top = t; right = l + w; bottom = t + h; }
		ExRectF(RECT* rc) { left = rc->left; top = rc->top; right = rc->right; bottom = rc->bottom; }
		ExRectF(RECT& rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }

		inline float Width() const { return right - left; }
		inline float Height() const { return bottom - top; }

		inline float GetLeft() const { return min(left, right); }
		inline float GetTop() const { return min(top, bottom); }
		inline float GetRight() const { return max(left, right); }
		inline float GetBottom() const { return max(top, bottom); }
		inline float GetWidth() const { return fabs(right - left); }
		inline float GetHeight() const { return fabs(bottom - top); }

		inline void Offset(float x, float y) { left += x; top += y; right += x; bottom += y; }
		inline bool IsEmpty() const { return float_eq(left, right) || float_eq(top, bottom); }
		inline bool PtInRect(float x, float y) const { return x >= left && x <= top && y >= top && y <= bottom; }

		inline ExRectF Normalize() const
		{
			ExRectF rc;
			rc.left = min(left, right);
			rc.top = min(top, bottom);
			rc.right = max(left, right);
			rc.bottom = max(top, bottom);
			return rc;
		}

		inline bool operator==(ExRectF& rc) const
		{
			return float_eq(left, rc.left) && float_eq(top, rc.top) &&
				float_eq(right, rc.right) && float_eq(bottom, rc.bottom);
		}

		ExRect ToRect() const;
	};

	inline ExRectF ExRect::ToRectF() const { return ExRectF(left, top, right, bottom); }
	inline ExRect ExRectF::ToRect() const { return ExRect(roundf(left), roundf(top), roundf(right), roundf(bottom)); }

	struct ExNumberU
	{
		float value;
		DWORD unit;
	};

	struct ExPointU : public ExPointF
	{
		ExPoint unit;
	};

	struct ExRectU : public ExRectF
	{
		ExRect unit;
	};
}

#define _expand_point_(point)	point.x, point.y
#define _expand_rect_(rect)	rect.left, rect.top, rect.right, rect.bottom

#define _expand_point_split_(point, _split_)	point.x _split_ point.y
#define _expand_rect_split_(rect, _split_)	rect.left _split_ rect.top _split_ rect.right _split_ rect.bottom




