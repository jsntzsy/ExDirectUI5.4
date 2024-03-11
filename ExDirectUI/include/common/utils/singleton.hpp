/**
 * @file singleton.hpp
 * @brief 单例对象实现类头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 */
#pragma once

namespace ExDirectUI
{
	template <class T>
	class ExSingleton
	{
	public:
		static T* Instance()
		{
			static T s_instance;
			return &s_instance;
		}
	};

	///////////////////////

	template <class T>
	class ExLazySingleton
	{
	private:
		static T* s_instance;
	public:
		static bool SetInstance(T* instance)
		{
			bool is_null = s_instance == nullptr;
			if (is_null) { s_instance = instance; }
			return is_null;
		}

		static T* GetInstance()
		{
			if (s_instance == nullptr) { s_instance = new T(); }
			return s_instance;
		}

		static T* Instance() { ExAssert(s_instance); return s_instance; }

		static T* DetachInstance() { T* temp = s_instance; s_instance = nullptr; return temp; }

		static void ClearInstance(bool delete_instance = true)
		{
			if (s_instance != nullptr)
			{
				if (delete_instance) { delete s_instance; }
				s_instance = nullptr;
			}
		}

	};

	template<class T>
	T* ExLazySingleton<T>::s_instance = nullptr;

}

