/**
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
	MAKE_HRESULT(ERROR, FACILITY_ITF, ((MODULE & 0xFF) << 16) | (CODE))

#define EX_DECLEAR_ERROR_STATUS (NAME, MODULE, CODE) \
	constexpr HRESULT NAME = _EX_MAKE_HRESULT(SEVERITY_ERROR, MODULE, CODE)

#define EX_DECLEAR_NORMAL_STATUS (NAME, MODULE, CODE) \
	constexpr HRESULT NAME = _EX_MAKE_HRESULT(SEVERITY_SUCCESS, MODULE, CODE)

#define EX_DECLEAR_STATUS_NAME_(STATUS,NAME)	case STATUS: return NAME
#define EX_DECLEAR_STATUS_NAME(STATUS)			EX_DECLEAR_STATUS_NAME_(STATUS, L#STATUS)

#define EX_DECLEAR_STATUS_NAME_BEGIN() \
	inline LPCWSTR EXCALL _ExStatus_GetName(HRESULT status) { \
		switch (status) {

#define EX_DECLEAR_STATUS_NAME_END() default: return nullptr; } }

#pragma endregion

namespace ExDirectUI
{
	



	EX_DECLEAR_STATUS_NAME_BEGIN();





	EX_DECLEAR_STATUS_NAME_END();
	
}



