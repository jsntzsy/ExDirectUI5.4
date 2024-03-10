/**
 * @file status.h
 * @brief 状态定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#define _EX_MAKE_HRESULT(ERROR, MODULE, CODE) \
	MAKE_HRESULT(ERROR, FACILITY_ITF, ((MODULE & 0xFF) << 16) | (CODE))

#define EX_DECLEAR_ERROR_STATUS (NAME, MODULE, CODE) \
	constexpr HRESULT NAME = _EX_MAKE_HRESULT(SEVERITY_ERROR, MODULE, CODE)

#define EX_DECLEAR_NORMAL_STATUS (NAME, MODULE, CODE) \
	constexpr HRESULT NAME = _EX_MAKE_HRESULT(SEVERITY_SUCCESS, MODULE, CODE)

namespace ExDirectUI
{
	

	
}



