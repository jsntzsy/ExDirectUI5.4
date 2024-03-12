/**
 * @file lock.hpp
 * @brief 线程锁功能实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once
#include <Windows.h>

namespace ExDirectUI
{
	class ExLockRegion;
	
	/// 线程锁
	class ExLock
	{
	public:
		ExLock() { InitializeCriticalSection(&m_cs); }
		~ExLock() { DeleteCriticalSection(&m_cs); }
		void Lock() { EnterCriticalSection(&m_cs); }
		void UnLock() { LeaveCriticalSection(&m_cs); }
		
		ExLockRegion LockRegion();

	private:
		CRITICAL_SECTION m_cs;
	};	

	/// 线程锁区域
	class ExLockRegion
	{
	private:
		ExLockRegion(ExLockRegion& r) = delete;
		
	public:
		ExLockRegion(const ExLock& locker) :m_locker(const_cast<ExLock&>(locker)) { m_locker.Lock(); }
		~ExLockRegion() { m_locker.UnLock(); }

	private:
		ExLock& m_locker;
		friend class ExLock;
	};

	inline ExLockRegion ExLock::LockRegion() { return ExLockRegion(*this); }

	// REMARK:不确定Release模式下会不会被优化掉，需要测试
#define _Locked(locker)		volatile ExLockRegion __LOCK_REGION__(locker)

}
