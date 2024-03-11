/**
 * @file atom.h
 * @brief 原子号计算头文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-11
 * @copyright
 */
#pragma once

namespace ExDirectUI
{
	/// 原子号类型
	typedef uint32_t EXATOM;
	
	/// 原子号：未知
	constexpr EXATOM EXATOM_UNKNOWN = 0x00000000;

	////////////////////
	
	/**
	 * @brief 原子号_自数据
	 * 通过数据计算原子号
	 * @param data 数据指针
	 * @param size 数据大小
	 * @return 返回对应原子号,失败返回{EXATOM_UNKNOWN}
	 */
	EXATOM EXAPI EXCALL ExAtomData(const void* data, size_t size);

	/**
	 * @brief 原子号_自字符串
	 * 通过字符串计算原子号
	 * @param str 字符串
	 * @return 返回对应原子号,失败返回{EXATOM_UNKNOWN}
	 */
	EXATOM EXAPI EXCALL ExAtomStrCase(LPCWSTR str);

	/**
	 * @brief 原子号_自字符串_忽略大小写
	 * 通过字符串计算原子号
	 * @param str 字符串
	 * @return 返回对应原子号,失败返回{EXATOM_UNKNOWN}
	 */
	EXATOM EXAPI EXCALL ExAtomStrNoCase(LPCWSTR str);

	/**
	 * @brief 原子号_自文件
	 * 通过文件计算原子号
	 * @param file 文件路径
	 * @return 返回对应原子号,失败返回{EXATOM_UNKNOWN}
	 */
	EXATOM EXAPI EXCALL ExAtomFile(LPCWSTR file);

	inline EXATOM EXCALL ExAtom(LPCWSTR str)
	{
		return ExAtomStrNoCase(str);
	}

	template<typename T>
	inline EXATOM EXCALL ExAtom(const T& p)
	{
		return ExAtomData(&p, sizeof(T));
	}

	
}
