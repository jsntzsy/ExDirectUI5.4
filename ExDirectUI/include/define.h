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
#define ExDbgBreak()				_CrtDbgBreak()		
#define ExAssert(exp_is_true)		assert(exp_is_true)
#define ExAssertMsg(exp_is_true,message)																	\
		(void)(																								\
            (!!(exp_is_true)) ||																			\
            (_wassert(message, __CALLINFO__), 0)															\
        )																									\


#define ExAssertFmt(exp_is_true, format, ...)																\
						(void) ((!!(exp_is_true)) ||														\
						(1 != _CrtDbgReportW(_CRT_ASSERT, __CALLINFO__, nullptr, format, __VA_ARGS__)) ||	\
						(ExDbgBreak(), 0))

#define ExAssertFmtCallInfo(exp_is_true, file, line, format, ...)											\
						(void) ((!!(exp_is_true)) ||														\
						(1 != _CrtDbgReportW(_CRT_ASSERT, file, line, nullptr, format, __VA_ARGS__)) ||		\
						(ExDbgBreak(), 0))

#else

#define ExDbgBreak()												(FALSE)
#define ExAssert(exp_is_true)										(exp_is_true)
#define ExAssertMsg(exp_is_true,message)							(exp_is_true)
#define ExAssertFmt(exp_is_true, format, ...)						(exp_is_true)
#define ExAssertFmtCallInfo(exp_is_true, file,line, format, ...)	(exp_is_true)

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
#define EX_PARAM_CHECK(EXP)				{if(!(EXP)){ ExAssertMsg(EXP, L"InvalidParam: " #EXP); return E_INVALIDARG;}}
#define EX_PARAM_CHECK_RET(EXP,RET)		{if(!(EXP)){ ExAssertMsg(EXP, L"InvalidParam: " #EXP); return RET;}}
#else
#define EX_PARAM_CHECK(EXP)
#define EX_PARAM_CHECK_RET(EXP,RET)
#endif // EX_CFG_DEBUG_CHECK_PARAM

#pragma endregion

#pragma endregion












