/**
 * @file data.cpp
 * @brief 数据块实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "kernel/data.h"

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExDataAlloc(size_t size, ExData* r_data)
	{
		CHECK_PARAM(r_data);

		//申请内存
		byte_t* data = (byte_t*)exalloc(size);
		handle_if_false(data, EE_MEM_ALLOC_FAILED, L"申请数据块内存失败");

		//设置到数据块
		r_data->data = data;
		r_data->size = size;
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataFree(ExData* r_data)
	{
		CHECK_PARAM(r_data);

		//如果数据块不为空，则释放内存
		if (r_data->data) {
			exfree(r_data->data);
			r_data->data = nullptr;
			r_data->size = 0;
		}

		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataResize(ExData* data, size_t new_size)
	{
		CHECK_PARAM(data);
		if (new_size == data->size) { return S_OK; }

		//重新申请内存
		byte_t* new_data = (byte_t*)exrealloc(data->data, new_size);
		handle_if_false(new_data, EE_MEM_ALLOC_FAILED, L"重新分配数据块内存失败");

		//设置到数据块
		data->data = new_data;
		data->size = new_size;
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataCopy(ExData* data, ExData* r_new_data, size_t new_size)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(r_new_data);

		//默认值处理
		if (new_size == 0) { new_size = data->size; }
		if (new_size == 0 || data->data == nullptr || data->size == 0) { 
			r_new_data->data = nullptr;
			r_new_data->size = 0;
			return S_OK; 
		}

		//申请内存
		byte_t* new_data = (byte_t*)exalloc(new_size);
		handle_if_false(new_data, EE_MEM_ALLOC_FAILED, L"申请数据块内存失败");

		//拷贝数据
		memcpy(new_data, data->data, min(data->size, new_size));

		//设置到数据块
		r_new_data->data = new_data;
		r_new_data->size = new_size;
		return S_OK;
	}
}


