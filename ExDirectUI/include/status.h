﻿/**
 * @file status.h
 * @brief 状态定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#pragma region 状态定义相关宏

#define _EX_MAKE_HRESULT(ERROR, MODULE, CODE) \
	MAKE_HRESULT(ERROR, FACILITY_ITF, (((MODULE & 0xFF) << 16) | (CODE)))

#define EX_DECLEAR_ERROR_STATUS(NAME, MODULE, CODE) \
	constexpr HRESULT NAME = _EX_MAKE_HRESULT(SEVERITY_ERROR, MODULE, CODE)

#define EX_DECLEAR_NORMAL_STATUS(NAME, MODULE, CODE) \
	constexpr HRESULT NAME = _EX_MAKE_HRESULT(SEVERITY_SUCCESS, MODULE, CODE)

#define EX_DECLEAR_STATUS_NAME(STATUS)				case STATUS: return L#STATUS
#define EX_DECLEAR_STATUS_NAME_(STATUS,NAME)		case STATUS: return NAME
#define EX_DECLEAR_STATUS_NAME_STR(STATUS, STR)		case STATUS: return L#STATUS "(" STR ")"


#define EX_DECLEAR_STATUS_NAME_BEGIN() \
	inline LPCWSTR EXCALL _ExStatus_GetName(HRESULT status) { \
		switch (status) {

#define EX_DECLEAR_STATUS_NAME_END() default: return nullptr; } }

#pragma endregion

namespace ExDirectUI
{
	EX_DECLEAR_ERROR_STATUS(EE_LOST_NECESSARY, 0x00, 0x80);

	EX_DECLEAR_ERROR_STATUS(EE_NOREADY,0x00, 0x01);
	EX_DECLEAR_ERROR_STATUS(EE_NOEXISTS, 0x00, 0x02);
	EX_DECLEAR_ERROR_STATUS(EE_EXISTS, 0x00, 0x02);
	EX_DECLEAR_ERROR_STATUS(EE_IO, 0x00, 0x0F);



	EX_DECLEAR_STATUS_NAME_BEGIN();

	EX_DECLEAR_STATUS_NAME_STR(EE_LOST_NECESSARY, L"缺少必备项");

	EX_DECLEAR_STATUS_NAME_STR(EE_NOREADY, L"未就绪");
	EX_DECLEAR_STATUS_NAME_STR(EE_IO, L"读写错误");




	EX_DECLEAR_STATUS_NAME_END();
	
}



