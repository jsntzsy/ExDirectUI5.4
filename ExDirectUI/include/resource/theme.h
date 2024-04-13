/**
 * @file theme.h
 * @brief 主题对象头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-04-13
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 主题包信息
	struct ExThemeInfo
	{
		DWORD flags;

		wchar_t name[64];

		wchar_t author[64];

		wchar_t version[32];

		wchar_t description[256];
	};

	/////////////////////////////

	/// 元素透明度信息
	struct ExEleOpacityInfo
	{
		EXCHANNEL normal;
		float disable_percent;
	};


	//////////////////////





}
