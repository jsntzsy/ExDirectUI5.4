/**
 * @file atom.cpp
 * @brief 原子号计算实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */

#include "stdafx.h"
#include "kernel/atom.h"
#include "src/kernel/winapi.h"

#include <fstream>

namespace ExDirectUI
{
	inline EXATOM _ExAtom(const void* data, size_t size)
	{
		EXATOM atom = _RtlComputeCrc32(0, data, (uint32_t)size);
		return atom == EXATOM_UNKNOWN ? (EXATOM_UNKNOWN + 1) : atom;
	}

	EXATOM EXAPI EXCALL ExAtomData(const void* data, size_t size)
	{
		ExAssert(_RtlComputeCrc32);
		CHECK_PARAM_RET(data, EXATOM_UNKNOWN);
		CHECK_PARAM_RET(size > 0, EXATOM_UNKNOWN);

		return _ExAtom(data, size);
	}
	EXATOM EXAPI EXCALL ExAtomStrCase(LPCWSTR str)
	{
		ExAssert(_RtlComputeCrc32);
		CHECK_PARAM_RET(str, EXATOM_UNKNOWN);

		return _ExAtom(str, wcslen(str) * sizeof(wchar_t));
	}
	EXATOM EXAPI EXCALL ExAtomStrNoCase(LPCWSTR str)
	{
		ExAssert(_RtlComputeCrc32);
		CHECK_PARAM_RET(str, EXATOM_UNKNOWN);

		//预处理
		wchar_t buffer[EX_CFG_SIZEOF_ATOM_STR]{};
		wchar_t* buffer_ptr = buffer;
		size_t len = wcslen(str);

		//如果缓冲区不够放,则分配新的缓冲区
		if (len >= _countof(buffer)) {
			buffer_ptr = new wchar_t[len + 1];
		}

		//拷贝到缓冲区,并全部转小写
		wcsncpy_s(buffer_ptr, len + 1, str, len);
		CharLowerW(buffer_ptr);

		//计算原子号
		EXATOM atom = _ExAtom(buffer_ptr, len * sizeof(wchar_t));

		//如果之前动态分配过,则释放掉
		if (buffer_ptr != buffer) {
			delete[] buffer_ptr;
		}

		return atom;
	}
	EXATOM EXAPI EXCALL ExAtomFile(LPCWSTR file)
	{
		ExAssert(_RtlComputeCrc32);
		CHECK_PARAM_RET(file, EXATOM_UNKNOWN);
		
		//打开文件
		std::fstream fs;
		fs.open(file, std::ios::in | std::ios::binary);
		throw_if_false(fs.is_open(), EE_IO, L"打开文件失败");

		//循环读入缓冲,并计算原子号
		EXATOM atom = 0;
		char buffer[EX_CFG_SIZEOF_ATOM_FILE]{};
		while (fs.read(buffer, sizeof(buffer))) {
			atom = _RtlComputeCrc32(atom, buffer, fs.gcount());
		}
		
		//如果读取失败,则返回未知原子号
		if (!fs.eof()) { return EXATOM_UNKNOWN; }
		
		return atom == EXATOM_UNKNOWN ? (EXATOM_UNKNOWN + 1) : atom;
	}
}


