/**
 * @file apng_process.h
 * @brief 图像解析函数定义文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright 
 */
#pragma once

namespace ExDirectUI
{
	//apng图像信息
	struct ExAPngImageInfo
	{
		EXBITSDATA	frames_data;
		uint32_t*	delays;
		uint32_t	width;
		uint32_t	height;
		uint32_t	frame_count;
		int			loop_count;
	};

	ExAPngImageInfo* ExAPngLoadFromMemory(byte_t* data, size_t size) MAYTHROW;
	ExAPngImageInfo* ExAPngLoadFromFile(LPCWSTR file) MAYTHROW;
	void ExAPngFree(ExAPngImageInfo* info);

}

