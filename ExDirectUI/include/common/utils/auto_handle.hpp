/**
 * @file auto_handle.hpp
 * @brief 自动句柄实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 */
#pragma once
#include <functional>

namespace ExDirectUI
{
	class ExAutoHandle
	{
		inline const static std::function<void(EXHANDLE handle)> default_deletor = [](EXHANDLE handle) { ExHandleClose(handle); };

	public:
		ExAutoHandle(EXHANDLE handle = NULL, std::function<void(EXHANDLE handle)> deletor = default_deletor, bool add_ref = false)
		{
			if (add_ref)
			{
				throw_or_ok(ExHandleRef(handle), L"引用句柄失败");
			}
			m_handle = handle;
			m_deletor = deletor;
		}
		ExAutoHandle(std::function<EXST(EXHANDLE* r_handle)> creator,
			std::function<void(EXHANDLE handle)> deletor = default_deletor)
		{
			throw_or_ok(creator(&m_handle), L"创建对象句柄失败");
			m_deletor = deletor;
		}
		~ExAutoHandle()
		{
			if (m_handle && m_deletor)
			{
				m_deletor(m_handle);
			}
		}

		inline operator EXHANDLE() const { return m_handle; }
		inline EXHANDLE* operator&() { return &m_handle; }
		inline EXHANDLE get() const { return m_handle; }
		inline void free()
		{
			if (m_handle && m_deletor)
			{
				m_deletor(m_handle);
			}
			m_handle = NULL;
		}

	private:
		EXHANDLE m_handle{};
		std::function<void(EXHANDLE handle)> m_deletor{};
	};

#define ex_declear_handle(name) ExAutoHandle name{}

#define ex_use_object(name,handle) ExAutoHandle name(handle)

#define ex_use_object_free(name,handle,on_free) \
	ExAutoHandle name(handle, [&](EXHANDLE h){ on_free(h);})

#define ex_use_create_object(name, on_create) \
	ExAutoHandle name([&](EXHANDLE* r_##name)->EXST{ return on_create; })

#define ex_use_create_object_free(name, on_create, on_free) \
	ExAutoHandle name([&](EXHANDLE* r_##name)->EXST{ return on_create; }, [&](EXHANDLE h){ on_free(h);})


}
