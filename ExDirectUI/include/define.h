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
#define MAYTHROW 
#define EXTERN_C extern "C"

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
#define EXINTERFACE(iid)	interface __declspec(uuid(iid)) __declspec(novtable)
#define EXMETHOD virtual __declspec(nothrow)
//#define EXSTDMETHOD  EXMETHOD HRESULT EXOBJCALL

#pragma endregion

#pragma region 枚举类型声明

#define EXENUM(name) struct _##name##_Enum_ {								\
	_##name##_Enum_() = delete;												\
	_##name##_Enum_(const _##name##_Enum_&) = delete;						\
	_##name##_Enum_(_##name##_Enum_&&) = delete;							\
	_##name##_Enum_& operator=(const _##name##_Enum_&) = delete;			\
	enum Values; };															\
using name = _##name##_Enum_::Values;										\
enum _##name##_Enum_::Values 												

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
            (!!(exp_is_true)) || !ExEngineQueryFlag(EX_ENGINE_FLAG_DEBUG) ||								\
            (_wassert(message, __CALLINFO__), 0)															\
        )																									\


#define ExAssertFmt(exp_is_true, format, ...)																\
						(void) ((!!(exp_is_true)) || !ExEngineQueryFlag(EX_ENGINE_FLAG_DEBUG) ||			\
						(1 != _CrtDbgReportW(_CRT_ASSERT, __CALLINFO__, nullptr, format, __VA_ARGS__)) ||	\
						(ExDbgBreak(), 0))

#define ExAssertFmtCallInfo(exp_is_true, file, line, format, ...)											\
						(void) ((!!(exp_is_true)) || !ExEngineQueryFlag(EX_ENGINE_FLAG_DEBUG) ||			\
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


#pragma region 输出调试
#ifdef EX_CFG_DEBUG_OUTPUT
#define __VAL__(x) L#x
#define __STR__(x) __VAL__(x)


#define ExDebugOutputRaw(str)		OutputDebugStringW(str)
#define ExDebugOutputLine(str)		OutputDebugStringW(str); OutputDebugStringW(L"\n")

#ifdef EX_CFG_DEBUG_CALL_INFO
#define ExDebugOutput(str)	ExDebugOutputRaw(__FILEW__  "("  __STR__(__LINE__) "): "); ExDebugOutputLine(str)

#else
#define ExDebugOutput(str)			ExDebugOutputLine(str)
#endif // EX_CFG_DEBUG_CALL_INFO

#else
#define ExDebugOutputRaw(str)		str
#define ExDebugOutputLine(str)		str
#define ExDebugOutput(str)			str
#endif // EX_CFG_DEBUG_OUTPUT

#pragma endregion



#pragma region 参数检查

#ifdef EX_CFG_DEBUG_CHECK_PARAM
#define CHECK_PARAM(EXP)				{if(!(EXP)){ ExAssertMsg(EXP, L"InvalidParam: " #EXP); return E_INVALIDARG;}}
#define CHECK_PARAM_RET(EXP,RET)		{if(!(EXP)){ ExAssertMsg(EXP, L"InvalidParam: " #EXP); return RET;}}
#else
#define CHECK_PARAM(EXP)
#define CHECK_PARAM_RET(EXP,RET)
#endif // EX_CFG_DEBUG_CHECK_PARAM

#pragma endregion

#pragma endregion


#pragma region 去除define

#undef LoadModule
#undef FreeModule
#undef GetObject
#undef DrawText
#undef DrawTextEx

#pragma endregion











