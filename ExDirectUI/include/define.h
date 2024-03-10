/**
 * @file define.h
 * @brief 宏定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#pragma region 导出函数声明

#define EXCALL __stdcall
#define NOTHROW noexcept

#if defined(EX_CFG_BUILD_TO_LIB) || defined(EX_CFG_BUILD_IN_LIB)
#define EXAPI 
#else
#ifdef EXDIRECTUI_EXPORTS
#define EXAPI __declspec(dllexport)
#else
#define EXAPI __declspec(dllimport)
#endif // EXDIRECTUI_EXPORTS
#endif

#pragma endregion

#pragma region 接口相关声明

#define EXOBJCALL STDMETHODCALLTYPE
#define EXINTERFACE(iid)	__interface __declspec(uuid(iid)) __declspec(novtable)
#define EXMETHOD virtual __declspec(nothrow)
#define EXSTDMETHOD EXMETHOD ::HRESULT EXOBJCALL

#pragma endregion

#pragma region 调试处理

#ifdef EX_CFG_DEBUG
#endif // EX_CFG_DEBUG

#pragma region 调试中断

#ifdef EX_CFG_DEBUG_INTERRUPT
#include <assert.h>
#define EX_DEBUG_BREAK()	__debugbreak()


#else
#define EX_DEBUG_BREAK()

#endif // EX_CFG_DEBUG_INTERRUPT


#pragma endregion


#pragma region 调用信息

#ifdef EX_CFG_DEBUG_CALL_INFO
#define __CALLINFO__	__FILEW__, __LINE__
#else
#define __CALLINFO__	nullptr, 0
#endif // EX_CFG_DEBUG_CALL_INFO

#pragma endregion

#pragma region 参数检查

#ifdef EX_CFG_DEBUG_CHECK_PARAM
#define EX_PARAM_CHECK(EXP)		{if(!(EXP)) return E_INVALIDARG;}
#else
#define EX_PARAM_CHECK(EXP)
#endif // EX_CFG_DEBUG_CHECK_PARAM

#pragma endregion

#pragma endregion












