/**
 * @file new.h
 * @brief new接管头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

#ifndef EX_CFG_NO_TAKEOVER_NEW

#ifdef EX_CFG_DEBUG_CALL_INFO

inline void* operator new(size_t size) { return ::ExDirectUI::ExMemAllocD(size, nullptr, 0); }
inline void* operator new[](size_t size) { return ::ExDirectUI::ExMemAllocD(size, nullptr, 0); }
inline void operator delete(void* p) { ::ExDirectUI::ExMemFreeD(p); }
inline void operator delete[](void* p) { ::ExDirectUI::ExMemFreeD(p); }

inline void* operator new(size_t size, LPCWSTR file, int line) { return ::ExDirectUI::ExMemAllocD(size, file, line); }
inline void* operator new[](size_t size, LPCWSTR file, int line) { return ::ExDirectUI::ExMemAllocD(size, file, line); }
inline void operator delete(void* p, LPCWSTR file, int line) { ::ExDirectUI::ExMemFreeD(p); }
inline void operator delete[](void* p, LPCWSTR file, int line) { ::ExDirectUI::ExMemFreeD(p); }

#define NEW new(__CALLINFO__)
#else
inline void* operator new(size_t size) { return ::ExDirectUI::ExMemAlloc(size); }
inline void* operator new[](size_t size) { return ::ExDirectUI::ExMemAlloc(size); }
inline void operator delete(void* p) { ::ExDirectUI::ExMemFree(p); }
inline void operator delete[](void* p) { ::ExDirectUI::ExMemFree(p); }

#define NEW new
#endif
#else
#define NEW new
#endif // 


