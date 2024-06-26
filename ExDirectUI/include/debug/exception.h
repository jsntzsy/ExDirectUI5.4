﻿/**
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
	/// ExDirectUI异常
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

		inline HRESULT handle(bool only_failed = false) const
		{
			if (!only_failed || is_failed())
			{
				return ExStatusHandle(m_status,
					m_file.size() != 0 ? m_file.c_str() : nullptr, m_line,
					m_message.c_str()
				);
			}
			return m_status;
		}

	private:
		HRESULT m_status;
		std::wstring m_message;
		std::wstring m_file;
		int m_line;
	};

#define throw_ex(status,message)	throw ExException(status, message, __CALLINFO__)						// 抛出异常
#define throw_if_false(exp,status,message) if(!(exp)) { throw_ex(status,message); }							// 错误则抛出异常
#define throw_if_failed(exp,message) { HRESULT _HR_ = exp; if(FAILED(_HR_)){ throw_ex(_HR_, message); }}	// 失败则抛出异常
#define throw_if_notok(exp,message) { HRESULT _HR_ = exp; if(_HR_ != S_OK){ throw_ex(_HR_, message); }}		// 不成功则抛出异常

#define catch_return(todo)		catch(ExException& e) { todo; return e.handle(); }							// 捕获异常并返回
#define catch_continue(todo)	catch(ExException& e) { todo; e.handle(); }									// 捕获异常并继续执行
#define catch_ignore(todo)		catch(ExException& e) { UNREFERENCED_PARAMETER(e); todo; }					// 捕获异常但忽略
#define catch_throw(todo)		catch(ExException& e) { UNREFERENCED_PARAMETER(e); todo; throw; }			// 捕获异常并继续抛出
#define catch_default			catch_return																// 默认捕获异常

#define try_default(try_block,todo)		try{ try_block; } catch_default(todo);								// 简单异常捕获
#define try_continue(try_block,todo)	try{ try_block; } catch_continue(todo);								// 捕获异常并继续执行
#define try_ignore(try_block,todo)		try{ try_block; } catch_ignore(todo);								// 捕获异常但忽略

#define handle_ex(status,message)			return ExStatusHandle(status,__CALLINFO__,message)				// 处理异常
#define handle_continue(status,message)		ExStatusHandle(status,__CALLINFO__,message)						// 处理异常

	// 错误则处理异常
#define handle_if_false(exp,status,message,todo) \
	if(!(exp)) { todo; return ExStatusHandle(status,__CALLINFO__,message); }

	// 失败则处理异常
#define handle_if_failed(exp,message,todo) \
	{ HRESULT _HR_ = exp; if(FAILED(_HR_)){ todo; return ExStatusHandle(_HR_,__CALLINFO__, message); }}

	// 不成功则处理异常
#define handle_if_notok(exp,message,todo) \
	{ HRESULT _HR_ = exp; if(_HR_ != S_OK){ todo; return ExStatusHandle(_HR_,__CALLINFO__, message); }}

	//错误则返回
#define return_if_false(exp, todo, ret)	if(!(exp)){todo; return ret;}

	//失败则返回
#define return_if_failed(exp, todo) { HRESULT _HR_ = exp; if(FAILED(_HR_)){ todo; return _HR_; }}

	//不成功则返回
#define return_if_notok(exp, todo) { HRESULT _HR_ = exp;  if(_HR_ != S_OK){ todo; return _HR_; }}

}
