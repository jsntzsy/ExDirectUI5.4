/**
 * @file window.h
 * @brief 皮肤窗口定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-05-04
 * @copyright
 */
#pragma once
#include "element/element.h"

namespace ExDirectUI
{
	EXINTERFACE("42E306BA-A4F5-42D8-83FE-981C0FD47941") IExWindow : public IExElement
	{
		EXMETHOD HRESULT EXOBJCALL Show(DWORD mode) PURE;
		EXMETHOD HRESULT EXOBJCALL ShowEx(DWORD mode) PURE;

		EXMETHOD HRESULT EXOBJCALL SetTrayIcon(HICON icon, LPCWSTR tips) PURE;
		EXMETHOD HRESULT EXOBJCALL PopupTrayIcon(LPCWSTR text, LPCWSTR title, DWORD flags) PURE;


	};

}
