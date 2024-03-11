/**
 * @file config.h
 * @brief 配置项声明头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 * @copyright
 */
#pragma once

#pragma region 引擎生成配置项

 // 配置项：生成为静态库
 //#define EX_CFG_BUILD_TO_LIB

#pragma endregion

#pragma region 模块管理配置项

// 配置项：加载DLL模块
#define EX_CFG_MODULE_LOAD_DLL

// 配置项：加载LIB模块
#define EX_CFG_MODULE_LOAD_LIB

#pragma endregion

#pragma region 调试功能配置项
#ifdef _DEBUG

// 配置项：调试模式
#define EX_CFG_DEBUG

#ifdef EX_CFG_DEBUG

// 配置项：允许中断
#define EX_CFG_DEBUG_INTERRUPT

// 配置项：记录内存泄露
#define EX_CFG_DEBUG_MEMORY_LEAK

// 配置项：允许输出日志
#define EX_CFG_DEBUG_LOG

// 配置项：生成调用信息
#define EX_CFG_DEBUG_CALL_INFO

// 配置项：检查参数
#define EX_CFG_DEBUG_CHECK_PARAM


#endif // EX_CFG_DEBUG
#endif // _DEBUG
#pragma endregion


#pragma region 缓冲区尺寸配置项

// 配置项：文本取原子缓冲区尺寸
#define EX_CFG_SIZEOF_ATOM_STR	128

// 配置项：文件取原子缓冲区尺寸
#define EX_CFG_SIZEOF_ATOM_FILE	1024

// 配置项：格式化文本缓冲区尺寸
#define EX_CFG_SIZEOF_FORMAT_BUF 1024

// 配置项：类名最大尺寸
#define EX_CFG_SIZEOF_CLASS_NAME 128

#pragma endregion

#pragma region 默认类名

// 配置项：默认窗口类名
#define EX_CFG_WINDOW_CLASS		L"ExDirectUI.Window"

// 配置项：引擎消息
#define EX_CFG_WINDOW_MESSAGE	L"ExDirectUI.Message"

#pragma endregion







#pragma region 忽略警告配置项

// “=”: 从“XX”转换到“YY”，可能丢失数据
#pragma warning(disable: 4244) 

// “XXX”: 未引用的标签
#pragma warning(disable: 4102) 

// 非成员运算符 new 或 delete 函数均不可声明为内联函数
#pragma warning(disable: 4595) 

//类函数宏的调用“XXX”参数不足
#pragma warning(disable: 4003)

//“XXX”: 类型名称以前使用“class”现在使用的是“struct”
#pragma warning(disable: 4099)

#pragma endregion



















