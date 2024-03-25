/**
 * @file assist.h
 * @brief 辅助功能定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once
#include <math.h>

#pragma region 快捷操作

#define EX_SWAP(A,B)	{auto T=A;A=B;B=T;}

#pragma endregion

#pragma region 计算判断


 // 浮点数相等
inline bool float_eq(float a, float b) { return fabs(a - b) < 1e-6F; }

// 四舍五入
inline float roundf(float r) { return (r > 0.0F) ? floor(r + 0.5F) : ceil(r - 0.5F); }

// 四舍五入
inline double roundd(double r) { return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5); }

// 圆周率
constexpr float PI = 3.14159265358979323846F;

// 系统默认DPI
constexpr int _SYS_DEFAULT_DPI = 96;

// dp到px
inline long _px(float dp, int dpi) { return MulDiv(dp, dpi, _SYS_DEFAULT_DPI); }

// px到dp
inline float _dp(long px, int dpi) { return px * (float)_SYS_DEFAULT_DPI / dpi; }

// 整数到小数
inline float i2f(int n) { return *reinterpret_cast<float*>(&n); }

// 小数到整数
inline int f2i(float f) { return *reinterpret_cast<int*>(&f); }

// 有符号数到无符号数
inline unsigned int i2u(int n) { return *reinterpret_cast<unsigned int*>(&n); }

// 无符号数到有符号数
inline int u2i(unsigned int n) { return *reinterpret_cast<int*>(&n); }

// 生成两个整数之间的随机数
inline int random(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

// 生成两个小数间的随机数
inline float randomf(float min, float max)
{
	return min + (max - min) * (rand() / (RAND_MAX + 1.0F));
}

inline bool query_flags(unsigned int value, unsigned int flags)
{
	return (value & flags) == flags;
}

#pragma endregion

#pragma region 安全操作

// 安全删除
#define	SAFE_DELETE(p)			if(p != NULL){ delete (p); (p) = NULL; }

// 安全删除数组
#define	SAFE_DELETE_ARR(p)		if(p != NULL){ delete[] (p); (p) = NULL; }

// 安全释放对象
#define SAFE_RELEASE(pUnk)		if(pUnk != NULL){ (pUnk)->Release(); (pUnk) = NULL; }

// 安全释放
#define SAFE_FREE(h,FN,...)		if(h != NULL){ FN(h,##__VA_ARGS__); (h) = NULL; }

// 安全释放对象 (用于释放用句柄创建,但存储的是对象的情况)
#define SAFE_FREE_OBJ(pObj)		if(pObj != NULL){ EXHANDLE __H__ = pObj->GetHandle(); if(__H__ != NULL) ExHandleClose(__H__); else pObj->Release(); (pObj) = NULL; }

// 安全获取
#define SAFE_GET(pObj,XX)		(pObj != NULL ? pObj->XX : 0)

#pragma endregion

#pragma region 特定值生成

// 生成时钟ID
#define MAKE_TIMER_ID(P,ID)		((UINT_PTR)(((UINT_PTR)P) + (ID)))

// 合并复合整数
#define MAKEDWORD(A, B)			((DWORD_PTR)MAKELONG(A,B))

#pragma endregion


















