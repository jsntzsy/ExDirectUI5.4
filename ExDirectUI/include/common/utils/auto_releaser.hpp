/**
 * @file auto_releaser.h
 * @brief 自动释放器头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */
#pragma once
#include <functional>
#include <Windows.h>

namespace ExDirectUI
{
	template<typename T, T InvalidValue = NULL>
	class ExAutoReleaser
	{
	public:
		using ReleaserFunc = std::function<void(T object)>;

		ExAutoReleaser(T object, ReleaserFunc releaser)
			: m_object(object), m_releaser(releaser)
		{}

		ExAutoReleaser(ExAutoReleaser&& other) noexcept
			: m_object(other.m_object), m_releaser(other.m_releaser)
		{
			other.m_object = InvalidValue;
			other.m_releaser = nullptr;
		}

		~ExAutoReleaser() { Clear(); }

		ExAutoReleaser(const ExAutoReleaser&) = delete;
		ExAutoReleaser& operator=(const ExAutoReleaser&) = delete;

		inline bool IsValid() const { return m_object != InvalidValue; }
		inline T Get() const { return m_object; }
		inline T* GetAddress() { return &m_object; }
		inline void Release()
		{
			if (m_object != InvalidValue && m_releaser) { m_releaser(m_object); }
			m_object = InvalidValue;
			m_releaser = nullptr;
		}
		inline T* ReleaseGetAddress() { Release(); return &m_object; }

		inline void Attach(T object)
		{
			m_object = object;
			return object;
		}
		inline T Detach()
		{
			T object = m_object;
			m_object = InvalidValue;
			return object;
		}

		inline operator T() const { return Get(); }
		inline T* operator&() { return GetAddress(); }
		inline T* operator->() { return GetAddress(); }
		inline T* operator-() { return ReleaseGetAddress(); }
		inline bool operator bool() const { return IsValid(); }

	private:
		ReleaserFunc m_releaser;
		T m_object;
	};

	template<HANDLE InvalidValue = NULL>
	class ExAutoHandle : public ExAutoReleaser<HANDLE, InvalidValue>
	{
	public:
		inline static const auto _CloseHandleFunc = [](HANDLE handle) { CloseHandle(handle); };

		ExAutoHandleReleaser(HANDLE object, ReleaserFunc releaser = _CloseHandleFunc)
			: ExAutoReleaser(object, releaser)
		{}
	};

	template<HGDIOBJ InvalidValue = NULL>
	class ExAutoGdiObject : public ExAutoReleaser<HGDIOBJ, InvalidValue>
	{
	public:
		inline static const auto _DeleteObjectFunc = [](HGDIOBJ object) { DeleteObject(object); };

		ExAutoGdiObject(HGDIOBJ object, ReleaserFunc releaser = _DeleteObjectFunc)
			: ExAutoReleaser(object, releaser)
		{}
	};

	template<typename T, T* InvalidValue = nullptr>
	class ExAutoMemory : public ExAutoReleaser<T*, InvalidValue>
	{
	public:
		inline static const auto _DeleteMemoryFunc = [](T* memory) { delete memory; };

		ExAutoMemory(T* memory, ReleaserFunc releaser = _DeleteMemoryFunc)
			: ExAutoReleaser(memory, releaser)
		{}
	};

}
