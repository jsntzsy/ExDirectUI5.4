/**
 * @file package.h
 * @brief 数据包头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-28
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 数据包类型
	EXENUM(ExPackageType)
	{
		DataPackage,				///< 包类型：数据包
		ThemePackage,				///< 包类型：主题包
	};

	/// 数据包信息
	struct ExPackageInfo
	{
		/// 包类型
		uint16_t type;

		/// 数据包版本
		uint16_t package_ver;

		/// 标识
		uint32_t flags;

		/// 包名称
		wchar_t name[64];

	};

	/// 数据包项目类型
	EXENUM(ExPackageItemType)
	{
		Data,						///< 数据包项目类型：数据
		Text,						///< 数据包项目类型：文本
		Image,						///< 数据包项目类型：图像
		ImageData,					///< 数据包项目类型：图像数据
		Cursor,						///< 数据包项目类型：光标
		Font,						///< 数据包项目类型：字体
		Icon,						///< 数据包项目类型：图标
		Xml,						///< 数据包项目类型：XML
		Script,						///< 数据包项目类型：脚本
	};

	/// 数据包项目信息
	struct ExPackageItemInfo
	{
		/// ID
		EXATOM id;

		/// 类型
		uint32_t type;

		/// 标识
		uint32_t flags;

		/// 尺寸
		uint32_t size;

		/// 用户自定义数据
		uint64_t user_data;

		/// 数据指针
		const byte_t* data;

		/// 包内路径
		wchar_t path[256];
	};

	/// 数据包生成文件标识
	EXENUM(ExPackageBuildFlags)
	{
		None = 0x0000,				///< 数据包生成文件标识：无
		Subdirectories = 0x0001,	///< 数据包生成文件标识：包含子目录
		IncludeHidden = 0x0002,		///< 数据包生成文件标识：包含隐藏文件
		IncludeEmpty = 0x0004,		///< 数据包生成文件标识：包含空文件
	};

	interface IExPackage;

	/// 数据包项枚举回调函数
	typedef bool(CALLBACK* ExPackageItemEnumProc)(const IExPackage* package, EXATOM id,
		const ExPackageItemInfo* info, LPARAM lparam);

	/// 数据包生成筛选回调函数
	typedef bool(CALLBACK* ExPackageBuildFilterProc)(LPCWSTR path, ExPackageItemInfo* item_info,
		const WIN32_FIND_DATAW* find_data, DWORD build_flags, LPARAM lparam);


	EXINTERFACE("2C44BD8E-D4AF-4690-A884-E2E66E412A82") IExPackage : public IExObject
	{
		EXMETHOD HRESULT EXOBJCALL GetPackageInfo(ExPackageInfo * r_info) const PURE;
		EXMETHOD uint32_t EXOBJCALL GetItemCount() const PURE;

		EXMETHOD bool EXOBJCALL HasItem(EXATOM id) const PURE;
		EXMETHOD EXATOM EXOBJCALL FindItem(LPCWSTR path) const PURE;

		EXMETHOD HRESULT EXOBJCALL GetItemInfo(EXATOM id, ExPackageItemInfo* r_info) const PURE;
		EXMETHOD HRESULT EXOBJCALL GetItemData(EXATOM id, const byte_t** r_data, uint32_t* r_size) const PURE;
		EXMETHOD LPCWSTR EXOBJCALL GetItemPath(EXATOM id) const PURE;

		EXMETHOD HRESULT EXOBJCALL EnumItems(ExPackageItemEnumProc proc, LPARAM lparam) const PURE;
	};

	/////////////////////


	/**
	 * @brief 数据包_生成
	 * 生成数据包文件
	 * @param package_info 数据包信息
	 * @param items 数据包项目信息
	 * @param item_count 数据包项目数量
	 * @param key 密码数据指针 @可空
	 * @param key_size 密码数据大小 @可空
	 * @param r_data 接收生成的数据包文件数据
	 * @return 返回执行状态
	 * @attention 生成的数据包文件需要使用{ExDataFree}释放
	*/
	HRESULT EXAPI EXCALL ExPackageBuild(const ExPackageInfo* package_info, const ExPackageItemInfo* items,
		uint32_t item_count, const byte_t* key, uint32_t key_size, ExData* r_data);

	/**
	 * @brief 数据包_生成自目录
	 * 将某个目录下的文件生成为数据包文件
	 * @param package_info 数据包信息
	 * @param directory 目录路径
	 * @param wildcard 文件筛选通配符 @可空<空或空字符串表示所有文件>
	 * @param build_flags 文件生成标识 #*ExPackageBuildFlags,数据包生成文件标识#
	 * @param filter_proc 文件筛选回调函数 @可空<空则表示不筛选>
	 * @param lparam 回调函数参数 @可空
	 * @param key 密码数据指针 @可空
	 * @param key_size 密码数据大小 @可空
	 * @param r_data 接收生成的数据包文件数据
	 * @return 返回执行状态
	 * @attention 生成的数据包文件需要使用{ExDataFree}释放
	*/
	HRESULT EXAPI EXCALL ExPackageBuildFromDirectory(const ExPackageInfo* package_info, LPCWSTR directory,
		LPCWSTR wildcard, DWORD build_flags, ExPackageBuildFilterProc filter_proc, LPARAM lparam,
		const byte_t* key, uint32_t key_size, ExData* r_data);

	/**
	 * @brief 数据包_加载
	 * 加载一个数据包文件
	 * @param package_data 数据包文件数据指针
	 * @param package_size 数据包文件数据大小
	 * @param key 密码数据指针 @可空
	 * @param key_size 密码数据大小 @可空
	 * @param r_package 接收数据包对象指针
	 * @return 返回执行状态
	*/
	HRESULT EXAPI EXCALL ExPackageLoad(const byte_t* package_data, uint32_t package_size,
		const byte_t* key, uint32_t key_size, IExPackage** r_package);



}
