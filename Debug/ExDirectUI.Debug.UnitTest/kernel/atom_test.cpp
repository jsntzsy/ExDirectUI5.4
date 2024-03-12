/**
 * @file atom_test.cpp
 * @brief 原子号计算测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-01-20
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME Ex_Kernel_Atom_Test

class TEST_MODULE_NAME : public ::testing::Test
{
protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
	}
};

TEST_F(TEST_MODULE_NAME, DataAtomCompute)
{
	byte data[] = {11, 22, 33, 44, 55, 66, 77, 88, 99, 100, 0, 0, 1, 2, 3, 4, 5, 6};
	size_t size = sizeof(data);

	ASSERT_EQ(ExAtomData(0, size), EXATOM_UNKNOWN);
	ASSERT_EQ(ExAtomData(data, 0), EXATOM_UNKNOWN);
	ASSERT_EQ(ExAtomData(data, size), 0x2ADE1677);
}

TEST_F(TEST_MODULE_NAME, StringAtomCompute)
{
	ASSERT_EQ(ExAtomStrCase(nullptr), EXATOM_UNKNOWN);
	ASSERT_EQ(ExAtomStrNoCase(nullptr), EXATOM_UNKNOWN);

	LPCWSTR str = L"abcABC@@";
	LPCWSTR str2 = L"abcabc@@";
	ASSERT_EQ(ExAtomStrCase(str), 0xB18C64CB);
	ASSERT_NE(ExAtomStrCase(str2), 0xB18C64CB);
	ASSERT_EQ(ExAtomStrCase(str2), 0x000B007B);

	ASSERT_EQ(ExAtomStrNoCase(str), 0x000B007B);
	ASSERT_EQ(ExAtomStrNoCase(str2), 0x000B007B);
}

TEST_F(TEST_MODULE_NAME, FileAtomCompute)
{
	LPCWSTR file = L"../../../../_res/1.jpg";

	ASSERT_EQ(ExAtomFile(nullptr), EXATOM_UNKNOWN);
	ASSERT_EQ(ExAtomFile(L"UNKNOWN_FILE"), EXATOM_UNKNOWN);

	ASSERT_EQ(ExAtomFile(file), 0xBEB7498F);


}
