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


}
