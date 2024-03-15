/**
 * @file module.h
 * @brief 模块管理头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 模块文件信息
	struct ExModuleFileInfo
	{
		const void* file;		///< 文件数据
		WPARAM wparam;			///< 参数1
		LPARAM lparam;			///< 参数2
	};

	/// 模块文件加载器
	struct ExModuleFileLoader
	{
		/// 加载模块
		HMODULE(WINAPI* LoadModule)(const void* file, WPARAM waram, LPARAM lparam);

		/// 释放模块
		bool(WINAPI* FreeModule)(HMODULE module_handle);

		/// 获取模块函数
		FARPROC(WINAPI* GetProcAddress)(HMODULE module_handle, LPCSTR proc_name);
	};

	/// 模块类型
	enum ExModuleType
	{
		EX_MODULE_UNKNOWN,				///< 模块类型：未知
		EX_MODULE_IMAGE_DECODER,		///< 模块类型：图像解码器
		EX_MODULE_RENDER,				///< 模块类型：渲染器
	};

	/// 模块标志
	enum ExModuleFlags
	{
		EX_MODULE_FLAG_NONE = 0x0000,			///< 模块标志：无
		EX_MODULE_FLAG_CAN_UNLOAD = 0x0001,		///< 模块标志：可卸载
	};

	/// 模块信息
	struct ExModuleInfo
	{
		/// 模块ID
		EXATOM id;

		/// 模块类型
		uint16_t type;

		/// 模块标志
		uint16_t flags;

		/// 模块名称
		wchar_t name[64];

		/// 模块版本
		wchar_t version[32];

		/// 模块作者
		wchar_t author[32];

		/// 模块描述
		wchar_t description[256];
	};

	class IExModuleUtils;
	class IExModule;

	/// 模块入口函数类型
	typedef HRESULT(APIENTRY* ExModuleEntryProc)(HMODULE module_handle, IExModuleUtils* utils,
		ExModuleInfo* r_info, IExModule** r_module);

	/////////////////////////////

	EXINTERFACE("D3888235-E4C6-4E0F-B26D-B85CA5202B11") IExModule : public IExObject
	{
		EXMETHOD EXATOM EXOBJCALL GetId() const PURE;
		EXMETHOD uint16_t EXOBJCALL GetType() const PURE;
		EXMETHOD HRESULT EXOBJCALL GetInfo(ExModuleInfo* r_info) const PURE;
		EXMETHOD HRESULT EXOBJCALL Invoke(uint32_t code, WPARAM wparam, LPARAM lparam, LRESULT* r_result) PURE;
		EXMETHOD HRESULT EXOBJCALL OnModuleUnLoad() PURE;
	};

	interface IExDecodeImage;

	EXINTERFACE("4BB3ED43-DD75-4679-882B-3E137D592496") IExModuleUtils : public IUnknown
	{
		EXMETHOD HRESULT EXOBJCALL DecodeImageFile(LPCWSTR file, IExDecodeImage * *r_image) PURE;
		EXMETHOD HRESULT EXOBJCALL DecodeImageMemory(const byte_t* data, size_t size, IExDecodeImage** r_image) PURE;
	};

	////////////////////////////

	HRESULT EXAPI EXCALL ExModuleLoadFromFile(const void* file, WPARAM waram, LPARAM lparam, EXATOM* r_module_id);
	HRESULT EXAPI EXCALL ExModuleLoadFromHandle(HMODULE module_handle, EXATOM* r_module_id);
	HRESULT EXAPI EXCALL ExModuleLoadFromEntry(ExModuleEntryProc entry_proc, EXATOM* r_module_id);

	HRESULT EXAPI EXCALL ExModuleUnLoad(EXATOM module_id);

	HRESULT EXAPI EXCALL ExModuleGetInfo(EXATOM module_id, ExModuleInfo* r_info);
	HRESULT EXAPI EXCALL ExModuleInvoke(EXATOM module_id, uint32_t msg, WPARAM wparam, LPARAM lparam, LRESULT* r_result);


}
