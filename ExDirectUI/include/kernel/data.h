/**
 * @file data.h
 * @brief 数据块管理头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 数据块
	struct ExData
	{
		/// 数据指针
		byte_t* data;

		/// 数据大小
		size_t size;
	};

	/////////////////////

	/**
	 * @brief 数据块_申请
	 * 申请一个数据块
	 * @param size 数据块尺寸
	 * @param r_data 接收创建的数据块
	 * @return 返回执行状态
	 * @attention 创建的数据块使用完毕后，调用{ExDatFree}释放
	 */
	HRESULT EXAPI EXCALL ExDataAlloc(size_t size, ExData* r_data);

	/**
	 * @brief 数据块_释放
	 * 释放一个数据块
	 * @param r_data 数据块指针
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExDataFree(ExData* r_data);

	/**
	 * @brief 数据块_调整尺寸
	 * 调整数据块尺寸
	 * @param data 数据块指针
	 * @param new_size 新尺寸
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExDataResize(ExData* data, size_t new_size);

	/**
	 * @brief 数据块_复制
	 * 复制数据块
	 * @param data 源数据块
	 * @param r_new_data 接收新数据块
	 * @param new_size 新尺寸 @可空(0则与源相同)
	 * @return 返回执行状态
	 * @attention 创建的数据块使用完毕后，调用{ExDatFree}释放
	 */
	HRESULT EXAPI EXCALL ExDataCopy(ExData* data, ExData* r_new_data, size_t new_size = 0);

	/**
	 * @brief 数据块_读取文件
	 * 将文件读取到一个数据块中
	 * @param file 文件路径
	 * @param r_data 接收文件数据块
	 * @return 返回执行状态
	 * @attention 读入的文件使用完毕后，调用{ExDatFree}释放
	 */
	HRESULT EXAPI EXCALL ExDataReadFile(LPCWSTR file, ExData* r_data);

	/**
	 * @brief 数据块_写入文件
	 * 将数据块写入到文件中
	 * @param file 文件路径
	 * @param data 数据块 @可空(空则写出空文件)
	 * @param size 数据块尺寸 @可空(0则写出空文件)
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExDataWriteFile(LPCWSTR file, const byte_t data, size_t size);

	/**
	 * @brief 数据块_取文件尺寸
	 * 获取文件尺寸
	 * @param file 文件路径
	 * @param r_size 接收文件尺寸
	 * @return 返回执行状态
	 */
	HRESULT EXAPI EXCALL ExDataGetFileSize(LPCWSTR file, size_t* r_size);
	
}
