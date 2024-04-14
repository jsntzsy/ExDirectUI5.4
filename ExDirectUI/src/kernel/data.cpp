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
#include <fstream>

namespace ExDirectUI
{
	HRESULT EXAPI EXCALL ExDataAlloc(size_t size, ExData* r_data)
	{
		CHECK_PARAM(r_data);

		if (size == 0) { 
			r_data->data = nullptr; 
			r_data->size = 0; 
			return S_OK; 
		}

		//申请内存
		byte_t* data = (byte_t*)exalloc(size);
		handle_if_false(data, E_OUTOFMEMORY, L"申请数据块内存失败");

		//设置到数据块
		r_data->data = data;
		r_data->size = size;
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataFree(ExData* r_data)
	{
		CHECK_PARAM(r_data);

		//如果数据块不为空且尺寸不为0，则释放内存
		if (r_data->data && r_data->size != 0) {
			exfree(r_data->data);
		}
		
		//清空变量
		r_data->data = nullptr;
		r_data->size = 0;

		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataResize(ExData* data, size_t new_size)
	{
		CHECK_PARAM(data);
		if (new_size == data->size) { return S_OK; }
		if (new_size == 0) {
			if (data->data) {
				exfree(data->data);
				data->data = nullptr;
			}
			data->size = 0;
			return S_OK;
		}

		//重新申请内存
		byte_t* new_data = (byte_t*)exrealloc(data->data, new_size);
		handle_if_false(new_data, E_OUTOFMEMORY, L"重新分配数据块内存失败");

		//设置到数据块
		data->data = new_data;
		data->size = new_size;
		return S_OK;
	}
	
	HRESULT EXAPI EXCALL ExDataCopy(ExData* r_new_data, const byte_t* data_src, size_t size_src, size_t new_size)
	{
		CHECK_PARAM(r_new_data);

		//默认值处理
		if (new_size == 0) { new_size = size_src; }
		if (new_size == 0 || data_src == nullptr || size_src == 0) {
			r_new_data->data = nullptr;
			r_new_data->size = 0;
			return S_OK;
		}

		//申请内存
		byte_t* new_data = (byte_t*)exalloc(new_size);
		handle_if_false(new_data, E_OUTOFMEMORY, L"申请数据块内存失败");

		//拷贝数据
		memcpy(new_data, data_src, min(size_src, new_size));

		//设置到数据块
		r_new_data->data = new_data;
		r_new_data->size = new_size;
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataReadFile(LPCWSTR file, ExData* r_data)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_data);

		//打开文件
		std::fstream fs(file, std::ios::in | std::ios::binary);
		handle_if_false(fs.is_open(), EE_IO, L"打开文件失败");

		//获取文件大小
		fs.seekg(0, std::ios::end);
		size_t size = fs.tellg();

		//申请内存
		byte_t* data = (byte_t*)exalloc(size);
		handle_if_false(data, E_OUTOFMEMORY, L"申请数据块内存失败");

		try
		{
			//读取文件
			fs.seekg(0, std::ios::beg);
			throw_if_false(fs.read((char*)data, size), EE_IO, L"读取文件失败");
			
			//设置到数据块
			r_data->data = data;
			r_data->size = size;
			return S_OK;
		}
		catch_default({ if (data) { exfree(data); } });
	}
	HRESULT EXAPI EXCALL ExDataWriteFile(LPCWSTR file, const byte_t* data, size_t size)
	{
		CHECK_PARAM(file);

		//打开文件
		std::fstream fs(file, std::ios::out | std::ios::binary | std::ios::trunc);
		handle_if_false(fs.is_open(), EE_IO, L"打开文件失败");

		//如果数据为空，则直接关闭文件
		if (data == nullptr || size == 0) {
			fs.close();
			return S_OK;
		}

		//写入文件
		handle_if_false(fs.write((const char*)data, size), EE_IO, L"写入文件失败");
		return S_OK;
	}
	HRESULT EXAPI EXCALL ExDataGetFileSize(LPCWSTR file, size_t* r_size)
	{
		CHECK_PARAM(file);
		CHECK_PARAM(r_size);

		//打开文件
		std::fstream fs(file, std::ios::in | std::ios::binary);
		handle_if_false(fs.is_open(), EE_IO, L"打开文件失败");
		
		//获取文件大小
		fs.seekg(0, std::ios::end);
		*r_size = fs.tellg();
		return S_OK;
	}

	HRESULT EXAPI EXCALL ExDataRc4(byte_t* data, size_t data_size, const byte_t* key, size_t key_size)
	{
		CHECK_PARAM(data);
		CHECK_PARAM(key);
		CHECK_PARAM(key_size > 0);

		int S[256];
		int T[256];
		ULONG i = 0, j = 0;

		for (i = 0; i < 256; i++)
		{
			S[i] = i;
			T[i] = key[i % key_size];
		}

		for (i = 0; i < 256; i++)
		{
			j = (j + S[i] + T[i]) % 256;
			int tmp = S[j];
			S[j] = S[i];
			S[i] = tmp;
		}

		i = 0, j = 0;
		for (size_t p = 0; p < data_size; p++)
		{
			i = (i + 1) % 256;
			j = (j + S[i]) % 256;
			int tmp = S[j];
			S[j] = S[i];
			S[i] = tmp;

			int k = S[(S[i] + S[j]) % 256];
			data[p] = data[p] ^ k;
		}

		return S_OK;
	}
}


