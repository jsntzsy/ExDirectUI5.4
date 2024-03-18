/**
 * @file factory.h
 * @brief 渲染器工厂头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-18
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	EXINTERFACE("8B77AA10-57F3-4676-A8DA-549221E426C2") IExRender : public IExModule
	{
		EXMETHOD bool EXOBJCALL IsSupportComposition() const PURE;
		
		
	};

	struct _EnumTest_ {
		_EnumTest_() = delete;
		_EnumTest_(const _EnumTest_&) = delete;
		_EnumTest_(_EnumTest_&&) = delete;
		_EnumTest_& operator=(const _EnumTest_&) = delete;

		enum Values;
	};
	using EnumTest = _EnumTest_::Values;
	enum _EnumTest_::Values {
		One,
		Two,
		Three
	};

	DWORD a = EnumTest::One | EnumTest::Two;

	int T(EnumTest t)
	{
		return T(EnumTest::One);
	}



}
