/**
 * @file event.hpp
 * @brief 事件委托类头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-25
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	interface IExElement;

	template<typename ResultType, typename ...Args>
	class ExDelegate
	{
	public:
		using handler_func_t = bool(CALLBACK*)(Args..., ResultType* r_result);

		bool AddHandler(handler_func_t fn)
		{
			auto it = std::find(m_handlers.begin(), m_handlers.end(), fn);
			if (it != m_handlers.end()) { return false; }
			m_handlers.push_back(fn);
			return true;
		}

		bool RemoveHandler(handler_func_t fn)
		{
			auto it = std::find(m_handlers.begin(), m_handlers.end(), fn);
			if (it == m_handlers.end()) { return false; }
			m_handlers.erase(it);
			return true;
		}

		void ClearHandlers() { m_handlers.clear(); }

		ResultType Invoke(Args... args)
		{
			ResultType result = ResultType();
			for (auto handler : m_handlers) {
				if (handler(args..., &result)) { break; }
			}
			return result;
		}

	public:
		inline ExDelegate& operator +=(handler_func_t fn) { AddHandler(fn); return *this; }
		inline ExDelegate& operator -=(handler_func_t fn) { RemoveHandler(fn); return *this; }
		inline ResultType operator()(Args... args) { return Invoke(args...); }

	private:
		std::vector<handler_func_t> m_handlers;
	};

	using ExEventDelegate = ExDelegate<LRESULT, IExElement*, int, uint32_t, WPARAM, LPARAM>;
}
