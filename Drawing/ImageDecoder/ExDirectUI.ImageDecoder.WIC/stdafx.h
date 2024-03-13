/**
 * @file stdafx.h
 * @brief 预编译头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */
#pragma once

#define EX_CFG_BUILD_IN_LIB

#include <Windows.h>

#include "ExDirectUI.h"
using namespace ExDirectUI;
#pragma comment(lib,"ExDirectUI.lib")

#define ATOM_EXDIRECTUI_IMAGEDECODER_WIC				0x53922A6E		// 原子号: exdirectui.imagedecoder.wic

#include <wincodec.h>
#pragma comment(lib,"WindowsCodecs.lib")

#include "decode_image.h"
#include "image_decoder.h"


