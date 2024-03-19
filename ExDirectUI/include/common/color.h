/**
 * @file color.h
 * @brief 颜色定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright 
 */
 #pragma once

namespace ExDirectUI
{
	/// ARGB颜色
	typedef uint32_t EXARGB;

	/// RGB颜色
	typedef uint32_t EXRGB;

	/// 颜色通道
	typedef uint8_t EXCHANNEL;

	/// 像素数据指针
	typedef EXCHANNEL* EXBITSDATA;


	/// RGB色转ARGB色
	inline EXARGB RGB2ARGB(EXRGB rgb, EXCHANNEL alpha = 0xFF)
	{
		EXCHANNEL* ch = (EXCHANNEL*)&rgb;
		return (EXARGB)((alpha << 24) | (ch[0] << 16) | (ch[1] << 8) | ch[2]);
	}

	/// ARGB色转RGB色
	inline EXRGB ARGB2RGB(EXARGB argb, EXCHANNEL* alpha = nullptr)
	{
		EXCHANNEL* ch = (EXCHANNEL*)&argb;
		if (alpha) { *alpha = ch[3]; }
		return (EXRGB)((ch[0] << 16) | (ch[1] << 8) | ch[2]);
	}

	/// 生成ARGB颜色
	inline EXARGB MAKEARGB(EXCHANNEL r, EXCHANNEL g, EXCHANNEL b, EXCHANNEL a = 0xFF)
	{
		return (EXARGB)((a << 24) | (r << 16) | (g << 8) | b);
	}

	/// 取透明通道值
	inline EXCHANNEL ALPHA(EXARGB argb)
	{
		return (EXCHANNEL)(argb >> 24);
	}


#pragma region 常用颜色值

#define COLOR_TRANSPARENT	0x00000000		///<颜色：透明
#define COLOR_DEFAULT		0x00110101		///<颜色：默认
#define COLOR_UNDEFINE		0x00120202		///<颜色：未定义
#define COLOR_USER			0x00130303		///<颜色：用户
#define COLOR_AUTO			0x00140404		///<颜色：自动

#define COLOR_BLACK			0xFF000000		///<颜色：黑色
#define COLOR_WHITE			0xFFFFFFFF		///<颜色：白色
#define COLOR_RED			0xFFFF0000		///<颜色：红色
#define COLOR_GREEN			0xFF00FF00		///<颜色：绿色
#define COLOR_BLUE			0xFF0000FF		///<颜色：蓝色
#define COLOR_MAGENTA		0xFFFF00FF		///<颜色：品红
#define COLOR_YELLOW		0xFFFFFF00		///<颜色：黄色


#define ALPHA_TRANSPARENT	0x00			///<透明度：透明
#define ALPHA_OPAQUE		0xFF			///<透明度：不透明
#define ALPHA_HALF			0x80			///<透明度：半透明

#pragma endregion


}
