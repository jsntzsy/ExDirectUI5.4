/**
 * @file locker.hpp
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
	class ExLocker
	{
	public:
		ExLocker() { InitializeCriticalSection(&m_cs); }
		~ExLocker() { DeleteCriticalSection(&m_cs); }
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
		ExLockRegion(ExLocker& locker) :m_locker(locker) { locker.Lock(); }
		~ExLockRegion() { m_locker.UnLock(); }

	private:
		ExLocker& m_locker;
		friend class ExLocker;
	};

	inline ExLockRegion ExLocker::LockRegion() { return ExLockRegion(*this); }

	// REMARK:不确定Release模式下会不会被优化掉，需要测试
#define _Locked(locker)		volatile ExLockRegion __LOCK_REGION__(locker)

}
