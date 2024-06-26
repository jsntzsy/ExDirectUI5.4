﻿/**
 * @file module.h
 * @brief 模块管理内部头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	HRESULT EXCALL _ExModule_Init(const ExEngineInitInfo* init_info);
	void EXCALL _ExModule_UnInit();
}
