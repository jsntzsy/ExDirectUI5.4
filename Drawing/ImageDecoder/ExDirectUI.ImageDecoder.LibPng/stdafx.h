/**
 * @file stdafx.h
 * @brief 预编译头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-12
 * @copyright
 */
#pragma once

#define EX_CFG_BUILD_IN_LIB

#include <Windows.h>

#include "ExDirectUI.h"
using namespace ExDirectUI;
#pragma comment(lib,"ExDirectUI.lib")

#include "apng_process.h"
#include "decode_image.h"
#include "image_decoder.h"

#define ATOM_EXDIRECTUI_IMAGEDECODER_LIBPNG				0x3AB97817		// 原子号: exdirectui.imagedecoder.libpng

