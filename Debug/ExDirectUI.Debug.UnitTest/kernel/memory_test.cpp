/**
 * @file memory_test.cpp
 * @brief 内存管理测试文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-01-17
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME    Ex_Kernel_Memory_Test

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

TEST_F(TEST_MODULE_NAME, Test)
{
	LPVOID p = ExMemAlloc(10);
	ASSERT_NE(p, nullptr);
	ASSERT_FALSE(::IsBadWritePtr(p, 10));

	ASSERT_EQ(ExMemGetSize(p), 10);

	ExMemFree(p);

	p = ExMemAlloc(100000000000);
	ASSERT_EQ(p, nullptr);

	p = ExMemAlloc(100);
	ASSERT_NE(p, nullptr);
	ASSERT_EQ(ExMemGetSize(p), 100);

	p = ExMemReAlloc(p, 50);
	ASSERT_NE(p, nullptr);
	ASSERT_EQ(ExMemGetSize(p), 50);

	p = ExMemReAlloc(p, 200);
	ASSERT_NE(p, nullptr);
	ASSERT_EQ(ExMemGetSize(p), 200);

	ExMemFree(p);

}



