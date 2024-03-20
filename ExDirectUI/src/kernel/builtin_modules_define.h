/**
 * @file builtin_modules_define.h
 * @brief 内置模块定义头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-13
 * @copyright
 */
#pragma once
#include "config.h"

//去掉注释则停用内置模块
// 如出现找不到ExDirectUI.lib或无法覆盖的错误，请取消注释编译一次后再注释编译一至两次即可

//#undef EX_CFG_MODULE_ENABLE_BUILTIN


#ifdef EX_CFG_MODULE_ENABLE_BUILTIN

#define EX_BUILT_MODULE_IMAGEDECODER_1_ENTRY		_ExDirectUI_ImageDecoder_LibPng_
#define EX_BUILT_MODULE_IMAGEDECODER_1_FILE			"ExDirectUI.ImageDecoder.LibPng.lib"
#define EX_BUILT_MODULE_IMAGEDECODER_1_PRELOAD		true


//#define EX_BUILT_MODULE_IMAGEDECODER_2_ENTRY		_ExDirectUI_ImageDecoder_XXX_
//#define EX_BUILT_MODULE_IMAGEDECODER_2_FILE		"ExDirectUI.ImageDecoder.XXX.lib"
//#define EX_BUILT_MODULE_IMAGEDECODER_2_PRELOAD	true


//#define EX_BUILT_MODULE_IMAGEDECODER_3_ENTRY		_ExDirectUI_ImageDecoder_XXX_
//#define EX_BUILT_MODULE_IMAGEDECODER_3_FILE		"ExDirectUI.ImageDecoder.XXX.lib"
//#define EX_BUILT_MODULE_IMAGEDECODER_3_PRELOAD	true

//#define EX_BUILT_MODULE_RENDER_ENTRY				_ExDirectUI_Render_XXX_
//#define EX_BUILT_MODULE_RENDER_FILE				"ExDirectUI.Render.XXX.lib"
//#define EX_BUILT_MODULE_RENDER_PRELOAD			true





#endif // EX_CFG_MODULE_ENABLE_BUILTIN

