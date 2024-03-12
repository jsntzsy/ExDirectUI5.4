/**
 * @file data_test.cpp
 * @brief 数据块管理测试文件
 * @author EternalZSY
 * @version 5.3.0.0
 * @date 2024-01-19
 * @copyright
 */

#include "pch.h"

#define TEST_MODULE_NAME    Ex_Kernel_Data_Test

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

TEST_F(TEST_MODULE_NAME, AllocAndFree)
{
	ExData data{};
	ExData data2{};

	ASSERT_EQ(ExDataAlloc(10, nullptr), E_INVALIDARG);

	ASSERT_EQ(ExDataAlloc(0, &data), S_OK);
	ASSERT_EQ(data.data, nullptr);
	ASSERT_EQ(data.size, 0);

	ASSERT_EQ(ExDataAlloc(10, &data), S_OK);
	ASSERT_NE(data.data, nullptr);
	ASSERT_EQ(data.size, 10);

	ASSERT_EQ(ExDataResize(nullptr, 0), E_INVALIDARG);
	ASSERT_EQ(ExDataResize(&data, 0), S_OK);
	ASSERT_EQ(data.data, nullptr);
	ASSERT_EQ(data.size, 0);

	ASSERT_EQ(ExDataResize(&data, 20), S_OK);
	ASSERT_NE(data.data, nullptr);
	ASSERT_EQ(data.size, 20);

	ASSERT_EQ(ExDataCopy(nullptr, &data2, 0), E_INVALIDARG);
	ASSERT_EQ(ExDataCopy(&data, nullptr, 0), E_INVALIDARG);
	ASSERT_EQ(ExDataCopy(&data, &data2, 0), S_OK);
	ASSERT_EQ(data2.size, data.size);
	ASSERT_NE(data2.data, data.data);
	ASSERT_TRUE(memcmp(data2.data, data.data, min(data.size, data2.size)) == 0);
	ExDataFree(&data2);

	ASSERT_EQ(ExDataCopy( &data, &data2, 40), S_OK);
	ASSERT_EQ(data2.size, 40);
	ASSERT_NE(data2.data, data.data);

	ASSERT_EQ(ExDataResize(&data2, 30), S_OK);
	ASSERT_EQ(data2.size, 30);
	ASSERT_TRUE(memcmp(data2.data, data.data, min(data.size, data2.size)) == 0);
	ExDataFree(&data2);

	ASSERT_EQ(ExDataFree(nullptr), E_INVALIDARG);
	ASSERT_EQ(ExDataFree(&data), S_OK);
	ASSERT_EQ(data.data, nullptr);
	ASSERT_EQ(data.size, 0);

}

TEST_F(TEST_MODULE_NAME, ReadWriteFile)
{
	ExData data{};
	const LPCWSTR file = L"../../../../_res/1.png";

	ASSERT_EQ(ExDataReadFile(nullptr, &data), E_INVALIDARG);
	ASSERT_EQ(ExDataReadFile(file, nullptr), E_INVALIDARG);

	ASSERT_EQ(ExDataReadFile(L"___no_exists's_file___", &data), EE_IO);

	ASSERT_EQ(ExDataReadFile(file, &data), S_OK);
	ASSERT_NE(data.data, nullptr);
	ASSERT_EQ(data.size, 400);

	ASSERT_EQ(ExDataWriteFile(nullptr, data.data, data.size), E_INVALIDARG);

	ASSERT_EQ(ExDataWriteFile(L"../../../../__NO_EXISTS_DIR__/1.png", data.data, data.size), EE_IO);
	ASSERT_EQ(ExDataWriteFile(L"../../../../__build/output/1.png", data.data, data.size), S_OK);

	ASSERT_EQ(ExDataWriteFile(L"../../../../__build/output/2.txt", nullptr, data.size), S_OK);
	ASSERT_EQ(ExDataWriteFile(L"../../../../__build/output/3.txt", data.data, 0), S_OK);

	ASSERT_EQ(ExDataFree(&data), S_OK);
}


