/**
 * @file Exception.h
 * @brief 异常头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/**
	 * @brief ExDirectUI异常
	 */
	class ExException
	{
	public:
		ExException(HRESULT status)
		{
			m_status = status;
			m_message = L"";
			m_file = L"";
			m_line = 0;
		}
		ExException(HRESULT status, LPCWSTR message)
		{
			m_status = status;
			m_message = message;
			m_file = L"";
			m_line = 0;
		}
		ExException(HRESULT status, LPCWSTR message, LPCWSTR file, int line)
		{
			m_status = status;
			m_message = message ? message : L"";
			m_file = file ? file : L"";
			m_line = line;
		}

		inline HRESULT status() const { return m_status; }
		inline const std::wstring& message() const { return m_message; }
		inline const std::wstring& file() const { return m_file; }
		inline int line() const { return m_line; }

		inline bool is_failed() const { return FAILED(m_status); }
		inline operator bool() const { return is_failed(); }

		inline HRESULT handle() const
		{
			return ExStatusHandle(m_status,
				m_file.size() != 0 ? m_file.c_str() : nullptr, m_line,
				m_message.c_str()
			);
		}

	private:
		HRESULT m_status;
		std::wstring m_message;
		std::wstring m_file;
		int m_line;
	};

#define throw_ex(status,message)	throw ExException(status, message, __CALLINFO__)					// 抛出异常
#define throw_if_false(exp,status,message) if(!(exp)) { throw_ex(status,message); }						// 错误则抛出异常
#define throw_if_failed(hr,message) { HRESULT _HR_ = hr; if(FAILED(_HR_)){ throw_ex(_HR_, message); }}	// 失败则抛出异常
#define throw_if_notok(hr,message) { HRESULT _HR_ = hr; if(_HR_ != S_OK){ throw_ex(_HR_, message); }}	// 不成功则抛出异常

#define catch_return(todo)		catch(ExException& e) { todo; return e.handle(); }		// 捕获异常并返回
#define catch_continue(todo)	catch(ExException& e) { todo; e.handle(); }				// 捕获异常并继续执行
#define catch_ignore(todo)		catch(ExException& e) { todo; }							// 捕获异常但忽略
#define catch_throw(todo)		catch(ExException& e) { todo; throw; }					// 捕获异常并继续抛出
#define catch_default			catch_return											// 默认捕获异常

	// 错误则处理异常
#define handle_if_false(exp,status,message) \
	if(!(exp)) { return ExStatusHandle(status,__CALLINFO__,message); }

	// 失败则处理异常
#define handle_if_failed(hr,message) \
	{ HRESULT _HR_ = hr; if(FAILED(_HR_)){ return ExStatusHandle(_HR_,__CALLINFO__, message); }}

	// 不成功则处理异常
#define handle_if_notok(hr,message) \
	{ HRESULT _HR_ = hr; if(_HR_ != S_OK){ return ExStatusHandle(_HR_,__CALLINFO__, message); }}

}
