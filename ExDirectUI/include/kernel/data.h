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

	HRESULT EXAPI EXCALL ExDataAlloc(size_t size, ExData* r_data);
	HRESULT EXAPI EXCALL ExDataFree(ExData* r_data);
	HRESULT EXAPI EXCALL ExDataResize(ExData* data, size_t new_size);
	HRESULT EXAPI EXCALL ExDataCopy(ExData* data, ExData* r_new_data, size_t new_size = 0);

}
